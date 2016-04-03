#include "edge.h"
#include <vector>

#define PI 3.1415926

CImg<unsigned char> HoughTrans(CImg<unsigned char>& inputImage, CImg<unsigned char>& scrImage, double value) {
	unsigned int width = inputImage.width();
	unsigned int height = inputImage.height();

	if (inputImage.spectrum() != 1) {
		cout << "you should input a gray image\n";
		return inputImage;
	}
	int centerX = width / 2;
	int centerY = height / 2;

	//计算霍夫空间的横轴size, r的最大值为sqrt(centerX^2 + centerY^2)
	int max_length = centerX * centerX + centerY * centerY;
	max_length = (int)sqrt(max_length + 0.0);
	cout << width << " " << height << endl;
	cout << "max_length = " << max_length << endl;

	//计算霍夫空间的纵轴size, 0~PI, 分成500个刻度
	const int hough_space = 500;
	double hough_intervals = PI / (double)hough_space;

	//霍夫空间,图像初始化
	CImg<unsigned char> output(2 * max_length, hough_space, 1, 1);
	int** hough = new int*[500];
	for (int k = 0; k < hough_space; k ++)
		hough[k] = new int[2*max_length] ();
	output.fill(0);

	//检测每一个点的所有可能直线方程，并记录投票，以及最大值
	int max_hough = 0;
	for (int x = 0; x < width; x ++) {
		for (int y = 0; y < height; y ++) {
			int temp = (int)inputImage.atXYZC(x, y, 1, 0);
			if (temp == 0)continue;
			else {
				for (int degree = 0; degree < hough_space; degree ++) {
					double r = (x - centerX) * cos(degree * hough_intervals) + (y - centerY) * sin(degree * hough_intervals);
					r += max_length;
					if (r < 0 || (r >= 2 * max_length))continue;
					unsigned char temp = output.atXYZC((unsigned int)r, degree, 1, 0) + 1;
					output.atXYZC((unsigned int)r, degree, 1, 0) = temp;
					hough[degree][(int)r] ++;
					if (max_hough < hough[degree][(int)r])max_hough = hough[degree][(int)r];
				}
			}
		}
	}
	cout << "max_hough = " << max_hough << endl;

	//输出直线轨迹
	CImg<unsigned char> output1(width, height, 1, 1);
	output1.fill(0);

	//设置阈值
	int threshold = int(max_hough * value);
	cout << "threshold = " << threshold << endl;
	int count = 0;
	vector<pair<int, int> > lines;
	//遍历hough空间，找到所有比阈值大的点
	for (int row = 0; row < hough_space; row ++) {
		for (int col = 0; col < 2 * max_length; col ++) {
			bool newLines = true;
			int temp = hough[row][col];
			if (hough[row][col] > threshold) {
				for (int k = 0; k < lines.size(); k ++) {
					//判断极值
					if ((abs(lines[k].first - row) < 15 || abs((500 - lines[k].first) + row) < 5) && abs(lines[k].second - col) < 300) {
						if (hough[row][col] > hough[lines[k].first][lines[k].second]) {
							lines[k].first = row;
							lines[k].second = col;
						}
						newLines = false;
					}
				}
				if (newLines) {
					lines.push_back(make_pair(row, col));
					//cout << "push " << row << " "<< col << endl;
				}
			}
		}
	}



	//角点
	vector<pair<int, int> > node;

	for (int k = 0; k < lines.size(); k ++) {
		int row = lines[k].first;
		int col = lines[k].second;
		//cout << "line " << k << " = " << row << " " << col << endl;
		double dy = sin(row * hough_intervals);
		double dx = cos(row * hough_intervals);
		if ((row <= hough_space / 4 ) || (row >= 3 * hough_space / 4)) {
			for (int sRow = 0; sRow < height; ++sRow) {
				int sCol;
				if (row == 0 || row == 500)sCol =  (int)(col - max_length) + centerX;
				sCol = (int)((col - max_length - ((sRow - centerY) * dy)) / dx) + centerX;
				if (sCol < width && sCol >= 0) {
					if((int)output1.atXYZC(sCol, sRow, 1, 0) == 255)node.push_back(make_pair(sCol, sRow));
					else output1.atXYZC(sCol, sRow, 1, 0) = (unsigned char)255;
				}
			}
		}
		else {
			for (int sCol= 0; sCol < width; ++sCol) {
				int sRow;
				if(row == 250)sRow = (int)(col - max_length) + centerY;
				sRow = (int)((col - max_length - ((sCol - centerX) * dx)) / dy) + centerY;
				if (sRow < height && sRow >= 0) {
					if((int)output1.atXYZC(sCol, sRow, 1, 0) == 255)node.push_back(make_pair(sCol, sRow));
					else output1.atXYZC(sCol, sRow, 1, 0) = (unsigned char)255;
				}
			}
		}
	}

	//在原图上标记
	CImg<unsigned char> output2(scrImage);

	//标记
	for (int k = 0; k < lines.size(); k ++) {
		unsigned int w = output2.width();
		unsigned int h = output2.height();

		int range = 50;

		cout << "node x = " << node[k].first << " " << "   y = " << node[k].second << endl;

		for (int c = -range; c < range; c ++) {
			for (int r = -range; r < range; r ++) {
				int distance = (int)sqrt(c * c + r * r + 0.0);
				if (node[k].first>= range && node[k].first < width - range && node[k].second >= range && node[k].second < height - range) {
					if (distance <= 50 && node[k].first + c >= 0 && node[k].first + c < width && node[k].second + r >= 0 && node[k].second + r < height) {
						output2.atXYZC(node[k].first + c, node[k].second + r, 1, 0) = (unsigned char)(255);
						output2.atXYZC(node[k].first + c, node[k].second + r, 1, 1) = (unsigned char)(0);
						output2.atXYZC(node[k].first + c, node[k].second + r, 1, 2) = (unsigned char)(255);
					}
				}
			}
		}
	}

	output.display("hough_space");
	output1.display("edges");

	return output2;
}
