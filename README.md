1.首先需要添加的功能：
（１）在每次记录实际后的时候，应该使用一个vector,类似于栈的数据结构，vector的大小可以任意，例如取１０，
将每次的


2.如何生成动态链接库
例如生成　libImageProcessing.so
使用文件是ImageProcessing.h 和ImageProcessing.c ，进入到ImageProcessing文件件
g++ ImageProcessing.* -fPIC -shared -o libImageProcessing.so
注意：如果是以cpp为结尾的以g++编译，如果是c语言语法用gcc编译

例如生成 libserialSend.so
使用文件是serialSend.h 和serialSend.cpp
gcc serialSend.* -fPIC -shared -o libserialSend.so


注意在makefile中添加的部分：
用到C++11特性

CPPFLAGS = ...
            -std=c++11 \  
            -I./serialSend　-I./ImageProcessing



LIBFLAGS = ... \
 -L/usr/local/lib\
-lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui \
-lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_objdetect -lopencv_shape \
-lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab \

-L./ImageProcessing \
-lImageProcessing \

-L ./serialSend \
-lserialSend

分别是使用到的OpenCV