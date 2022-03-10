#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("C:/Users/wook/source/repos/OpenCV/OpenCV/ó��ó��.jpg", IMREAD_GRAYSCALE);
	Mat src_1 = imread("C:/Users/wook/source/repos/OpenCV/OpenCV/ó��ó��_����.jpg", IMREAD_GRAYSCALE);
	Mat src_2 = imread("C:/Users/wook/source/repos/OpenCV/OpenCV/��.jpg", IMREAD_GRAYSCALE);
	threshold(src, src, 127, 255, THRESH_BINARY_INV);
	threshold(src_1, src_1, 127, 255, THRESH_BINARY_INV);
	threshold(src_2, src_2, 127, 255, THRESH_BINARY_INV);
	resize(src, src, Size(600, 186), 0, 0, INTER_CUBIC);
	resize(src_1, src_1, Size(600, 122), 0, 0, INTER_CUBIC);
	resize(src_2, src_2, Size(200, 308), 0, 0, INTER_CUBIC);
	Mat skel(src.size(), CV_8UC1, Scalar(0));
	Mat skel_1(src_1.size(), CV_8UC1, Scalar(0));
	Mat skel_2(src_2.size(), CV_8UC1, Scalar(0));
	imshow("ó��ó��", src);
	imshow("ó��ó��_����", src_1);
	imshow("��", src_2);
	Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
	Mat temp, dst, temp_1, dst_1, temp_2, dst_2;
	Mat addh, addv, final;

	for (int i = 20; i > 0; i--) //ó��ó��
	{
		erode(src, dst, element); //ħ��
		dilate(dst, temp, element); //��â
		subtract(src, temp, temp); // ���� ����
		bitwise_or(skel, temp, skel); //��Ʈ�� �� �� ���� (skel�� temp�� ����, skel�� ��ȯ)
		dst.copyTo(src); //dst�� src�� ����
	}

	for (int i = 15; i > 0; i--) //ó��ó�� ����
	{
		erode(src_1, dst_1, element); //ħ��
		dilate(dst_1, temp_1, element); //��â
		subtract(src_1, temp_1, temp_1); // ���� ����
		bitwise_or(skel_1, temp_1, skel_1); //��Ʈ�� �� �� ���� (skel�� temp�� ����, skel�� ��ȯ)
		dst_1.copyTo(src_1); //dst�� src�� ����
	}

	for (int i = 15; i > 0; i--) // ��
	{
		erode(src_2, dst_2, element); //ħ��
		dilate(dst_2, temp_2, element); //��â
		subtract(src_2, temp_2, temp_2); // ���� ����
		bitwise_or(skel_2, temp_2, skel_2); //��Ʈ�� �� �� ���� (skel�� temp�� ����, skel�� ��ȯ)
		dst_2.copyTo(src_2); //dst�� src�� ����
	}
	
	vconcat(skel,skel_1, addh);
	hconcat(skel_2,addh, final);

	imshow("����̹���", final);
	waitKey(0);
	return 0;
}