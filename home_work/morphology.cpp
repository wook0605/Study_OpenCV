#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("C:/Users/wook/source/repos/OpenCV/OpenCV/처음처럼.jpg", IMREAD_GRAYSCALE);
	Mat src_1 = imread("C:/Users/wook/source/repos/OpenCV/OpenCV/처음처럼_문구.jpg", IMREAD_GRAYSCALE);
	Mat src_2 = imread("C:/Users/wook/source/repos/OpenCV/OpenCV/새.jpg", IMREAD_GRAYSCALE);
	threshold(src, src, 127, 255, THRESH_BINARY_INV);
	threshold(src_1, src_1, 127, 255, THRESH_BINARY_INV);
	threshold(src_2, src_2, 127, 255, THRESH_BINARY_INV);
	resize(src, src, Size(600, 186), 0, 0, INTER_CUBIC);
	resize(src_1, src_1, Size(600, 122), 0, 0, INTER_CUBIC);
	resize(src_2, src_2, Size(200, 308), 0, 0, INTER_CUBIC);
	Mat skel(src.size(), CV_8UC1, Scalar(0));
	Mat skel_1(src_1.size(), CV_8UC1, Scalar(0));
	Mat skel_2(src_2.size(), CV_8UC1, Scalar(0));
	imshow("처음처럼", src);
	imshow("처음처럼_문구", src_1);
	imshow("새", src_2);
	Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
	Mat temp, dst, temp_1, dst_1, temp_2, dst_2;
	Mat addh, addv, final;

	for (int i = 20; i > 0; i--) //처음처럼
	{
		erode(src, dst, element); //침식
		dilate(dst, temp, element); //팽창
		subtract(src, temp, temp); // 빼기 연산
		bitwise_or(skel, temp, skel); //비트별 논리 합 연산 (skel에 temp를 더해, skel을 반환)
		dst.copyTo(src); //dst를 src로 복사
	}

	for (int i = 15; i > 0; i--) //처음처럼 문구
	{
		erode(src_1, dst_1, element); //침식
		dilate(dst_1, temp_1, element); //팽창
		subtract(src_1, temp_1, temp_1); // 빼기 연산
		bitwise_or(skel_1, temp_1, skel_1); //비트별 논리 합 연산 (skel에 temp를 더해, skel을 반환)
		dst_1.copyTo(src_1); //dst를 src로 복사
	}

	for (int i = 15; i > 0; i--) // 새
	{
		erode(src_2, dst_2, element); //침식
		dilate(dst_2, temp_2, element); //팽창
		subtract(src_2, temp_2, temp_2); // 빼기 연산
		bitwise_or(skel_2, temp_2, skel_2); //비트별 논리 합 연산 (skel에 temp를 더해, skel을 반환)
		dst_2.copyTo(src_2); //dst를 src로 복사
	}
	
	vconcat(skel,skel_1, addh);
	hconcat(skel_2,addh, final);

	imshow("결과이미지", final);
	waitKey(0);
	return 0;
}