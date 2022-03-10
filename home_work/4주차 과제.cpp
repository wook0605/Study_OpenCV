#include <opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

Mat img, img2;
int red, green, blue;
double Rgamma, Bgamma, Ggamma = 1.0;

void on_trackbar(int, void*)
{

	Rgamma = ((double)red / 100);
	Bgamma = ((double)blue / 100);
	Ggamma = ((double)green / 100);

	for (int r = 0; r < img.rows; r++)
	{
		for (int c = 0; c < img.cols; c++)
		{
			img2.at<cv::Vec3b>(r, c)[2] = saturate_cast<uchar>(pow((double)img.at<Vec3b>(r, c)[2] / 255.0, Rgamma + 0.1) * 255.0); //Blue
			img2.at<cv::Vec3b>(r, c)[1] = saturate_cast<uchar>(pow((double)img.at<Vec3b>(r, c)[1] / 255.0, Ggamma + 0.1) * 255.0); //Green
			img2.at<cv::Vec3b>(r, c)[0] = saturate_cast<uchar>(pow((double)img.at<Vec3b>(r, c)[0] / 255.0, Bgamma + 0.1) * 255.0); //Red
		}
	}
	imshow("img", img2);
}

int main()
{
	img = imread("lenna.jpg");
	img2 = img.clone();
	if (img.empty()) {
		cout << "영상을 읽을 수 없음" << endl;
		return 0;
	}
	imshow("img", img);
	createTrackbar("R", "img", &red, 180, on_trackbar, &img2);
	createTrackbar("G", "img", &green, 180, on_trackbar, &img2);
	createTrackbar("B", "img", &blue, 180, on_trackbar, &img2);

	waitKey(0);
	return 0;
}