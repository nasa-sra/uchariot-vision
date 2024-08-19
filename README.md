# uchariot-vision

Vision software for MicroChariot.

## Libraries

| Name | Desc | Install |
| --- | --- | --- |
| libeigen3 | linear algebra | sudo apt install libeigen3-dev | 
| OpenCV | computer vision | custom built |
| librealsense | realsense camera API | custom built | 
| Jetson Inference | DNN model execution | custom built  | 
| CUDA Framework | GPU Acceleration | included with jetpack | 

## Deploy

Currently, to deploy to the robot you must copy it to the Jetson and compile it there in order to compile with CUDA.  
Just run `./send.sh <IP_OF_JETSON>`  
Then on the jetson in ~/uchariot-vision/build run
```
cmake ..
make -j4
./uChariotVision
```
You can add the simulation option using `cmake -DSIMULATION=true ..` to run the application with a test video instead of a live camera feed.  
If you are on VNC and wish to view the GUI, add the display flag with `./uChariotVision -d`. You can toggle to depth view by pressing d on the GUI.  

## Application

The vision application connects to a realsense camera, processes it to find various detections, and then pushes them to a UNIX message queue in JSON for consumption by the main robot application. Currently, it reports the location of the closest object in the depth map, and detections from an object detection model. 

## Object Detection Models

There are two options for models, both of which are based on the MobileNet-SSD-v2 architecture. The default model used is trained on the COCO dataset and can be used to detect a variety of common objects, including people. The other model has been trained on the rocks from the JSC rockyard with this [dataset](https://universe.roboflow.com/nasarockyard/nasa-rockyard/) and is intended for obstacle avoidance. They are run with the [Jetson Inference library](https://github.com/dusty-nv/jetson-inference/tree/master) which uses TensorRT to run the model with GPU acceleration. To train new models, consult the Jetson Inference [guide](https://github.com/dusty-nv/jetson-inference/blob/master/docs/pytorch-ssd.md). Be advised that when a model is run for the first time, the application must convert the ONNX file to an engine file, which can take 5-10 minutes.

It may also be easier to train YOLO models, as given [here](https://wiki.seeedstudio.com/YOLOv8-TRT-Jetson/). There is a tensorRT-yolo branch to run YOLO models directly with tensorRT in progress. 
