#include "ImageProcessing.h"

/**
*��ʽ��
*     Zc = 32*fx/(u-cx)
*     Xc = (u-cx)*Zc/fx
*     Yc = (v-cy)*Zc/fy = 32*fx*(v-cy)/(fy*(u-cx))
*     ���y����֮�� ���ǹ����Ⱥ͹ΰ���֮�͡�
*/


/**�ֿ鴦��ͼƬ
*/
void ImageProcessing(Mat& thisFrame, Mat& mask, float volume_weight, float CSA, float& sum_weight)
{
	float fx = 1761.6;
	float fy = 1774.03;
	float cx = 699.36;
	float cy = 505.98;

	bitwise_and(thisFrame, mask, thisFrame);

	threshold(thisFrame, thisFrame, 160, 255, CV_THRESH_BINARY);


	/**������ֳ�������������
	* A:(77,964)(229,1022)
	* B:(229,964)(323,1053)
	* C:(323,964)(568,1112)
	*/
	bool inAreaA = false; //A�����Ƿ������
	bool inAreaB = false; //B�����Ƿ������
	bool inAreaC = false; //C�����Ƿ������

	Point peak;  //��¼������Ϣ
	Point3f real_peak; //�ȶ���ߵ���ά���꣨�������ϵ��

	Point bottom;//�ΰ���͵�
	Point3f real_bottom; //�ΰ���͵���ά���꣨�������ϵ��

	bool isVoid = false;//�Ƿ��ǿյĹΰ壻
	/*�鿴A�����Ƿ��е�
	*/
	for (int i = 77; i < 229; i++) //i�����У�Ҳ����V
	{
		if (inAreaA) break;
		for (int j = 964; j < 1022; j++)//j�����У�Ҳ����U
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

	/*�������A���򣬲鿴�Ƿ���B����
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

	/*�������A�����B���򣬲鿴�Ƿ���C����
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


	/*���ABC��������û�е㣬˵���ΰ����޹���
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

	//����ΰ岻Ϊ��,�ҵ��ΰ���͵�
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
	//ֻ�е��ΰ岻Ϊ���������ΰ���͵�ż���ȶѸ߶�
	if (!isVoid && findBottom)
	{
		real_bottom.z = 32 * fx / (bottom.x - cx);
		//real_bottom.x = (bottom.x - cx)*real_bottom.z / fx;
		real_bottom.y = 32 * fx*(bottom.y - cy) / (fy*(bottom.x - cx));
		//ע��y�������µģ�������Ҫ��һ������,��λ��mm
		float distance = -(real_peak.y - real_bottom.y);
		if (distance > 10) distance -= 10.0;
		else distance = 0.0;
		float this_weight = CSA * distance / 10 * volume_weight;
		sum_weight += this_weight;
		cout << "weight is " << (int)(sum_weight / 1000) << "  kg" << endl;
	}
}