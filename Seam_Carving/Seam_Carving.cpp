//�ּҼս� �̹��� ������¡
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <limits>

/*int a = (int)src.at<Vec3b>(x, Seam[x] - 1)[2], b = (int)src.at<Vec3b>(x, Seam[x] + 1)[2], c = (int)src.at<Vec3b>(x, Seam[x])[2], d = (a + b + c) / 3;
printf("%d, %d, %d = %d\n", a, b, c, d); */

using namespace std;
using namespace cv;

Mat calc_energy(Mat src) //������ ���
{
	Mat x, y;
	Mat dst = Mat::zeros(src.rows, src.cols, src.type());
	//ĳ�Ͽ��� ����
	Sobel(src, x, CV_64F, 1, 0); //x �̺а�
	Sobel(src, y, CV_64F, 0, 1); //y �̺а�
	magnitude(x, y, dst); //���� ũ�����Լ�
	double min_value, max_value, res;
	minMaxLoc(dst, &min_value, &max_value); //�ּҰ� �ִ밪 ã��
	res = (1 / max_value) * 255;
	dst = dst * res; //����ȭ (��� ���Ҹ� �ִ밪���� ������)
	dst.convertTo(dst, CV_8U); //1�� ä���� uchar ���·� �ٲٱ�
	return dst;
}

vector<uint> find_Seam(Mat src) //���� Seam ã�� (Seam�� x, y��ǥ�� return�ȴ�)
{
	Mat E_map = Mat::zeros(src.rows, src.cols, src.type());
	E_map = calc_energy(src); //���� Energy Map �̿�
	vector<vector<int>> dp(E_map.rows, vector<int>(E_map.cols)); //Dynamic Programing ����ϱ� ���� �޸�
	vector<uint> Seam; //ã�� Seam
	int minimum = INT_MAX;
	int minimum_idx = 0;

	for (int y = 0; y < E_map.cols; y++)
	{
		dp[0][y] = (int)E_map.at<uchar>(0, y);
	}

	//������ ����

	for (int x = 1; x < E_map.rows; x++) //����
	{
		for (int y = 0; y < E_map.cols; y++) //����
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
	//Seam ã�� (������ ���� �ʿ��� �Ųٷ� �ö󰣴�.)

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
	//Seam�� pixel ���� ���� ��İ��� �������
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
		transpose(src, src); //��ġ��� (x,y) => (y,x)
		flip(src, src, 1); //�¿� ��Ī
	}
	else
	{
		flip(src, src, 1); //�¿� ��Ī
		transpose(src, src); //��ġ��� (y,x) => (x,y)
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
	Mat src = imread("C:\\Users\\wook\\Desktop\\Castle.jpg", IMREAD_COLOR); //�̹��� �о����
	if (!src.data)
	{
		cout << "Can't Read Image" << endl;
		return -1;
	}
	char key;
	while (true)
	{
		namedWindow("Display window", WINDOW_AUTOSIZE); //�̹����͵�����ũ��� â ����
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
			rotate(src, '+'); //�� �� �ٲٱ�
			vector<uint>Seam = find_Seam(src);
			src = remove_pixel(src, Seam);
			image_reduction(src);
			Seam.clear();
			rotate(src, '-'); //�� �� �ٲٱ�
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