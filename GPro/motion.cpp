#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "header.h"
#include <algorithm>

using namespace cv;
using namespace std;

Mat srcw, src_grayw;
Mat dstw, detected_edgesw;
int edgeThreshw = 1;
int lowThresholdw;
int const max_lowThresholdw = 100;
int ratiow = 3;
int kernel_sizew = 3;
const char* window_namew = "Edge Map";

char* trackbar_valueW = "T for cut";
int const max_valueW = 100;
int threshold_valueW = 25;

// for optical flow para 1
char* tn_value = "T for Optical";
int t_value = 10;
int m_value = 100;

// for optical flow para 2
char* tn_value2 = "T for canny";
int t_value2 = 10;
int m_value2 = 200;

void GoProcess() {

	// add your file name
	VideoCapture cap("E:/opencvPracticles/sample-1- France vs Brazil- 360p.mp4");
	namedWindow("prew", CV_WINDOW_NORMAL);
	createTrackbar(tn_value, "prew", &t_value, m_value);
	createTrackbar(tn_value2, "prew", &t_value2, m_value2);

	Mat flow, frame;
	// some faster than mat image container
	UMat  flowUmat, prevgray;
	
	while(true)
	{
		cap.grab();
		bool Is = cap.grab();

		if (Is == false) {
			// if video capture failed
			cout << "Video Capture Fail" << endl;
			break;
		} else {

			Mat img;
			Mat original;

			// capture frame from video file
			cap.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
			resize(img, img, Size(640, 480));

			// save original for later
			img.copyTo(original);

			// just make current frame gray
			cvtColor(img, img, COLOR_BGR2GRAY);
			Canny(img, img, t_value2, 100);


			if (prevgray.empty() == false) {
				//goodFeaturesToTrack()
				//calcOpticalFlowPyrLK()
				
				calcOpticalFlowFarneback(prevgray, img, flowUmat, 0.4, 1, 12, 2, 2, 1.2, 0);
				// copy Umat container to standard Mat
				flowUmat.copyTo(flow);
				

				// By y += 5, x += 5 you can specify the grid 
				for (int y = 0; y < original.rows; y += 5) {
					for (int x = 0; x < original.cols; x += 5)
					{
						// get the flow from y, x position * 10 for better visibility
						const Point2f flowatxy = flow.at<Point2f>(y, x) * t_value;
						
						//calculate points distance
						double res =  norm(Point(y, x)- Point(cvRound(y + flowatxy.y), cvRound(x + flowatxy.x)) );
						
						if (res > 4) {
							// draw line at flow direction
							line(original, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255, 0, 0));
						}

						// draw initial point
						//circle(original, Point(x, y), 1, Scalar(0, 0, 0), -1);
					
					}
				}

				// draw the results
				namedWindow("prew", WINDOW_AUTOSIZE);
				imshow("prew", original);
				// draw the results
				

				// fill previous image again
				img.copyTo(prevgray);

			} else {

				// fill previous image in case prevgray.empty() == true
				img.copyTo(prevgray);

			}

			int key1 = waitKey(20);
		}
	}

}
