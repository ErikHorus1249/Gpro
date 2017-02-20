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

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "T for cut";

int threshold_value = 16;
int threshold_type = 1;

// for msd avarage Logo transion
char* trackbar_value_msd = "T for logo";
int threshold_value_msd = 25;
int const max_value_msd = 60;

int const max_value = 100;
int const max_type = 4;
int const max_BINARY_value = 255;
static string filename = "xmlFile.xml";
bool DebugMode = false;


//File Writer ...
ofstream myfile;

//arrays
double** imd; // store MSD array..
// vectors
vector<double> msdVector;

// Methods
void DrawHist11(int, int, Mat, int);
void DrawChart(vector<double>,String);

FileStorage fs(filename, FileStorage::WRITE);
int SearchMax(vector<double>);
double tmpMsd=0;

// Boundary Detection Algorithm
void runProgramX() {

	VideoCapture cap("E:/opencvPracticles/vb_frans_brazil.mp4");
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "ERROR: Cannot open the video file" << endl;
	}

	namedWindow("MainVid", CV_WINDOW_NORMAL); //create a window called "MyVideo"
	
	//createTrackbar(trackbar_type, "MyVideo--", &threshold_type, max_type);
	createTrackbar(trackbar_value, "MainVid", &threshold_value, max_value);
	createTrackbar(trackbar_value_msd, "MainVid", &threshold_value_msd, max_value_msd);

	Mat frameColor;
	Mat frameGray1;
	Mat frameGray2;
	Mat diffFrame;

	bool bSuccess1 = cap.read(frameColor); // read a new frame from video
	if (!bSuccess1) //if not success, break loop
	{
		cout << "ERROR: Cannot read a frame from video file" << endl;
	}

	// open FIle Writer
	myfile.open("test.txt");
	// CREATING XML writer
	

	//cap.set(CV_CAP_PROP_POS_FRAMES, 15000);
	int height = frameColor.rows;
	int width = frameColor.cols;

	//imageDiffArray
	imd = new double*[height ];
	for (int i = 0; i < height ; ++i)
		imd[i] = new double[width];

	double maxValueAtLogoTransition=0;
	while (1)
	{
		double frameNo = cap.get(CV_CAP_PROP_POS_FRAMES);
		//cout << "FRAME - :" << frameNo << endl;


		bool bSuccess1 = cap.read(frameColor); // read a new frame from video
		if (!bSuccess1) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}
		cvtColor(frameColor, frameGray1, CV_RGB2GRAY);

		bool bSuccess2 = cap.read(frameColor); // read a new frame from video
		if (!bSuccess2) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}
		cvtColor(frameColor, frameGray2, CV_BGR2GRAY);
		
		/*
		double MSD = norm(frameGray1, frameGray2);

		MSD = MSD * MSD / frameGray1.total();
		if(MSD > threshold_value)
			cout << "# Mean Square Deviation - :" << MSD << endl;
		*/

		double diffVal=0;
		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				diffVal = (int)frameGray1.at<uchar>(i , j ) - (int)frameGray2.at<uchar>(i, j);
				diffVal = diffVal*diffVal;
				imd[i][j] = diffVal;
			}
		}

		double Tot = 0;
		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				Tot += imd[i][j];
			}
		}

		double MSD = sqrt(  Tot / frameColor.total()) ;
		tmpMsd += MSD;
		//cout << "# Tot"<< Tot<< " height*width - :" << height*width << endl;
		//if (MSD > threshold_value){
			int milliseconds = cap.get(CAP_PROP_POS_MSEC);
			
			int seconds = (int)(milliseconds / 1000) % 60;
			int minutes = (int)((milliseconds / (1000 * 60)) % 60);
			int hours = (int)((milliseconds / (1000 * 60 * 60)) % 24);
		
			
			string timeInFormat= to_string(milliseconds);
			//writing to text file ..
			
	//	}

		msdVector.push_back(MSD);
		DrawChart(msdVector,"1");
		
		int sizeAr = msdVector.size();
		// for cut detection technic
		if (sizeAr > 3) {
			int rightGradient = abs((int)msdVector.at(sizeAr - 3) - (int)msdVector.at(sizeAr - 2));
			int leftGradient = abs((int)msdVector.at(sizeAr - 2) - (int)msdVector.at(sizeAr - 1));

			if ((rightGradient>threshold_value) && (leftGradient>threshold_value)) {
				cout << "@@@  CUT DETECTED " << endl;

				// save to text file
				myfile << "@ CUT TRANSION - " << frameNo << " ||   at :: " << hours << ":" << minutes << ":" << seconds << " \n";
				//myfile << "@@@  CUT DETECTED " << endl;
				
				//save to xml file
				if (fs.isOpened()) {
					fs << "MSD";                              
					fs << "{" << "frameID" << frameNo;
					fs << "time" << timeInFormat;
					fs << "transition" << "CUT" << "}";
				}
			}else if ((rightGradient>threshold_value) && (leftGradient > (threshold_value/4) )) {
				cout << "@@@  CUT DETECTED " << endl;

				//save to text file
				myfile << "@ CUT TRANSION - " << frameNo << " ||   at :: " << hours << ":" << minutes << ":" << seconds << " \n";
				//save to xml file
				if (fs.isOpened()) {
					fs << "MSD";
					fs << "{" << "frameID" << frameNo;
					fs << "time" << timeInFormat;
					fs << "transition" << "CUT" << "}";
				}
			}



		}
			// ........      LOGO TRANSION ALGORITHM     ....... // 
			double avgMSD = 0;
			avgMSD = tmpMsd / msdVector.size();
			int vecSize = msdVector.size();
			if(frameNo)
				if (vecSize > 16) {
					vector<double> tmp;
					for (size_t i = 15; i > 0; i--) {
						tmp.push_back(msdVector.at(vecSize - i));
						//cout << "**** frame "<< vecSize - i<<" value" << msdVector.at(vecSize - i) << endl;
					}

					if (abs(avgMSD - tmp.at(8)) > threshold_value_msd) {
							auto biggest = max_element(begin(tmp), end(tmp));
							auto smallest = min_element(begin(tmp), end(tmp));
							int maxPosition, MinPosision;
							//MinPosision = distance(begin(tmp), smallest);
							maxPosition = distance(begin(tmp), biggest);
							
							if (maxValueAtLogoTransition != 0) {
								if (maxValueAtLogoTransition != (double)*biggest) {
									if (maxPosition < 14 && maxPosition>0) {
										int rightGradient = abs((int)tmp.at(maxPosition) - (int)tmp.at(maxPosition - 1));
										int leftGradient = abs((int)tmp.at(maxPosition + 1) - (int)tmp.at(maxPosition));

										if (!(rightGradient > threshold_value_msd && leftGradient>threshold_value_msd)) {
											cout << "@ LOGO TRANSION - " << frameNo << " ||   At :: " << hours << ":" << minutes << ":" << seconds << endl;
											//cout << "Max element is " << *biggest<< " at position " << maxPosition << endl;
											//cout << "min element is " << *smallest<< " at position " << MinPosision << endl;
											DrawChart(tmp, "2");
											rectangle(
												frameColor,
												Point(0, 0),
												Point(width, height),
												Scalar(255, 255, 255), 50
												);
											// save to text
											myfile << "@ LOGO TRANSION - " << frameNo << " ||   at :: " << hours << ":" << minutes << ":" << seconds << " \n";
											//save to xml
											if (fs.isOpened()) {
												fs << "MSD";
												fs << "{" << "frameID" << frameNo;
												fs << "time" << timeInFormat;
												fs << "transition" << "LOGO" << "}";
											}

										}
									}
									
								}

							}
							maxValueAtLogoTransition = (double)*biggest;						
					}
					tmp.clear();
				}

			if (DebugMode) {
				cout << "# Mean Square Deviation - :" << MSD << " | avg MSD: " << avgMSD << " ||   At :: " << hours << ":" << minutes << ":" << seconds << endl;
			}
			//myfile << "Frame ID " << frameNo << " | msd :" << MSD << " " << " | avg MSD: " << avgMSD << " ||   At :: " << hours << ":" << minutes << ":" << seconds << " \n";
			
			

		imshow("MainVid", frameColor); //show the frame in "MyVideo" window
	
		if (waitKey(50) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	myfile.close();
	fs.release();
}

int SearchMax(vector<double> arr) {

	int tmp=0;
	for (size_t i = 0; i < arr.size(); i++)
	{
		if (arr.at(i) > tmp) {
			tmp = arr.at(i);
		}
	}
	return tmp;
}


void DrawChart(vector<double> msdAry,String a) {

	int width = msdAry.size();
	int height = 400;
	Mat histImage(height, width, CV_8UC1, Scalar(0, 0, 0));
	



	for (int i = 0; i < width; i++)
	{
		line(histImage, Point(i , height),
			Point(i, height - 5*msdAry.at(i)),
			Scalar(255,0, 0), 2, 8, 0);
	}
	
	imshow("HistGram "+a, histImage);

}

void DrawHist11(int height2, int width2, Mat img2, int count) {


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
	// << "Max Value :" << max << endl;

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

	//delete[] binH;
	

	string h2 = "HistTable2 " + count;
	string h = "HistTable1 " + count;
	namedWindow(h, 1);
	imshow(h, histImage);

}

void runProgram2() {

	VideoCapture cap("E:/opencvPracticles/Cricket.mp4");

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "ERROR: Cannot open the video file" << endl;

	}


	//cvNamedWindow("MyWindow", CV_WINDOW_NORMAL);
	namedWindow("MyVideo", CV_WINDOW_NORMAL); //create a window called "MyVideo"
	namedWindow("Diff_video", CV_WINDOW_NORMAL);
	//namedWindow("MyVideo-Color", CV_WINDOW_NORMAL);

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame Size = " << dWidth << "x" << dHeight << endl;

	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

	// adding track bars for thresold values...
	createTrackbar(trackbar_type, "MyVideo", &threshold_type, max_type);

	createTrackbar(trackbar_value, "MyVideo", &threshold_value, max_value);

	cap.set(CV_CAP_PROP_POS_FRAMES, 900);


	int count = 0;
	while (1)
	{
		double frameNo = cap.get(CV_CAP_PROP_POS_FRAMES);
		cout << "FRAME - :" << frameNo << endl;

		Mat frameColor;
		Mat frameGray1;
		Mat frameGray2;
		Mat diffFrame;

		bool bSuccess1 = cap.read(frameColor); // read a new frame from video
		if (!bSuccess1) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}

		cvtColor(frameColor, frameGray1, CV_BGR2GRAY);

		bool bSuccess2 = cap.read(frameColor); // read a new frame from video
		if (!bSuccess2) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}

		cvtColor(frameColor, frameGray2, CV_BGR2GRAY);


		//cvtColor(frameColor, frameGray, CV_BGR2HSV);
		threshold(frameGray1, frameGray1, threshold_value, 255, threshold_type);
		//GaussianBlur(frameGray1, frameGray1, Size(5, 5), 0, 0);

		threshold(frameGray2, frameGray2, threshold_value, 255, threshold_type);
		//GaussianBlur(frameGray2, frameGray2, Size(5, 5), 0, 0);

		// this is for slow motion detection..
		subtract(frameGray1, frameGray2, diffFrame);

		imshow("Diff_video", diffFrame);

		//imshow("MyVideo", frameGray2); //show the frame in "MyVideo" window
		imshow("MyVideo-Color", frameColor);

		if (waitKey(50) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}


	}
}

void runProgram() {

	VideoCapture cap("E:/opencvPracticles/volleyball.mp4");

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "ERROR: Cannot open the video file" << endl;

	}


	//cvNamedWindow("MyWindow", CV_WINDOW_NORMAL);
	namedWindow("MyVideo", CV_WINDOW_NORMAL); //create a window called "MyVideo"
	namedWindow("Diff_video", CV_WINDOW_NORMAL);
	//namedWindow("MyVideo-Color", CV_WINDOW_NORMAL);

	//double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	//double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	// adding track bars for thresold values...
	createTrackbar(trackbar_type, "MyVideo", &threshold_type, max_type);

	createTrackbar(trackbar_value, "MyVideo", &threshold_value, max_value);

	//cap.set(CV_CAP_PROP_POS_FRAMES, 900);

	Mat frameColor;
	Mat frameGray1;
	Mat frameGray2;
	Mat diffFrame;

	int count = 0;
	while (1)
	{
		double frameNo = cap.get(CV_CAP_PROP_POS_FRAMES);
		cout << "FRAME - :" << frameNo << endl;



		bool bSuccess1 = cap.read(frameColor); // read a new frame from video
		if (!bSuccess1) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}


		cvtColor(frameColor, frameGray1, CV_BGR2HSV);
		Canny(frameColor, frameGray1, threshold_value, threshold_value);

		/*
		bool bSuccess2 = cap.read(frameColor); // read a new frame from video
		if (!bSuccess2) //if not success, break loop
		{
		cout << "ERROR: Cannot read a frame from video file" << endl;
		break;
		}

		cvtColor(frameColor, frameGray2, CV_BGR2GRAY);
		Canny(frameColor, frameGray2, 100, 100);


		//cvtColor(frameColor, frameGray, CV_BGR2HSV);
		//threshold(frameGray1, frameGray1, threshold_value, 255, threshold_type);
		//GaussianBlur(frameGray1, frameGray1, Size(5, 5), 0, 0);

		//threshold(frameGray2, frameGray2, threshold_value, 255, threshold_type);
		//GaussianBlur(frameGray2, frameGray2, Size(5, 5), 0, 0);

		// this is for slow motion detection..
		subtract(frameGray1, frameGray2, diffFrame);

		int h  = diffFrame.rows;
		int w = diffFrame.cols;

		DrawHist11(h, w, diffFrame, 1);

		imshow("Diff_video", diffFrame);
		*/


		imshow("MyVideo", frameGray1); //show the frame in "MyVideo" window

									   //imshow("MyVideo-Color", frameGray1);

		if (waitKey(50) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}


	}
}
