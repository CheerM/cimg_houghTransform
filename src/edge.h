#include "CImg.h"
#include <iostream>

using namespace std;
using namespace cimg_library;

//高斯平滑去噪
CImg<unsigned char> GLPF(CImg<unsigned char>& inputImage) {
	unsigned int width = inputImage.width();
	unsigned int height = inputImage.height();

	//高斯核
	int glpf[5][5] = {1, 4, 7, 4, 1, 4, 16, 26, 16, 4, 7,26,41,26,7, 4,16,26,16,4,1,4,7,4,1};
	
	if (inputImage.spectrum() != 1) {
		cout << "you should input a gray image\n";
		return inputImage;
	}

	CImg<unsigned char> output(width, height, 1, 1);
	for (int x = 0; x <= width; x ++) {
		for (int y = 0; y <= height; y ++) {
			int sum = 0;
			for (int p = -2; p <= 2; p ++) {
				for (int q = -2; q <= 2; q ++) {
					int temp;
					//超出图像矩阵范围，则用0填充
					if (x + p >= width || x + p < 0 || y + q >= height || y + q < 0) {
						temp = 0;
					}
					else  {
						temp = (int)inputImage.atXYZC(x + p, y + p, 1, 0);
					}
					sum += temp * glpf[p + 2][q + 2];
				}
			}
			//归一化处理
			sum /= 273;
			output.atXYZC(x, y, 1, 0) = (unsigned char)(sum);
		}
	}
	return output;
}

CImg<unsigned char> rgb2Gray(CImg<unsigned char>& inputImage, int threshold) {
	unsigned int width = inputImage.width();
	unsigned int height = inputImage.height();
	
	CImg<unsigned char> output(width, height, 1, 1);
	output.fill(0);
	for (int x = 0; x <= width; x ++) {
		for (int y = 0; y <= height; y ++) {
			int sum = 0;
			for (int z = 0; z < 3; z ++) {
				sum += inputImage.atXYZC(x,y,0, z);
			}
			/*
			if (sum/3 > threshold)sum = 255;
			else sum = 0;
			*/
			sum = sum/3 > threshold ? 255 : 0;
			output.atXYZC(x, y, 1, 0) = static_cast<unsigned char>(sum);
		}
	}
	return output;
}

//求解梯度幅值和方向：sobel算子
CImg<unsigned char> mySobel(CImg<unsigned char>& inputImage) {
	unsigned int width = inputImage.width();
	unsigned int height = inputImage.height();

	//sobel算子
	int sobel_1[3][3] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
	int sobel_2[3][3] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

	if (inputImage.spectrum() != 1) {
		cout << "you should input a gray image\n";
		return inputImage;
	}
	CImg<unsigned char> output(width, height, 1, 1);
	for (int x = 0; x <= width; x ++) {
		for (int y = 0; y <= height; y ++) {
			int sum_1 = 0;
			int sum_2 = 0;
			for (int p = -1; p <= 1; p ++) {
				for (int q = -1; q <= 1; q ++) {
					int temp;
					//超出图像矩阵范围，则用0填充
					if (x + p >= width || x + p < 0 || y + q >= height || y + q < 0) {
						temp = 0;
					}
					else  {
						temp = (int)inputImage.atXYZC(x + p, y + p, 1, 0);
					}
					sum_1 += temp * sobel_1[p + 1][q + 1];
					sum_2 += temp * sobel_2[p + 1][q + 1];
				}
			}
			double tmp = pow(sum_1+0.0, 2) + pow(sum_2+0.0, 2);
			tmp = sqrt(tmp);
			tmp = tmp > 125 ? 255 : 0;
			output.atXYZC(x, y, 1, 0) = (unsigned char)(tmp);

		}
	}
	return output;
}


CImg<unsigned char> edge(CImg<unsigned char>& input, int threshold) {
	CImg<unsigned char> output = rgb2Gray(input, threshold);
	output = GLPF(output);
	output = mySobel(output);
	return output;
};