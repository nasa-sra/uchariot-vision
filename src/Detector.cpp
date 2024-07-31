#include "Detector.h"

Detector::Detector(CameraBase *camera)
{
    _camera = camera;
}

void ClosestDetector::run(std::vector<Detection*> &detections) {

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

    Detection* det = new Detection();
    det->pos = _camera->getCameraPoint(x, y);
    det->name = "closest";
    det->x = x;
    det->y = y;

    detections.push_back(det);
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

ObjectDetector::ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath, float confThresh, float scoreThresh, float NMSThresh) :
    Detector(camera),
    _confidence_threshold(confThresh),
    _score_threshold(scoreThresh),
    _NMS_threshold(NMSThresh)
{
    _labels = load_class_list(labelPath);

    auto result = cv::dnn::readNet(modelPath);
    Utils::LogFmt("Attempting to use CUDA for object detector");
    result.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    result.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);

    _net = result;
}

void ObjectDetector::run(std::vector<Detection*> &detections)
{
    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    cv::Mat image;
    cv::resize(colorFrame, image, cv::Size(300, 300));

    cv::Mat blob;
    auto input_image = format_yolov5(image);

    cv::dnn::blobFromImage(input_image, blob, 1. / 255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
    _net.setInput(blob);
    std::vector<cv::Mat> outputs;
    _net.forward(outputs, _net.getUnconnectedOutLayersNames());

    float x_factor = input_image.cols / INPUT_WIDTH;
    float y_factor = input_image.rows / INPUT_HEIGHT;

    float *data = (float *)outputs[0].data;

    const int OUTPUT_COLUMNS = 85;
    const int OUTPUT_ROWS = 25200;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < OUTPUT_ROWS; ++i) {
        float confidence = data[4];
        if (confidence >= _confidence_threshold) {
            float* classes_scores = data + 5;
            cv::Mat scores(1, _labels.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double max_class_score;
            cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            if (max_class_score > _score_threshold) {

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

        data += OUTPUT_COLUMNS;
    }

    // non maxiumum supression
    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, _score_threshold, _NMS_threshold, nms_result);
    for (int i = 0; i < nms_result.size(); i++) {
        int idx = nms_result[i];
        ObjectDetection* det = new ObjectDetection();
        det->name = _labels[class_ids[idx]];
        det->x = boxes[idx].x + boxes[idx].width / 2;
        det->y = boxes[idx].y + boxes[idx].height / 2;
        det->pos = _camera->getCameraPoint(det->x, det->y);
        det->confidence = confidences[idx];
        det->box = boxes[idx];
        detections.push_back(det);
    }
}
