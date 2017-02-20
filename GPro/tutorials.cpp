
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

void drawHistrogramMap(int, int, Mat, int);
void SketchImage(int, int, Mat);
void meanFilter(Mat, Mat);
void medianFiler(Mat, Mat);
void sortToAsc(int[], int);
void modeFiler(Mat, Mat);
int returnMode(int[], int);
void  SobalOparator(Mat, Mat, int, int);
void  SuccusiveFiffOparator(Mat, Mat, int, int);
void DrawHist(int, int, Mat, int);


void CallMyMethod(pair<const int, int>& pair)
{
	cout << pair.first << "    |    " << pair.second << endl;
}

void runImage() {

	//     ----------------------------------  Histrogram     -------------------------------------
	Mat img;
	Mat copyImg;
	//img = imread("H:/Projects/OpenCv/Dog.jpg");
	img = imread("rat.png");
	//img = imread("Koala.jpg", CV_LOAD_IMAGE_GRAYSCALE);   

	// Read the file
	if (!img.data) {
		printf("Could not load image");
		system("pause");
		waitKey();
	}

	int height = img.rows;
	int width = img.cols;

	Mat img2;

	cvtColor(img, img2, CV_RGB2GRAY);
	copyImg = img2.clone();



	namedWindow("Histrogram", 1);
	imshow("Histrogram", img2);

	namedWindow("Original");
	imshow("Original", copyImg);

	cvWaitKey(0);

}


void DrawHist(int height2, int width2, Mat img2, int count) {


	int hBin[256];
	int binH[256];

	double N = height2*width2; // Number of Pixels...

	double Prob[256];
	double CProb[256];
	int NEW[256];

	for (int n = 0; n < 256; n++)
	{
		hBin[n] = 0;
		binH[n] = 0;

		Prob[n] = 0;
		CProb[n] = 0;
		NEW[n] = 0;

	}

	int m = 0;

	for (int i = 0; i < height2; i++)
	{
		for (int j = 0; j < width2; j++)
		{
			m = (int)img2.at<uchar>(i, j);
			hBin[m] = hBin[m] + 1;
		}
	}

	double max = 0;

	for (int h = 0; h < 256; h++)
	{
		float binVal = hBin[h];
		if (max < binVal)
			max = binVal;
	}

	// creating Hist ------------------------------ start 
	cout << "Max Value :" << max << endl;

	for (int h = 0; h < 256; h++)
	{
		binH[h] = cvRound((double)hBin[h] / max * 400);
	}

	int hist_w = 512;
	int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);
	//gap bitween 2 bin

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));

	for (int i = 0; i < 256; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h),
			Point(bin_w*(i - 1), hist_h - binH[i - 1]),
			Scalar(255, 0, 0), 2, 8, 0);
	}

	// creating Hist ------------------------------ end


	for (int h = 0; h < 256; h++)
	{
		Prob[h] = (double)hBin[h] / N;
	}

	CProb[0] = Prob[0];

	//creating cumulative probability
	for (int i = 1; i < 256; i++)
	{
		CProb[i] = CProb[i - 1] + Prob[i];

	}
	for (int i = 0; i < 256; i++)
	{
		NEW[i] = cvRound(255 * CProb[i]);
	}

	for (int i = 0; i < height2; i++)
	{
		for (int j = 0; j < width2; j++)
		{
			m = (int)img2.at<uchar>(i, j);
			img2.at<uchar>(i, j) = NEW[m];

		}
	}

	string h2 = "HistTable2 " + count;
	string h = "HistTable1 " + count;
	namedWindow(h, 1);
	imshow(h, histImage);

}

/* -----------------         SOBEL OPARATOR      --------------------------------    */
void  SobalOparator(Mat img2, Mat copyImg, int height, int width) {

	int maxG = 10;
	int Gx = 0, Gy = 0, G = 0;
	for (int i = 1; i < (height - 1); i++) {
		for (int j = 1; j < (width - 1); j++) {

			Gx = 0;
			Gy = 0;

			Gx += ((int)img2.at<uchar>(i - 1, j + 1) + 2 * (int)img2.at<uchar>(i, j + 1) + (int)img2.at<uchar>(i + 1, j + 1))
				- ((int)img2.at<uchar>(i - 1, j - 1) + 2 * (int)img2.at<uchar>(i, j - 1) + (int)img2.at<uchar>(i + 1, j - 1));

			Gy += ((int)img2.at<uchar>(i - 1, j - 1) + 2 * (int)img2.at<uchar>(i - 1, j) + (int)img2.at<uchar>(i - 1, j + 1))
				- ((int)img2.at<uchar>(i + 1, j - 1) + 2 * (int)img2.at<uchar>(i + 1, j) + (int)img2.at<uchar>(i + 1, j + 1));

			//G = sqrt(Gx*Gx + Gy*Gy);
			G = abs(Gx) + abs(Gy);

			if (G > maxG) {
				maxG = G;
			}

			int T = maxG* 0.1;

			//int T = 75;
			if (G >= T) {
				copyImg.at<uchar>(i, j) = 255;
			}
			else {
				copyImg.at<uchar>(i, j) = 0;
			}


			/*
			int T = 90;
			if (G >= T) {
			copyImg.at<uchar>(i, j) = 255;
			} else if ( 3* G/4 >= T) {
			copyImg.at<uchar>(i, j) = 180;
			}
			else if (G / 2 >= T) {
			copyImg.at<uchar>(i, j) = 128;
			}
			else if (  G / 4 >= T) {
			copyImg.at<uchar>(i, j) = 64;
			}
			else {
			copyImg.at<uchar>(i, j) = 0;
			}
			*/

		}
	}

	namedWindow("Histrogram Picture original", 1);
	imshow("Histrogram Picture original", img2);

	namedWindow("Histrogramed Picture", 1);
	imshow("Histrogramed Picture", copyImg);

}


void  SuccusiveFiffOparator(Mat img2, Mat copyImg, int height, int width) {
	int kernalSize = 2;
	int Gx = 0, Gy = 0, G = 0;

	for (int i = 0; i < (height - 1); i++) {
		for (int j = 0; j < (width - 1); j++) {
			Gx = 0;
			Gy = 0;

			Gx += (int)img2.at<uchar>(i, j) - (int)img2.at<uchar>(i, j + 1);
			Gy += (int)img2.at<uchar>(i, j) - (int)img2.at<uchar>(i + 1, j);

			G = abs(Gx) + abs(Gy);

			int T = 50;

			if (G >= T) {
				copyImg.at<uchar>(i, j) = 255;
			}
			else {
				copyImg.at<uchar>(i, j) = 0;
			}
		}
	}

	namedWindow("Histrogram Picture original", 1);
	imshow("Histrogram Picture original", img2);

	namedWindow("Histrogramed Picture", 1);
	imshow("Histrogramed Picture", copyImg);

}

int returnMode(int cal[], int size) {
	sortToAsc(cal, size);

	map<int, int> modeValueFair;
	int tmpMaxCount = 1;

	for (size_t i = 0; i < size; i++) {

		tmpMaxCount = 1;
		for (size_t j = 0; j < size; j++) {
			if (i != j) {
				if (cal[i] == cal[j]) {
					tmpMaxCount++;
				}
			}
		}
		modeValueFair[cal[i]] = tmpMaxCount;

	}

	unsigned currentMax = 0;
	unsigned arg_max = 0;

	for (auto it = modeValueFair.cbegin(); it != modeValueFair.cend(); ++it) {
		if (it->second > currentMax) {
			arg_max = it->first;
			currentMax = it->second;
		}
	}

	return arg_max;
}

void modeFiler(Mat img2, Mat copyImg) {

	cout << "Please insert mean value (odd value) :" << endl;
	int height = img2.rows;
	int width = img2.cols;

	int kernalSize;
	while (true) {
		cin >> kernalSize;
		cout << "Kernal Size:" << kernalSize << endl;
		if (kernalSize % 2 == 1) {
			break;
		}
		else {
			cout << "Please insert odd value : " << endl;
		}
	}

	int *cal = new int[kernalSize*kernalSize];

	for (size_t i = 0; i < 9; i++)
	{
		cal[i] = 0;
	}

	int var = (int)(kernalSize - 1) / 2;

	cout << "var:" << var << endl;
	for (int i = var; i < (height - var); i++) {
		for (int j = var; j < (width - var); j++) {
			for (size_t i = 0; i < 9; i++)
			{
				cal[i] = 0;
			}
			int y = 0;
			for (int m = i - var; m <= i + var; m++) {
				for (int n = j - var; n <= j + var; n++) {
					cal[y] = (int)img2.at<uchar>(m, n);
					y++;
				}
			}

			int d = returnMode(cal, kernalSize*kernalSize);
			copyImg.at<uchar>(i, j) = (uchar)d;

		}
	}

	namedWindow("Histrogram Picture original", 1);
	imshow("Histrogram Picture original", img2);

	namedWindow("Histrogramed Picture", 1);
	imshow("Histrogramed Picture", copyImg);
}

void medianFiler(Mat img2, Mat copyImg) {

	cout << "Please insert mean value (odd value) :" << endl;
	int height = img2.rows;
	int width = img2.cols;

	int kernalSize;
	while (true) {

		cin >> kernalSize;
		cout << "Kernal Size:" << kernalSize << endl;

		if (kernalSize % 2 == 1) {
			break;
		}
		else {
			cout << "Please insert odd value : " << endl;
		}
	}

	int *cal = new int[kernalSize*kernalSize];

	//for (size_t i = 0; i < kernalSize*kernalSize; i++)
	//{
	//	cal[i] = 0;
	//}

	int var = (int)(kernalSize - 1) / 2;

	cout << "var:" << var << endl;
	for (int i = var; i < (height - var); i++) {
		for (int j = var; j < (width - var); j++) {

			for (size_t i = 0; i < kernalSize*kernalSize; i++)
			{
				cal[i] = 0;
			}

			int y = 0;

			for (int m = i - var; m <= i + var; m++) {
				for (int n = j - var; n <= j + var; n++) {
					cal[y] = (int)img2.at<uchar>(m, n);
					y++;
				}
			}

			sortToAsc(cal, kernalSize*kernalSize);
			copyImg.at<uchar>(i, j) = (uchar)cal[((kernalSize*kernalSize) + 1) / 2];

		}
	}

	namedWindow("Histrogram Picture original", 1);
	imshow("Histrogram Picture original", img2);

	namedWindow("Histrogramed Picture", 1);
	imshow("Histrogramed Picture", copyImg);
}

void sortToAsc(int cal[], int size) {
	int max = cal[0];

	for (size_t i = 0; i < size; i++) {
		for (size_t j = i; j < size; j++) {
			int tmp = cal[i];
			if (cal[i] > cal[j]) {
				cal[i] = cal[j];
				cal[j] = tmp;
			}
		}
	}
}

void meanFilter(Mat img2, Mat copyImg) {

	cout << "Please insert mean value (odd value) :" << endl;
	int height = img2.rows;
	int width = img2.cols;

	int kernalSize;
	while (true) {
		cin >> kernalSize;
		cout << "Kernal Size:" << kernalSize << endl;
		if (kernalSize % 2 == 1) {
			break;
		}
		else {
			cout << "Please insert odd value : " << endl;
		}
	}


	int cal = 0, ave = 0;
	double tmp = (kernalSize - 1) / 2;
	int var = (int)tmp;
	cout << "var:" << var << endl;

	for (int i = var; i < (height - var); i++) {
		for (int j = var; j < (width - var); j++) {

			cal = 0;
			for (int m = i - var; m <= i + var; m++) {
				for (int n = j - var; n <= j + var; n++) {

					cal += (int)img2.at<uchar>(m, n);

				}
			}

			ave = cal / (kernalSize*kernalSize);
			copyImg.at<uchar>(i, j) = (uchar)ave;

		}
	}

	namedWindow("Histrogram Picture original", 1);
	imshow("Histrogram Picture original", img2);

	namedWindow("Histrogramed Picture", 1);
	imshow("Histrogramed Picture", copyImg);

}


void drawHistrogramMap(int height2, int width2, Mat img2, int count) {

	int hBin[256];
	int binH[256];

	for (int n = 0; n < 256; n++)
	{
		hBin[n] = 0;
		binH[n] = 0;
	}

	int m = 0;

	for (int i = 0; i < height2; i++)
	{
		for (int j = 0; j < width2; j++)
		{
			m = (int)img2.at<uchar>(i, j);
			hBin[m] = hBin[m] + 1;
		}
	}

	double max = 0;

	for (int h = 0; h < 256; h++)
	{
		float binVal = hBin[h];
		cout << "H :[" << h << "] :" << binVal << endl;

		if (max < binVal)
			max = binVal;

	}

	for (int h = 0; h < 256; h++)
	{
		binH[h] = cvRound((double)hBin[h] / max * 400);
	}

	int hist_w = 512;
	int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);
	//gap bitween 2 bin

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));

	for (int i = 0; i < 256; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h),
			Point(bin_w*(i - 1), hist_h - binH[i - 1]),
			Scalar(255, 0, 0), 2, 8, 0);
	}
	string name = "HistTable " + count;
	namedWindow(name, 1);
	imshow(name, histImage);


}

void SketchImage(int height2, int width2, Mat img2) {
	double a = 41;
	double b = 183;
	double c = 9;
	double d = 250;
	double L = 255;

	double alpha = c / a;
	double beta = (d - c) / (b - a);
	double gamma = (L - d) / (L - b);

	cout << "alpha ::" << alpha << endl;
	cout << "beta ::" << beta << endl;
	cout << "gamma ::" << gamma << endl;

	int x = 0;
	for (int i = 0; i < height2; i++) {
		for (int j = 0; j < width2; j++) {
			x = (int)img2.at<uchar>(i, j);

			if (x <= a) {
				img2.at<uchar>(i, j) = (uchar)cvRound(alpha*x);
			}
			else if (x <= b) {
				img2.at<uchar>(i, j) = (uchar)cvRound(beta*(x - a) + c);
			}
			else {
				img2.at<uchar>(i, j) = (uchar)cvRound(gamma*(x - b) + d);
			}

		}
	}

}

void runWithImage() {



	Mat src, dst;
	/// Load image
	src = imread("lena.jpg");

	if (!src.data)
	{
		cout << "image loading problem";
	}

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}




	/// Display
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);



	waitKey(0);

}
