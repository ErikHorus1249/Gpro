#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

char* trackbar_value_t1 = "T1";
int threshold_value_t1 = 10;
int max_value_t1 = 255;

char* trackbar_value_t2 = "T2";
int threshold_value_t2 = 10;
int max_value_t2 = 255;

void  SuccusiveFiffOparatorw(Mat, Mat, int, int);
void salt(Mat &image, int n);
void colorReduce(cv::Mat &image, int div);
void help()
{
	cout << "\nThis program demonstrates line finding with the Hough transform.\n"
		"Usage:\n"
		"./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}


void ProcessImage() {



	VideoCapture cap("E:/opencvPracticles/vb_frans_brazil.mp4");
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "ERROR: Cannot open the video file" << endl;
	}
	Mat src,cpy;
	namedWindow("source", CV_WINDOW_NORMAL); //create a window called "MyVideo"
	
											 //createTrackbar(trackbar_type, "MyVideo--", &threshold_type, max_type);
	createTrackbar(trackbar_value_t1, "source", &threshold_value_t1, max_value_t1);
	createTrackbar(trackbar_value_t2, "source", &threshold_value_t2, max_value_t2);
	while (1)
	{

		bool bSuccess1 = cap.read(src); // read a new frame from video
		cvtColor(src, src, CV_RGB2GRAY);
		src.copyTo(cpy);
		if (!bSuccess1) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}


		int h = src.rows;
		int w = src.cols;
		SuccusiveFiffOparatorw(src, cpy, h, w);

		if (waitKey(50) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		//imshow("source", src);
		//imshow("MainVid", cdst);

	}


	/*
	VideoCapture cap("E:/opencvPracticles/vb_frans_brazil.mp4");
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "ERROR: Cannot open the video file" << endl;
	}
	Mat src;
	namedWindow("MainVid", CV_WINDOW_NORMAL); //create a window called "MyVideo"
									  //createTrackbar(trackbar_type, "MyVideo--", &threshold_type, max_type);
	createTrackbar(trackbar_value_t1, "MainVid", &threshold_value_t1, max_value_t1);
	createTrackbar(trackbar_value_t2, "MainVid", &threshold_value_t2, max_value_t2);
	while (1)
	{
		
		bool bSuccess1 = cap.read(src); // read a new frame from video
		if (!bSuccess1) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}
	
	if (src.empty())
	{
		help();
		
		
	}

	Mat dst, cdst;
	Canny(src, dst, threshold_value_t1, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);

#if 0
	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}
#else
	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
#endif
	imshow("source", src);
	imshow("MainVid", cdst);

	if (waitKey(50) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
	{
		cout << "esc key is pressed by user" << endl;
		break;
	}


	}
	*/

}

void  SuccusiveFiffOparatorw(Mat img2, Mat copyImg, int height, int width) {
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

			if (G >= threshold_value_t2) {
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
void colorReduce(cv::Mat &image, int div) {
	int nl = image.rows; // number of lines
						 // total number of elements per line
	int nc = image.cols * image.channels();
	for (int j = 0; j < nl; j++) {
		// get the address of row j
		uchar* data = image.ptr<uchar>(j);
		for (int i = 0; i < nc; i++) {
			// process each pixel ---------------------
			data[i] = data[i] / div*div + div / 2;
			// end of pixel processing ----------------} 
			// end of line
		}
	}
}

void salt(Mat &image, int n) {
	for (size_t k = 0; k < n; k++)
	{
		int i = rand() % image.rows;
		int j = rand() % image.cols;
		if(image.channels() ==1){
			image.at<uchar>(i, j) = 255;
		}
		else if (image.channels() == 3) {
			image.at<Vec3b>(i, j)[0] = 255;
			image.at<Vec3b>(i, j)[1] = 255;
			image.at<Vec3b>(i, j)[2] = 255;
		}
	}


}