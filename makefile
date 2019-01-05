# Makefile for sample program
.PHONY			: all clean

# the program to build
NAME			:= main

# Build tools and flags
CXX			:= /usr/bin/g++
LD			:= /usr/bin/g++

CPPFLAGS		:= -w -std=c++11 -I$(GENICAM_ROOT_V2_3)/library/CPP/include \
			       -I$(GENICAM_ROOT_V2_3)/../../sdk/include \
				   -I./serialSend　-I./ImageProcessing
ifeq ($(shell getconf LONG_BIT),64)
LDFLAGS			:=-lgxiapi -ldximageproc -lpthread \
			      -L$(GENICAM_ROOT_V2_3)/bin/Linux64_x64 \
				  -L$(GENICAM_ROOT_V2_3)/bin/Linux64_x64/GenApi/Generic \
                  -lGCBase_gcc40_v2_3 -lGenApi_gcc40_v2_3 -llog4cpp_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3 \
				  
				  -L/usr/local/lib\
                  -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui \
				  -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_objdetect -lopencv_shape \
				  -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab \
				  -L./ImageProcessing \
				  -lImageProcessing \
				  -L ./serialSend \
				  -lserialSend
else
LDFLAGS                 :=-lgxiapi -ldximageproc -lpthread \
                              -L$(GENICAM_ROOT_V2_3)/bin/Linux32_i86 \
                                  -L$(GENICAM_ROOT_V2_3)/bin/Linux32_i86/GenApi/Generic \
                  -lGCBase_gcc40_v2_3 -lGenApi_gcc40_v2_3 -llog4cpp_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3
endif

all			: $(NAME)

$(NAME)			: $(NAME).o
	$(LD) -o $@ $^ $(CPPFLAGS) $(LDFLAGS)

$(NAME).o		: $(NAME).cpp 
	$(CXX) $(CPPFLAGS) -c -o $@ $<

clean			:
	$(RM) $(NAME).o $(NAME)

