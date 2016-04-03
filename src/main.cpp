#include "hough_line.h"

//img1.bmp 二值化阈值=60 , 筛选边界直线方程阈值=0.5
//img2.bmp 二值化阈值=100 , 筛选边界直线方程阈值=0.5
//img3.bmp 二值化阈值=85 , 筛选边界直线方程阈值=0.5
//img4.bmp 二值化阈值=80 , 筛选边界直线方程阈值=0.5
//img5.bmp 二值化阈值=100, 筛选边界直线方程阈值=0.5
//img6.bmp 二值化阈值=70, 筛选边界直线方程阈值=0.5


int main() {
	while(1) {
		string name;
		cin >> name;
		name = "images/" + name;
 		CImg<unsigned char> img(name.data());
 		int t1;
 		cin >> t1;
 		CImg<unsigned char> img1 = edge(img, t1);
		img1.display("My first CImg code");
		double t2;
		cin >> t2;
		CImg<unsigned char> img2 = HoughTrans(img1, img, t2);
		img2.display("img2");
 	}
 	return 0;
}