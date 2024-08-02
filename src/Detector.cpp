#include "Detector.h"

#define CUDA_WARN(XXX) \
    do { if (XXX != cudaSuccess) std::cerr << "CUDA Error: " << \
        cudaGetErrorString(XXX) << ", at line " << __LINE__ \
        << std::endl; cudaDeviceSynchronize(); } while (0)

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

ObjectDetector::ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath, float confThresh) :
    Detector(camera),
    _confidence_threshold(confThresh)
{
    _net = detectNet::Create("", modelPath.c_str(), 0.0, labelPath.c_str(), "", confThresh, "input_0", "scores", "bbox");
    _detections = new detectNet::Detection[_net->GetMaxDetections()];
    CUDA_WARN(cudaMalloc((void**) &_cudaImage, INPUT_WIDTH * INPUT_HEIGHT * sizeof(uchar3)));
	
	if (_net == nullptr) {
		Utils::LogFmt("detectnet:  failed to load detectNet model\n");
	}
 }

 ObjectDetector::~ObjectDetector() {
    SAFE_DELETE(_net);
    delete[] _detections;
    CUDA_WARN(cudaFree((void*)_cudaImage));
 }

void ObjectDetector::run(std::vector<Detection*> &detections) {
    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    cv::Mat resizedImage = colorFrame;
    cv::resize(colorFrame, resizedImage, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));

    float xFactor = colorFrame.cols / INPUT_WIDTH;
    float yFactor = colorFrame.rows / INPUT_HEIGHT;

    CUDA_WARN(cudaMemcpy(_cudaImage, resizedImage.data, INPUT_WIDTH * INPUT_HEIGHT * sizeof(uchar3), cudaMemcpyHostToDevice));

    int numDetections = _net->Detect(_cudaImage, INPUT_WIDTH, INPUT_HEIGHT, IMAGE_RGB8, _detections);
    
    if (numDetections > 0) {
        Utils::LogFmt("%i objects detected", numDetections);
        for(int n = 0; n < numDetections; n++ ) {
            ObjectDetection* det = new ObjectDetection();
            det->name = _net->GetClassDesc(_detections[n].ClassID);

            int left = int(_detections[n].Left * xFactor);
            int right = int(_detections[n].Right * xFactor);
            int top = int(_detections[n].Top * yFactor);
            int bottom = int(_detections[n].Bottom * yFactor);
            det->box = cv::Rect(left, top, right - left, bottom - top);

            det->x = det->box.x + det->box.width / 2;
            det->y = det->box.y + det->box.height / 2;
            det->pos = _camera->getCameraPoint(det->x, det->y);
            det->confidence = _detections[n].Confidence;
            detections.push_back(det);
        }
    }
}
