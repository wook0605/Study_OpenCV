#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>

using namespace std;
using namespace cv;


Mat rotate(Mat src, double angle) //ȸ�� �Լ�
{
	Mat dst;
	Point2f pt(src.cols / 2.0, src.rows / 2.0); //ȸ�� �߽��� ������ �߾�������
	Mat r = getRotationMatrix2D(pt, angle, 1.0); //ȸ�� ��� ���
	warpAffine(src, dst, r, Size(src.cols, src.rows)); //������ ��ȯ ����
	return dst;
}

Mat Image_moving(Mat src, int tx, int ty) //�̹��� ���� �̵�
{
	Mat dst = Mat();
	Mat tmat = (Mat_<double>(2, 3) << 1, 0, tx, 0, 1, ty);; 
	//
	// 	   {1, 0, tx}
	// 	   {0, 1, ty} ��� ����
	warpAffine(src, dst, tmat, src.size()); // tx, ty��ŭ �����̵��ϴ� ���
	return dst;
}

int main()
{
	Mat img;
	Mat out_img_1;
	Mat out_img_2;
	Mat resize_img;
	string in_dir = "C:\\Users\\wook\\Desktop\\new_FishPhoto\\"; //�׽�Ʈ �Է� ���͸�
	double angle = 0.0;
	int j = 0;
	int x = 300; //��
	int y = 300; //��
	vector<String> str; //���� �̸� ������ ����
	vector<string> _dir;
	char buf[256];
	int count[4] = { 0, };
	bool as = false;

	glob(in_dir, str, TRUE); //glob(ã�� ���� ���, ã�� ���� ���, recuisive(T or F)
		//T : ���� �� ���� ���� �� ���� ������ ã��
		//F : ���� �� ������ ã��
	cout << "�ε� ���� : " << str.size() << endl;

	if (str.size() == 0) //���Ϳ� �ƹ��� �����Ͱ� �ȵ��� ��
	{
		cout << "�̹����� �������� �ʽ��ϴ�.\n";
	}

	for (int i = 0; i < str.size(); i++) //�̹��� ó��
	{
		img = imread(str[i], IMREAD_COLOR);
		resize(img, img, Size(x, y), 0, 0, CV_INTER_LINEAR); //�̹��� ��������
		ofstream in("C:\\Users\\wook\\Desktop\\ó���� ������_�¿����.txt");
		ofstream delete_("C:\\Users\\wook\\Desktop\\�����ؾ��ϴ�_������.txt");
		char temp[100] = { NULL, };
		strcpy(temp, str[i].c_str());
		char* tok_dir = strtok(temp, "\\");
		while (tok_dir != NULL)
		{
			tok_dir = strtok(NULL, "\\");
			if (strcmp(tok_dir, "new_FishPhoto") == 0)
			{
				tok_dir = strtok(NULL, "\\");
				break;
			}
		}
		if (!img.empty())
		{
			//�̹��� ���� ó���κ�
			IplImage* img2 = new IplImage(img);
			sprintf_s(buf, sizeof(buf), "C:\\Users\\wook\\Desktop\\new_FishPhoto_mirror\\%s\\%d.jpg", tok_dir, i);
			cvFlip(img2, NULL, 1); //1 = �¿���� / -1 = ���Ϲ���
			img = cvarrToMat(img2, true);
			imwrite(buf, img); //�̹��� ����
			delete img2;
			cout << "�����̸� : " << str[i] << endl;
			in << str[i] << "\n";
			//�̹��� �̵� �κ�
			for (int tx = 10; tx < 110; tx += 10) //���������� �б�
			{
				sprintf_s(buf, sizeof(buf), "C:\\Users\\wook\\Desktop\\Fish_right\\%s\\%d.jpg", tok_dir,count[0]);
				out_img_1 = Image_moving(img, tx, 0);
				imwrite(buf, out_img_1);
				count[0]++;
				cout << str[i] << "�̹�����" << tx << "��ŭ ���������� �̵����׽��ϴ�.\n";
			}
			for (int ty = 10; ty < 110; ty += 10) //�Ʒ������� �б�
			{
				sprintf_s(buf, sizeof(buf), "C:\\Users\\wook\\Desktop\\Fish_down\\%s\\%d.jpg", tok_dir, count[1]);
				out_img_1 = Image_moving(img, 0, ty);
				imwrite(buf, out_img_1);
				count[1]++;
				cout << str[i] << "�̹�����" << ty << "��ŭ �Ʒ������� �̵����׽��ϴ�.\n";
			}
			for (int ty = -10; ty > -110; ty -= 10) //�������� �б�
			{
				sprintf_s(buf, sizeof(buf), "C:\\Users\\wook\\Desktop\\Fish_up\\%s\\%d.jpg", tok_dir, count[2]);
				out_img_1 = Image_moving(img, 0, ty);
				imwrite(buf, out_img_1);
				count[2]++;
				cout << str[i] << "�̹�����" << ty << "��ŭ �������� �̵����׽��ϴ�.\n";
			}
			//�̹��� ȸ�� �κ�
			for (angle = 0.0; angle < 360; angle += 5) //�̹��� 5���� �ð���� ȸ��
			{
				sprintf_s(buf, sizeof(buf), "C:\\Users\\wook\\Desktop\\new_FishPhoto\\%s\\%d.jpg",tok_dir, j); //������������ ���� ����
				out_img_2 = rotate(resize_img, angle); //�̹��� ȸ��
				imwrite(buf, out_img_2); //�̹��� ����
				j++;
			}
		}
		else
		{
			delete_ << str[i] << "\n";
		}
	}

}