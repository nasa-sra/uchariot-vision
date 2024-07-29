
#include "Detector.h"

Detector::Detector(CameraBase *camera) {
    _camera = camera;
}

std::vector<Detection> ClosestDetector::run(std::vector<Detection>& detections) {

    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    // cv::Mat decimatedDepthFrame;
    // cv::resize(depthFrame, decimatedDepthFrame, cv::Size(), 0.25, 0.25);

    float closest = 1e6;
    int x, y;
    for (int row = 1; row < depthFrame.rows / 2; row++) {
        for (int col = 1; col < depthFrame.cols; col++) {
            float pixel = depthFrame.at<float>(row, col);
            if (pixel > 0 && pixel < closest) {
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

ObjectDetector::ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath) : Detector(camera) {

    Utils::LogFmt("Creating detector with %s", modelPath);

    _model = tflite::FlatBufferModel::BuildFromFile(modelPath.c_str());

    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*_model.get(), resolver)(&_interpreter);
    if (tflite::InterpreterBuilder(*_model.get(), resolver)(&_interpreter) != kTfLiteOk) {
        throw std::runtime_error("Detector::Detector - Failed to build interpreter");
    }
    if (_interpreter->AllocateTensors() != kTfLiteOk) {
        throw std::runtime_error("Detector::Detector - Failed to allocate tensors");
    }
    _interpreter->SetNumThreads(3);
    _interpreter->SetAllowFp16PrecisionForFp32(true);

	if(!readFileContents(labelsPath, _labels)) {
        throw std::runtime_error("Detector::Detector - Could not load labels file");
	}
}

void ObjectDetector::run(std::vector<Detection>& detections) {

    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    int cam_width = src.cols;
    int cam_height = src.rows;

    cv::Mat image;
    cv::resize(colorFrame, image, cv::Size(300,300));
    memcpy(_interpreter->typed_input_tensor<uchar>(0), image.data, image.total() * image.elemSize());

    // std::cout << "tensors size: " << _interpreter->tensors_size() << "\n";
    // std::cout << "nodes size: " << _interpreter->nodes_size() << "\n";
    // std::cout << "inputs: " << _interpreter->inputs().size() << "\n";
    // std::cout << "input(0) name: " << _interpreter->GetInputName(0) << "\n";
    // std::cout << "outputs: " << _interpreter->outputs().size() << "\n";

    _interpreter->Invoke();

    const float* detection_locations = _interpreter->tensor(_interpreter->outputs()[0])->data.f;
    const float* detection_classes=_interpreter->tensor(_interpreter->outputs()[1])->data.f;
    const float* detection_scores = _interpreter->tensor(_interpreter->outputs()[2])->data.f;
    const int num_detections = *_interpreter->tensor(_interpreter->outputs()[3])->data.f;

    //there are ALWAYS 10 detections no matter how many objects are detectable

    for (int i = 0; i < num_detections; i++) {
        if (detection_scores[i] > _confidenceThresh){
            int det_index = (int) detection_classes[i] + 1;
            Detection d;
            d.y1 = detection_locations[4*i] * colorFrame.rows;
            d.x1 = detection_locations[4*i+1] * colorFrame.cols;
            d.y2 = detection_locations[4*i+2] * colorFrame.rows;
            d.x2 = detection_locations[4*i+3] * colorFrame.cols;

            d.score = detection_scores[i];
            d.name = _labels[det_index].c_str();
            d.x = (d.x2 - d.x1) / 2 + d.x1;
            d.y = (d.y2 - d.y1) / 2 + d.y1;
            d.pos = _camera->getCameraPoint(d.x, d.y);
            detections.push_back(d);
        }
    }
}

bool Detector::readFileContents(std::string fileName, std::vector<std::string>& lines) {
	std::ifstream in(fileName.c_str());
	if(!in.is_open()) return false;

	std::string str;
	while (std::getline(in, str)) {
		if(str.size() > 0)
            lines.push_back(str);
	}
	in.close();
	return true;
}