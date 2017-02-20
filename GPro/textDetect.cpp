#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

#include "opencv2\videoio.hpp"
#include "opencv2\video\video.hpp"
#include "opencv2\video\tracking_c.h"
#include "opencv2\video\tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>

using namespace cv;
//using namespace cv;
using namespace std;

/// Global variables


Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold = 10;
int const max_lowThreshold = 300;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

 //bool** diff;

char* trackbar_value_t = "T";
int threshold_value_t = 20;
int max_value_t = 255;
void DrawChart(int[], int );

/** @function main */
int mainText() {


	VideoCapture cap("E:/opencvPracticles/T2.mp4"); // open the video file for reading

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	namedWindow("Diff_video", CV_WINDOW_NORMAL);
	
	double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video

	createTrackbar(trackbar_value_t, "Diff_video", &threshold_value_t, max_value_t);

	Mat frameColor;
	Mat frameGray1;
	Mat frameGray2;
	Mat diffFrame;
	Mat framefinal;

	Mat fistIamge;
	bool bSuccess = cap.read(fistIamge);
	cvtColor(fistIamge, fistIamge, CV_BGR2GRAY);
	threshold(fistIamge, fistIamge, lowThreshold, 255, edgeThresh);
	//threshold(fistIamge, fistIamge, lowThreshold, 255, edgeThresh);
	if (!bSuccess) //if not success, break loop
	{
		cout << "Cannot read the frame from video file" << endl;
		
	}
	int height = fistIamge.rows;
	int width = fistIamge.cols;


	/*height = height / 3;
	width = width / 3;*/

/*
	diff = new double*[height];
	for (int i = 0; i < height; ++i)
		diff[i] = new double[width];*/


	Mat oop = Mat(height, width, CV_8UC1);
	//imshow("fist Image", fistIamge);

	int frameCount = 0;
	while (1)
	{
		
		frameCount++;
		
			bool bSuccess1 = cap.read(frameColor); // read a new frame from video
			if (!bSuccess1) //if not success, break loop
			{
				cout << "ERROR: Cannot read a frame from video file" << endl;
				break;
			}

			cvtColor(frameColor, frameGray1, CV_BGR2GRAY);
			//threshold(frameGray1, frameGray1, lowThreshold, 255, edgeThresh);

			bool bSuccess2 = cap.read(frameColor); // read a new frame from video
			if (!bSuccess1) //if not success, break loop
			{
				cout << "ERROR: Cannot read a frame from video file" << endl;
				break;
			}

			cvtColor(frameColor, frameGray2, CV_BGR2GRAY);
			//threshold(frameGray2, frameGray2, lowThreshold, 255, edgeThresh);
			subtract(frameGray1, frameGray2, diffFrame);


			for (size_t i = 0; i < height; i++)
			{
				for (size_t j = 0; j < width; j++)
				{
					if (oop.at<uchar>(i, j) < 255) {
						int tempdif = (int)diffFrame.at<uchar>(i, j);
						int tempOop = (int)oop.at<uchar>(i, j);
						int chec = tempdif + tempOop;
						if (chec >= 255) {
							oop.at<uchar>(i, j) = 255;
						} else {
							oop.at<uchar>(i, j) = chec;
						}

					}
				}
			}
			// after 150 frames final threshold comes as out put
 			if (frameCount > 180) {

				Mat mat;
				oop.copyTo(mat);
				for (size_t i = 0; i < height; i++)
				{
					for (size_t j = 0; j < width; j++)
					{
						if (mat.at<uchar>(i, j)<254) {
							mat.at<uchar>(i, j) = 255;
						}
						else {
							mat.at<uchar>(i, j) = 0;
						}
					}
				}

				imshow("Final", mat);
				int x1, y1, x2, y2;

				for (size_t i = 0; i < height; i++)
				{
					for (size_t j = 0; j < width; j++)
					{
						if ((int)mat.at<uchar>(i, j) == 255) {
							x1 = j;
							y1 = i;
							break;
						}
					}
				}

				for (size_t i = (height - 1); i >0; i--)
				{
					for (size_t j = (width - 1); j > 0; j--)
					{
						if ((int)mat.at<uchar>(i, j) == 255) {
							x2 = j;
							y2 = i;
							break;
						}
					}
				}

				cout << "x1 , Y1 :" << x1 << "," << y1 << "x2, Y2 :" << x2 << "," << y2 << endl;

				//	Mat oop = Mat( abs(x2-x1), abs(y2 - y1), CV_8UC1);
				rectangle(
					frameColor,
					Point(x1, y1),
					Point(x2, y2),
					Scalar(255, 255, 255),5
					);

				//system("pause");
				imshow("COLOR_video", frameColor);
			
			}
			double frameNo = cap.get(CV_CAP_PROP_POS_FRAMES);
			cout << "FRAME - :" << frameCount << endl;
			imshow("Diff_video", oop);
			//imshow("color", oop);

			if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
			{
				cout << "esc key is pressed by user" << endl;
				break;
			}
		
		

	}



}

void DrawChart(int arr[], int size) {

	int width = size ;
	int height = 500;
	Mat histImagee(height, width, CV_8UC1, Scalar(0, 0, 0));




	for (int i = 0; i < width; i++)
	{
		line(histImagee, Point(i, height),
			Point(i, height -  arr[i]),
			Scalar(255, 0, 0), 2, 8, 0);
	}

	imshow("chart -", histImagee);

}