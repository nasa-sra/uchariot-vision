scp -r ./src uchariot@$1:~/uchariot-vision/
scp -r ./include uchariot@$1:~/uchariot-vision/
# Line below is commented out bc my CMakeLists is different on my mac
#scp ./CMakeLists.txt uchariot@$1:~/uchariot-vision/CMakeLists.txt 
