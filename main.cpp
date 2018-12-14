/*　１．大恒相机硬出发采集图像
*　 ２．通过OpenCV处理图像
*  ３．动态计算升运器的速度
*  ４.串口发送流量传感器数据
*/


#include"GxIAPI.h"
#include <iostream>
#include<fstream>
#include <string>
#include <stdio.h>
#include <sys/time.h>


//使用串口发送产量数据
#include "serialSend/serialSend.h"


//使用OpenCV进行图像处理
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;


Mat thisFrame(1024,1280,CV_8UC1);   //OpenCV Mat

//掩模
Mat mask;

//保存的照片张数
int g_count;

//用来记录时间戳
struct timeval tstart,tend;


//用来计算使用的时间，单位是微秒，usec
long int use_microconds;


//串口发送的消息
unsigned char send_buf[3]={0,0,255};


//谷物厚度
float grain_thickness=0.0,real_length=0.0;

//累积的谷物体积
float volumeAcc=0.0;

//刮板的截面面积,单位是　mm2;
float CSA=14000.0;

// 谷物的容重，单位是　g/L
float volume_weight=502;


//谷物的产量，单位是kg
double yield=0.0;

//Image processing callback function.
static void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame)
{
      gettimeofday(&tend,NULL);

  	  use_microconds=1000000*(tend.tv_sec - tstart.tv_sec) + (tend.tv_usec - tstart.tv_usec);

  	  cout <<"used microseconds is "<<use_microconds<<endl;

  	  tstart.tv_sec=tend.tv_sec ;

  	  tstart.tv_usec=tend.tv_usec ;

      if (pFrame->status == 0)
      {
        //Performs some image processing operations

      	cout<<g_count<<" Frame captured"<<endl;
        //将相机拍摄的照片从内存中拷贝到Mat的data指针，这样才可以调用OpenCV来处理
      	memcpy(thisFrame.data,pFrame->pImgBuf,pFrame->nImgSize);

      	/*put OpenCV image processing code here,consider using multi-thread next time*/


      	//存储原始的照片
      	if(g_count<1000)
        {
         imwrite("before_test_"+to_string(g_count)+".jpg",thisFrame);
         g_count++;
        }

/*
      	bitwise_and(thisFrame, mask, thisFrame);
      	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
      	dilate(thisFrame, thisFrame, element);
      	vector<vector<Point>> contours;
      	vector<Vec4i> hierarchy;
      	findContours(thisFrame, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());

      	float maxLength = 0.0;



        if (contours.size() > 0)
      	{
      		for (int j = 0; j < contours.size(); j++)
      		{
      			RotatedRect rect = minAreaRect(contours[j]);
      			if (rect.size.width > maxLength)
      				maxLength = rect.size.width;
      		}

      		for (int k = 0; k < contours.size(); k++)
      		{
      			RotatedRect rect = minAreaRect(contours[k]);
      			if (rect.size.height > maxLength)
      			    maxLength = rect.size.height;
      		}
      	}

      	real_length = maxLength/17.0;


    		cout << "height is " << maxLength << endl;
    		cout << "real length is " << maxLength /17<<endl;


      	if (real_length > 10.0)
      	{
      		grain_thickness=real_length-10.0;
      		volumeAcc+=grain_thickness*CSA;
      		yield=volumeAcc*volume_weight;

      		//Need To Transform g to Kg and mm3 to L,thats why divided by 1,000,000,000,
      		send_buf[0]=((int)(yield/1000000000))%256;
      		send_buf[1]=((int)(yield/1000000000))/256;

          ofstream fout("result.txt");

      		fout<<"Yiled is "<<(int)(yield/1000000000)<<" Kg"<<endl;
      	}
        */
    }

      return;

}


int main(int argc, char* argv[])
{
   /*串口相关的设置*/
   int fd;                            //文件描述符
   int err;                           //返回调用函数的状态
   int len;

   struct termios options;

   //初始化开始时间戳
   tstart.tv_sec = 0;
   tstart.tv_usec = 0;

   /* 打开串口，返回文件描述符，在此之前必须先给串口写权限，否则会报错  */
   fd = open( "/dev/ttyUSB0", O_RDWR|O_NOCTTY|O_NDELAY);

   g_count=0;

   //mask = imread("MASK.jpg",0);

   //threshold(mask, mask, 80, 255, CV_THRESH_BINARY);

   GX_STATUS status = GX_STATUS_SUCCESS;

   GX_DEV_HANDLE hDevice = NULL;

   GX_OPEN_PARAM stOpenParam;

   uint32_t nDeviceNum = 0;

   // 初始化库
   status = GXInitLib();
   if (status!= GX_STATUS_SUCCESS)
   {
       cout<<"Unable to Init Camera"<<endl;
       return 0;
   }

   // 更新设备列表
   status = GXUpdateDeviceList(&nDeviceNum, 1000);
   if ((status != GX_STATUS_SUCCESS) || (nDeviceNum<= 0))
   {
       cout<<"Unable to Update Device List"<<endl;
       return 0;
   }

   // 独占方式打开设备
   //stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;

   //只读方式打开设备
   //stOpenParam.accessMode = GX_ACCESS_READONLY;

   //控制方式打开设备
   stOpenParam.accessMode = GX_ACCESS_CONTROL;

   //通过设备序号打开设备
   stOpenParam.openMode = GX_OPEN_INDEX;

   //从１开始
   stOpenParam.pszContent = "1";

    //打开设备
   status = GXOpenDevice(&stOpenParam, &hDevice);

    if (status == GX_STATUS_SUCCESS)
    {
	//触发模式置为On
         status =GXSetEnum(hDevice,GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);


        //选择触发源为Line0
        status =GXSetEnum(hDevice,GX_ENUM_TRIGGER_SOURCE, GX_TRIGGER_SOURCE_LINE0);


	      //设置引脚方向为输入
        status = GXSetEnum(hDevice, GX_ENUM_LINE_MODE, GX_ENUM_LINE_MODE_INPUT);

	       //选择Line0作为触发
	       //status = GXSetEnum(hDevice, GX_ENUM_LINE_SELECTOR, GX_ENUM_LINE_SELECTOR_LINE0);

	       //设置曝光模式
	       status = GXSetEnum(hDevice, GX_ENUM_EXPOSURE_MODE,GX_EXPOSURE_MODE_TIMED);


         //设置曝光时间
	       status = GXSetFloat(hDevice, GX_FLOAT_EXPOSURE_TIME, 3000);

         //设置触发极性为下降沿触发
         status=GXSetEnum(hDevice,GX_ENUM_TRIGGER_ACTIVATION,GX_TRIGGER_ACTIVATION_FALLINGEDGE);
	       //status=GXSetEnum(hDevice,GX_ENUM_TRIGGER_ACTIVATION,GX_TRIGGER_ACTIVATION_RISINGEDGE);

         //注册图像处理回调函数
         status = GXRegisterCaptureCallback(hDevice, NULL, OnFrameCallbackFun);

         //发送图像获取命令
         status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START);

       //---------------------
       //
       //If a valid trigger is generated before sending the stop call order,
       //the image will be returned to the user via the OnFrameCallbackFun interface.
       //---------------------
    	while(true)
    	{
    	   //Here.volume sensor message is sent using serial port
    	   for(int k=0;k<1000000;k++)
    	   {
    		     k++;
    	   }
    	//   cout<<"Yiled is"<<yield<<"Kg"<<endl;
    	//   serial_send(fd,&options,send_buf,3);
    	}

       //发送图像停止采集命令
       status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP);

       //Unregisters image processing callback function.
       status = GXUnregisterCaptureCallback(hDevice);
    }
    status = GXCloseDevice(hDevice);
    status = GXCloseLib();
    return 0;
}
