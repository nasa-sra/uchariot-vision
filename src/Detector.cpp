#include "Detector.h"

Detector::Detector(CameraBase *camera)
{
    _camera = camera;
}

std::vector<Detection> ClosestDetector::run(std::vector<Detection> &detections)
{

    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    float closest = 1e6;
    int x, y;
    for (int row = 1; row < depthFrame.rows / 2; row++)
    {
        for (int col = 1; col < depthFrame.cols; col++)
        {
            float pixel = depthFrame.at<float>(row, col);
            if (pixel > 0 && pixel < closest)
            {
                closest = pixel;
                x = col;
                y = row;
            }
        }
    }

    Detection det;
    det.pos = _camera->getCameraPoint(x, y);
    det.name = "closest";
    det.x = x;
    det.y = y;

    std::vector<Detection> detections;
    detections.push_back(det);
    return detections;
}

std::vector<std::string> load_class_list(std::string labelPath)
{
    std::vector<std::string> class_list;
    std::ifstream ifs(labelPath);
    std::string line;
    while (getline(ifs, line))
    {
        class_list.push_back(line);
    }
    return class_list;
}

// maybe not do this because it creates a square image and we probably want to train 640x480
cv::Mat format_yolov5(const cv::Mat &source)
{
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    source.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}

ObjectDetector::ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath, float CONFIDENCE_THRESHOLD) : Detector(camera)
{
    class_list = load_class_list(labelPath);

    auto result = cv::dnn::readNet(modelPath);
    std::cout << "Attempting to use CUDA\n";
    result.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    result.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);

    net = result;
    confidenceThreshold = CONFIDENCE_THRESHOLD;
}

void ObjectDetector::run(std::vector<Detection> &detections)
{
    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    cv::Mat image;
    cv::resize(colorFrame, image, cv::Size(300, 300));

    cv::Mat blob;
    auto input_image = format_yolov5(image);

    cv::dnn::blobFromImage(input_image, blob, 1. / 255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
    net.setInput(blob);
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    float x_factor = input_image.cols / INPUT_WIDTH;
    float y_factor = input_image.rows / INPUT_HEIGHT;

    float *data = (float *)outputs[0].data;

    const int dimensions = 85;
    const int rows = 25200;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i)
    {

        float confidence = data[4];
        if (confidence >= confidenceThreshold)
        {
            float *classes_scores = data + 5;
            cv::Mat scores(1, class_list.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double max_class_score;
            minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            if (max_class_score > SCORE_THRESHOLD)
            {

                confidences.push_back(confidence);

                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];
                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }

        data += 85;
    }

    // non maxiumum supression
    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);
    for (int i = 0; i < nms_result.size(); i++)
    {
        int idx = nms_result[i];
        ObjectDetection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];
        result.box = boxes[idx];
        detections.push_back(result);
    }

    // for (int i = 0; i < numDetections; ++i)
    // {
    //     auto detection = detections[i];
    //     auto box = detection.box;
    //     auto classId = detection.class_id;
    //     const auto color = colors[classId % colors.size()];
    //     cv::rectangle(image, box, color, 3);

    //     cv::rectangle(image, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
    //     cv::putText(image, class_list[classId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
    // }
}
