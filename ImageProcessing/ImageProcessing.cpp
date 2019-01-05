#include "ImageProcessing.h"

/**
*公式：
*     Zc = 32*fx/(u-cx)
*     Xc = (u-cx)*Zc/fx
*     Yc = (v-cy)*Zc/fy = 32*fx*(v-cy)/(fy*(u-cx))
*     算出y坐标之差 就是谷物厚度和刮板厚度之和。
*/


/**分块处理图片
*/
void ImageProcessing(Mat& thisFrame, Mat& mask, float volume_weight, float CSA, float& sum_weight)
{
	float fx = 1761.6;
	float fy = 1774.03;
	float cx = 699.36;
	float cy = 505.98;

	bitwise_and(thisFrame, mask, thisFrame);

	threshold(thisFrame, thisFrame, 160, 255, CV_THRESH_BINARY);


	/**将上面分成三个矩形区域
	* A:(77,964)(229,1022)
	* B:(229,964)(323,1053)
	* C:(323,964)(568,1112)
	*/
	bool inAreaA = false; //A区域是否包含点
	bool inAreaB = false; //B区域是否包含点
	bool inAreaC = false; //C区域是否包含点

	Point peak;  //记录顶点信息
	Point3f real_peak; //谷堆最高点三维坐标（相机坐标系）

	Point bottom;//刮板最低点
	Point3f real_bottom; //刮板最低点三维坐标（相机坐标系）

	bool isVoid = false;//是否是空的刮板；
	/*查看A区域是否有点
	*/
	for (int i = 77; i < 229; i++) //i代表行，也就是V
	{
		if (inAreaA) break;
		for (int j = 964; j < 1022; j++)//j代表列，也就是U
		{
			if (thisFrame.at<uchar>(i, j)>0)
			{
				inAreaA = true;
				cout << "In A area;" << endl;
				peak.x = j;
				peak.y = i;
				cout << j << " , " << i << endl;
				break;
			}
		}
	}

	/*如果不在A区域，查看是否在B区域
	*/
	if (!inAreaA)
	{
		for (int i = 229; i < 323; i++)
		{
			if (inAreaB) break;
			for (int j = 964; j < 1053; j++)
			{
				if (thisFrame.at<uchar>(i, j)>0)
				{
					inAreaB = true;
					cout << "In B area;" << endl;
					peak.x = j;
					peak.y = i;
					cout << j << " , " << i << endl;
					break;
				}
			}
		}
	}

	/*如果不在A区域和B区域，查看是否在C区域
	*/
	if (!inAreaA && !inAreaB)
	{
		for (int i = 323; i < 555; i++)
		{
			if (inAreaC) break;
			for (int j = 964; j < 1112; j++)
			{
				if (thisFrame.at<uchar>(i, j)>0)
				{
					inAreaC = true;
					cout << "In C area;" << endl;
					peak.x = j;
					peak.y = i;
					cout << j << " , " << i << endl;
					break;
				}
			}
		}
	}


	/*如果ABC三个区域都没有点，说明刮板上无谷物
	*/
	if (!inAreaA && !inAreaB && !inAreaC)
	{
		cout << "void scrapper" << endl;
		isVoid = true;
	}
	else
	{

		real_peak.z = 32 * fx / (peak.x - cx);
		real_peak.x = (peak.x - cx)*real_peak.z / fx;
		real_peak.y = 32 * fx*(peak.y - cy) / (fy*(peak.x - cx));
	}

	//如果刮板不为空,找到刮板最低点
	bool findBottom = false;
	if (!isVoid)
	{
		for (int i = 770; i >= 568; i--)
		{
			if (findBottom) break;
			for (int j = 1112; j > 1077; j--)
			{
				if (thisFrame.at<uchar>(i, j)>0)
				{
					findBottom = true;
					cout << "bottom find !!!" << endl;
					bottom.x = j;
					bottom.y = i;
					cout << j << " , " << i << endl;
					break;
				}
			}
		}
	}
	//只有当刮板不为空且扎到刮板最低点才计算谷堆高度
	if (!isVoid && findBottom)
	{
		real_bottom.z = 32 * fx / (bottom.x - cx);
		//real_bottom.x = (bottom.x - cx)*real_bottom.z / fx;
		real_bottom.y = 32 * fx*(bottom.y - cy) / (fy*(bottom.x - cx));
		//注意y轴是向下的，所以需要加一个负号,单位是mm
		float distance = -(real_peak.y - real_bottom.y);
		if (distance > 10) distance -= 10.0;
		else distance = 0.0;
		//此次照片中刮板上的谷物重量
		float this_weight = CSA * distance / 10 * volume_weight;
		sum_weight += this_weight;
	}
}