#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2\videoio.hpp"
#include "opencv2\video\video.hpp"
#include "opencv2\video\tracking_c.h"
#include "opencv2\video\tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>

using namespace cv;
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
void DrawChart(int[], int);
Mat cummilativeMat;
vector<Mat> cumulativeMats;

/** @function main */
int mainText() {


	VideoCapture cap("E:/opencvPracticles/sample-3-USA vs Russia - World League 480p.mp4"); // open the video file for reading

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


	Mat oop = Mat(height, width, CV_8UC1);
	Mat addedMat= Mat(height, width, CV_8UC1);
	Mat finalOut;
	bool isFound = false;
	bool done = false;
	int videoLenght = cap.get(CV_CAP_PROP_FRAME_COUNT);
	int frameCount = 0;
	int frameCumulativeCount = 0;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

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

		int frameNo = cap.get(CV_CAP_PROP_POS_FRAMES);


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
					}
					else {
						oop.at<uchar>(i, j) = chec;
					}

				}
			}
		}

		// after 180 frames final threshold comes as out put
		if (frameCount > 190 && !done) {

			if(!isFound){


			oop.copyTo(cummilativeMat);
			for (size_t i = 0; i < height; i++)
			{
				for (size_t j = 0; j < width; j++)
				{
					if (cummilativeMat.at<uchar>(i, j)<254) {
						cummilativeMat.at<uchar>(i, j) = 255;
					}
					else {
						cummilativeMat.at<uchar>(i, j) = 0;
					}
				}
			}

			cout << countNonZero(cummilativeMat) << endl;
			if (countNonZero(cummilativeMat) > 100) {
				cout << "cumulative count :!!!! " << cumulativeMats.size() << endl;
				cumulativeMats.push_back(cummilativeMat);
			}
			oop = Mat(height, width, CV_8UC1);
			addedMat = Scalar::all(0);
			frameCount = 0;
			imshow("Final", cummilativeMat);
			//score board area .. .. . . . .
		
			cout << "cumulative count :" << cumulativeMats.size() << endl;
			if (cumulativeMats.size() > 3) {
				//cout << "I am in" << endl;
				for (size_t n = 0; n < cumulativeMats.size(); n++)
				{
					Mat tmp = cumulativeMats.at(n);
					if (countNonZero(tmp) > 200) {
						for (size_t i = 0; i < tmp.rows; i++)
						{
							for (size_t j = 0; j < tmp.cols; j++)
							{
								if (((int)tmp.at<uchar>(i, j) >= 250) && ((int)addedMat.at<uchar>(i, j) >= 250)) {

									addedMat.at<uchar>(i, j) = 255;
								}
								else if ((int)tmp.at<uchar>(i, j) != (int)addedMat.at<uchar>(i, j)) {

									addedMat.at<uchar>(i, j) = 255;
								}
								else {
									addedMat.at<uchar>(i, j) = 0;
								}
							}
						}
					}
				}
				addedMat.copyTo(finalOut);
				cumulativeMats.clear();
				//imshow("added:", addedMat);
				isFound = true;
				cout << "I am in end" << endl;
				imshow("end", addedMat);

			}
				
			}else{
				// ****
				GaussianBlur(finalOut, finalOut, Size(5, 5), 3, 4, 4);

				try {
					for (size_t i = 0; i < height; i++)
					{
						for (size_t j = 0; j < width; j++)
						{
							if ((int)finalOut.at<uchar>(i, j) == 255) {
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
							if ((int)finalOut.at<uchar>(i, j) == 255) {
								x2 = j;
								y2 = i;
								break;
							}
						}
					}

					cout << "x1 , Y1 :" << x1 << "," << y1 << "x2, Y2 :" << x2 << "," << y2 << endl;
					

				}
				catch (Exception e) {
					cout << e.msg << endl;
				}

				//isFound = false;

				done = true;
				
			}

		}
		rectangle(
			frameColor,
			Point(x1, y1),
			Point(x2, y2),
			Scalar(255, 255, 255), 5
			);
		//system("pause");

		imshow("COLOR_video", frameColor);
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

	int width = size;
	int height = 500;
	Mat histImagee(height, width, CV_8UC1, Scalar(0, 0, 0));

	for (int i = 0; i < width; i++)
	{
		line(histImagee, Point(i, height),
			Point(i, height - arr[i]),
			Scalar(255, 0, 0), 2, 8, 0);
	}

	imshow("chart -", histImagee);

}