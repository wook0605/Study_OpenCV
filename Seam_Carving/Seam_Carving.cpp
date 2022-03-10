//최소손실 이미지 리사이징
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <limits>

/*int a = (int)src.at<Vec3b>(x, Seam[x] - 1)[2], b = (int)src.at<Vec3b>(x, Seam[x] + 1)[2], c = (int)src.at<Vec3b>(x, Seam[x])[2], d = (a + b + c) / 3;
printf("%d, %d, %d = %d\n", a, b, c, d); */

using namespace std;
using namespace cv;

Mat calc_energy(Mat src) //에너지 계산
{
	Mat x, y;
	Mat dst = Mat::zeros(src.rows, src.cols, src.type());
	//캐니에지 검출
	Sobel(src, x, CV_64F, 1, 0); //x 미분값
	Sobel(src, y, CV_64F, 0, 1); //y 미분값
	magnitude(x, y, dst); //벡터 크기계산함수
	double min_value, max_value, res;
	minMaxLoc(dst, &min_value, &max_value); //최소값 최대값 찾기
	res = (1 / max_value) * 255;
	dst = dst * res; //정규화 (모든 원소를 최대값으로 나누기)
	dst.convertTo(dst, CV_8U); //1개 채널의 uchar 형태로 바꾸기
	return dst;
}

vector<uint> find_Seam(Mat src) //세로 Seam 찾기 (Seam의 x, y좌표가 return된다)
{
	Mat E_map = Mat::zeros(src.rows, src.cols, src.type());
	E_map = calc_energy(src); //계산된 Energy Map 이용
	vector<vector<int>> dp(E_map.rows, vector<int>(E_map.cols)); //Dynamic Programing 사용하기 위한 메모리
	vector<uint> Seam; //찾은 Seam
	int minimum = INT_MAX;
	int minimum_idx = 0;

	for (int y = 0; y < E_map.cols; y++)
	{
		dp[0][y] = (int)E_map.at<uchar>(0, y);
	}

	//에너지 축적

	for (int x = 1; x < E_map.rows; x++) //가로
	{
		for (int y = 0; y < E_map.cols; y++) //세로
		{
			if (y == 0)
			{
				dp[x][y] = min(dp[x - 1][y], dp[x - 1][y + 1]);
			}
			else if (y == E_map.cols - 1)
			{
				dp[x][y] = min(dp[x - 1][y - 1], dp[x - 1][y]);
			}
			else
			{
				dp[x][y] = min(min(dp[x - 1][y - 1], dp[x - 1][y]), dp[x - 1][y + 1]);
			}
			dp[x][y] += (int)E_map.at<uchar>(x, y);
		}
	}
	//Seam 찾기 (에너지 누적 맵에서 거꾸로 올라간다.)

	for (int y = 0; y < E_map.cols; y++)
	{
		if (dp[E_map.rows - 1][y] < minimum)
		{
			minimum = dp[E_map.rows - 1][y];
			minimum_idx = y;
		}
	}
	Point pos(E_map.rows - 1, minimum_idx);
	Seam.push_back(pos.y);

	while (pos.x != 0)
	{
		int value = dp[pos.x][pos.y] - (int)E_map.at<uchar>(pos.x, pos.y);
		if (pos.y == 0)
		{
			if (value == dp[pos.x - 1][pos.y + 1]) pos = Point(pos.x - 1, pos.y + 1);
			else pos = Point(pos.x - 1, pos.y);
		}

		else if (pos.y == E_map.cols - 1)
		{
			if (value == dp[pos.x - 1][pos.y - 1]) pos = Point(pos.x - 1, pos.y - 1);
			else pos = Point(pos.x - 1, pos.y);
		}

		else
		{
			if (value == dp[pos.x - 1][pos.y - 1]) pos = Point(pos.x - 1, pos.y - 1);
			else if (value == dp[pos.x - 1][pos.y + 1]) pos = Point(pos.x - 1, pos.y + 1);
			else pos = Point(pos.x - 1, pos.y);
		}
		Seam.push_back(pos.y);
	}

	return Seam;
}



Mat remove_pixel (Mat src, vector<uint> &Seam)
{
	Mat dst = src.clone();
	for (int x = 0; x < dst.rows; x++)
	{
		for (int y = Seam[x]; y < dst.cols - 1; y++)
		{
			dst.at<Vec3b>(x, y)[2] = dst.at<Vec3b>(x, y + 1)[2];
			dst.at<Vec3b>(x, y)[1] = dst.at<Vec3b>(x, y + 1)[1];
			dst.at<Vec3b>(x, y)[0] = dst.at<Vec3b>(x, y + 1)[0];
		}
	}
	return dst;
}
Mat insert_pixel(Mat src, vector<uint>& Seam)
{
	Mat dst = src.clone();
	//Seam의 pixel 값은 인접 행렬과의 평균으로
	for (int x = 0; x < dst.rows; x++)
	{
		for (int y = dst.cols - 2; y > -1; y--)
		{
			if (y > Seam[x])
			{
				dst.at<Vec3b>(x, y)[2] = src.at<Vec3b>(x, y - 1)[2];
				dst.at<Vec3b>(x, y)[1] = src.at<Vec3b>(x, y - 1)[1];
				dst.at<Vec3b>(x, y)[0] = src.at<Vec3b>(x, y - 1)[0];
			}
			else if (y == Seam[x])
			{
				if (Seam[x] == 0)
				{
					dst.at<Vec3b>(x, y)[1] = ((int)src.at<Vec3b>(x, 0)[2] + (int)src.at<Vec3b>(x, 1)[2]) / 2;
					dst.at<Vec3b>(x, y)[0] = ((int)src.at<Vec3b>(x, 0)[1] + (int)src.at<Vec3b>(x, 1)[1]) / 2;
					dst.at<Vec3b>(x, y)[2] = ((int)src.at<Vec3b>(x, 0)[0] + (int)src.at<Vec3b>(x, 1)[0]) / 2;
				}
				else
				{
					dst.at<Vec3b>(x, y)[2] = ((int)src.at<Vec3b>(x, Seam[x] - 1)[2] + (int)src.at<Vec3b>(x, Seam[x] + 1)[2] + (int)src.at<Vec3b>(x, Seam[x])[2]) / 3;
					dst.at<Vec3b>(x, y)[1] = ((int)src.at<Vec3b>(x, Seam[x] - 1)[1] + (int)src.at<Vec3b>(x, Seam[x] + 1)[1] + (int)src.at<Vec3b>(x, Seam[x])[1]) / 3;
					dst.at<Vec3b>(x, y)[0] = ((int)src.at<Vec3b>(x, Seam[x] - 1)[0] + (int)src.at<Vec3b>(x, Seam[x] + 1)[0] + (int)src.at<Vec3b>(x, Seam[x])[0]) / 3;
				}
			}
			else
			{
				dst.at<Vec3b>(x, y)[2] = src.at<Vec3b>(x, y)[2];
				dst.at<Vec3b>(x, y)[1] = src.at<Vec3b>(x, y)[1];
				dst.at<Vec3b>(x, y)[0] = src.at<Vec3b>(x, y)[0];
			}
		}
	}

	return dst;
}

void rotate(Mat& src, char flag)
{
	if (flag == '+')
	{
		transpose(src, src); //전치행렬 (x,y) => (y,x)
		flip(src, src, 1); //좌우 대칭
	}
	else
	{
		flip(src, src, 1); //좌우 대칭
		transpose(src, src); //전치행렬 (y,x) => (x,y)
	}
}

void image_reduction(Mat& src)
{
	rotate(src, '+');
	src.pop_back();
	rotate(src, '-');
}

void image_insertion(Mat& src)
{
	rotate(src, '+');
	Mat row = Mat::ones(1, src.cols, src.type());
	for (int y = 0; y < src.cols; y++)
	{
		row.at<Vec3b>(0, y)[2] = src.at<Vec3b>(src.rows - 1, y)[2];
		row.at<Vec3b>(0, y)[1] = src.at<Vec3b>(src.rows - 1, y)[1];
		row.at<Vec3b>(0, y)[0] = src.at<Vec3b>(src.rows - 1, y)[0];
	}
	src.push_back(row);
	rotate(src, '-');
}

int main()
{
	Mat src = imread("C:\\Users\\wook\\Desktop\\Castle.jpg", IMREAD_COLOR); //이미지 읽어오기
	if (!src.data)
	{
		cout << "Can't Read Image" << endl;
		return -1;
	}
	char key;
	while (true)
	{
		namedWindow("Display window", WINDOW_AUTOSIZE); //이미지와동일한크기로 창 생성
		imshow("Display window", src);
		key = waitKey(0);
		if (key == 'q') break;
		else if (key == 'w') //Virtical Seam Remove
		{
			vector<uint>Seam = find_Seam(src);
			src = remove_pixel(src, Seam);
			image_reduction(src);
			Seam.clear();
		}
		else if (key == 'e') //Virtical Seam Insert
		{
			vector<uint>Seam = find_Seam(src);
			image_insertion(src);
			src = insert_pixel(src, Seam);
			Seam.clear();
		}
		else if (key == 's') //Horizon Seam Remove
		{
			rotate(src, '+'); //행 열 바꾸기
			vector<uint>Seam = find_Seam(src);
			src = remove_pixel(src, Seam);
			image_reduction(src);
			Seam.clear();
			rotate(src, '-'); //행 열 바꾸기
		}

		else if (key == 'd') //Horizon Seam Insert
		{
			rotate(src, '+');
			vector<uint>Seam = find_Seam(src);
			image_insertion(src);
			src = insert_pixel(src, Seam);
			Seam.clear();
			rotate(src, '-');
		}
	}
}