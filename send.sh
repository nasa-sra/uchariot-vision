scp -r ./src uchariot@$1:~/uchariot-vision/

# if there are two arguments then send the include dir
if [ "$#" -ne 1 ]
then
    echo sending /include
    scp -r ./include uchariot@$1:~/uchariot-vision/
fi
# Line below is commented out bc my CMakeLists is different on my mac
#scp ./CMakeLists.txt uchariot@$1:~/uchariot-vision/CMakeLists.txt 
