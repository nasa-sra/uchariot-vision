
#include <filesystem>
#include <fstream>
#include <NvOnnxParser.h>

#include "Utils.h"
#include "DetectModel.h"

#define CUDA_WARN(XXX) \
    do { if (XXX != cudaSuccess) std::cerr << "CUDA Error: " << \
        cudaGetErrorString(XXX) << ", at line " << __LINE__ \
        << std::endl; cudaDeviceSynchronize(); } while (0)

DetectModel::DetectModel(std::string modelPath) {
    try {
        std::filesystem::path path(modelPath);

        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("Model does not exist at path");
        }

        path.replace_extension(".engine");
        if (!std::filesystem::exists(path)) {
            buildEngine(modelPath);
        }
        loadEngine(modelPath);
        
        _context = _engine->createExecutionContext();

        CUDA_WARN(cudaMalloc((void**) &_outputBuffer, 5 * 84000 * sizeof(char)));

    } catch (std::exception& e) {
        Utils::LogFmt("DetectModel::DetectModel - %s", e.what());
    }
}

DetectModel::~DetectModel() {
    CUDA_WARN(cudaFree((void*) _outputBuffer));
}

void DetectModel::buildEngine(std::string modelPath) {

    Utils::LogFmt("Building engine for %s", modelPath);

    nvinfer1::IBuilder* builder = nvinfer1::createInferBuilder(_logger);
    nvinfer1::INetworkDefinition* network = builder->createNetworkV2(1U << (int32_t) nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);

    nvonnxparser::IParser* parser = nvonnxparser::createParser(*network, _logger);
    parser->parseFromFile(modelPath.c_str(), static_cast<int32_t>(nvinfer1::ILogger::Severity::kWARNING));
    for (int32_t i = 0; i < parser->getNbErrors(); ++i) {
        std::cout << parser->getError(i)->desc() << std::endl;
    }

    nvinfer1::IBuilderConfig* config = builder->createBuilderConfig();
    config->setMemoryPoolLimit(nvinfer1::MemoryPoolType::kWORKSPACE, 4e9);
    // config->setMemoryPoolLimit(MemoryPoolType::kTACTIC_SHARED_MEMORY, 48 << 10);
    nvinfer1::IHostMemory* serializedModel = builder->buildSerializedNetwork(*network, *config);

    std::filesystem::path path(modelPath);
    path.replace_extension(".engine");
    Utils::LogFmt("Saving engine to %s", path.string());
    std::fstream file("file.binary", std::ios::out | std::ios::binary);
    file.write((char*) serializedModel->data(), serializedModel->size());
    file.close();

    delete parser;
    delete network;
    delete config;
    delete builder;
    delete serializedModel;
}

void DetectModel::loadEngine(std::string modelPath) {

    Utils::LogFmt("Loading engine from %s", modelPath);

    if (!std::filesystem::exists(std::filesystem::path(modelPath))) {
        throw std::runtime_error("Engine does not exist at path");
    }

    std::ifstream file(modelPath, std::ios::in | std::ios::binary );
    if (!file.is_open()) {
        throw std::runtime_error("Could not open engine file");
    }

    long BUFFER_SIZE = 5000000000;
    char* buffer = new char[BUFFER_SIZE];
    file.read(buffer, BUFFER_SIZE);
    Utils::LogFmt("Read %i bytes of engine", file.gcount());
    if (!file.eof()) {
        file.close();
        delete[] buffer;
        throw std::runtime_error("Buffer size not big enough for engine");
    }
    file.close();

    _runtime = nvinfer1::createInferRuntime(_logger);
    _engine = _runtime->deserializeCudaEngine(buffer, file.gcount());

    delete[] buffer;
    if (_engine == nullptr) {
        throw std::runtime_error("Could not deserialize engine");
    }
}

void DetectModel::runInference(void* inputImage) {

    _context->setTensorAddress("images", inputImage);
    _context->setTensorAddress("output0", (void*) _outputBuffer);

    for (int32_t i = 0, e = _engine->getNbIOTensors(); i < e; i++) {
        auto const name = _engine->getIOTensorName(i);
        std::cout << "Tensor " << i << ": " << name << std::endl;
    }

    // void* bindings[2];
    // _context->executeV2(binding);

}