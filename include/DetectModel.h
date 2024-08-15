#pragma once
#include <NvInfer.h>

class TensorRTLogger : public nvinfer1::ILogger {
    void log(Severity severity, const char* msg) noexcept override {
        Utils::LogFmt("TensorRT: %s", msg);
        // suppress info-level messages
        // if (severity <= Severity::kWARNING)
        //     Utils::LogFmt("TensorRT: %s", msg);
    }
};

class DetectModel {
public:

    DetectModel(std::string modelPath);
    ~DetectModel();

    void runInference(void* inputImage);

private:

    void buildEngine(std::string modelPath);
    void loadEngine(std::string modelPath);

    TensorRTLogger _logger;
    nvinfer1::IRuntime* _runtime;
    nvinfer1::ICudaEngine* _engine;
    nvinfer1::IExecutionContext *_context;

    char* _outputBuffer;

};