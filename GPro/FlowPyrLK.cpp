#define _CRT_SECURE_NO_DEPRECATE
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <ctype.h>
#include <math.h>
#include <opencv2/video.hpp>
//C
#include <fstream>
#include <stdio.h>
#include <sstream>
// machine learninv=g
#include <opencv2/ml/ml.hpp>
#include <algorithm>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

using namespace cv;
using namespace std;
using namespace ml;
using namespace rapidxml;

Point2f point;
bool addRemovePt = false;

char* trackbar1 = "Threshold for Refresh Rate :";
int threshold1 = 50;
int const max_value1 = 500;

char* trackbar2forT = "Threshold for Rectangle :";
int threshold2forT = 32;
int const max_value2forT = 500;

int REFRESH_RATE = 100;
int refresh_count = 0;


xml_document<> docMotionService;
xml_document<> docMotionDash;
xml_document<> docMotionIdle;

static void onMouse(int event, int x, int y, int /*flags*/, void* /*param*/)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		point = Point2f((float)x, (float)y);
		addRemovePt = true;
	}
}

double lines[1080];

// angle calculation
// calculate Angles in Degrees
double angles[36]; // bins for histrogram

				   // bins for calculating motion .. 
float BinArrayAngle[36];
vector<float> CumilativeBin;

double calcDistance(Point P1, Point P2) {
	int xdel = abs(P2.x - P1.x);
	int ydel = abs(P2.y - P1.y);
	double res = sqrt(xdel*xdel + ydel*ydel);
	return res;
}

double calcAngle(Point P1, Point P2) {
	double Angle = -atan2(P2.y - P1.y, P2.x - P1.x) * 180.0 / CV_PI;
	if (Angle < 0) {
		Angle = 360 + Angle;
	}
	return abs(Angle);
}

void DrawChart(double arr[36], String a) {

	int width = 400;
	int height = 500;
	Mat histImage(height, width, CV_8UC1, Scalar(0, 0, 0));

	for (int i = 0; i < width; i++)
	{
		line(histImage, Point(i * 4, height),
			Point(i * 4, height - 5 * arr[i]),
			Scalar(255, 0, 0), 2, 8, 0);
	}

	imshow("HistGram--MSD " + a, histImage);

}

int findHighestValueBin(double arr[36]) {

	int MaxV = 0;
	int maxIndex = 0;
	for (size_t i = 0; i < 36; i++)
	{
		if (arr[i] > MaxV) {
			MaxV = arr[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

int findHighestValueBinInDis(double arr[1080]) {

	int MaxV = 0;
	int maxIndex = 0;
	for (size_t i = 0; i < 1080; i++)
	{
		if (arr[i] > MaxV) {
			MaxV = arr[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

void DetectDirection(int maxAnglebin, int maxDistance) {
	if (maxAnglebin <= 4 || maxAnglebin >= 31) {
		cout << "Camera movement    :- " << "right" << " - " << maxDistance << endl;
	}
	else  if (maxAnglebin <= 13 && maxAnglebin >= 4) {
		cout << "Camera movement    :- " << "up" << " - " << maxDistance << endl;
	}
	else  if (maxAnglebin <= 22 && maxAnglebin >= 13) {
		cout << "Camera movement    :- " << "left" << " - " << maxDistance << endl;
	}
	else  if (maxAnglebin <= 31 && maxAnglebin >= 22) {
		cout << "Camera movement    :- " << "down" << " - " << maxDistance << endl;
	}
}

float predictDataShot(float arr[720]) {
	// 8 arrays of data.. 
	//float ar[720] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,4,0,12,0,0,0,0,3,0,0,0,2,0,0,8,0,0,0,12,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,4,0,0,0,0,2,0,0,0,1,0,0,0,0,0,0,0,6,6,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,9,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,2,0,0,0,1,0,2,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,6,0,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,12,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,1,0,0,0,0,0,0,0,0,131,346,986,476,688,181,0,0,0,0,0,0,0,0,0,0,397,426,640,0,0,0,0,119,0,94,362,422,97,0,0,0,0,0,94,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,210,0,81,218,139,0,30,0,16,15,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,9,4,0,0,69,128,25,18,0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,0,0,0,6,8,6,0,3,0,103,57,18,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,53,0,0,0,0,0,0,0,8,0,34,52,109,74,0,0,0,0,0,1,0,0,0,5,0,8,4,145,3,4,16,10,23,0,0,0,0,42,0,0,17,0,0,0,0,0,38,0,0,0,0,0,0,0,0 };
	Ptr<SVM> svm = Algorithm::load<SVM>("saveData.xml");
	//	float	 testData[2] = { a, b };
	cv::Mat testDataMat(1, 720, CV_32FC1, arr);
	float response = svm->predict(testDataMat);
	//cout << response << endl;
	return response;

}

// Writing to xml body 
void WirteXmlBody4Service(xml_node<>* root, float time) {
	// converting to string to write in xml
	std::ostringstream Timesave;
	Timesave << time;
	std::string Timesaves = Timesave.str();
	char* savingtime = docMotionService.allocate_string(Timesaves.c_str());

	// Adding nodes to xml
	xml_node<>* child = docMotionService.allocate_node(node_element, "Frame");
	child->append_attribute(docMotionService.allocate_attribute("Time", savingtime));
	root->append_node(child);
}

void WirteXmlBody4Dash(xml_node<>* root, float time) {
	// converting to string to write in xml
	std::ostringstream Timesave;
	Timesave << time;
	std::string Timesaves = Timesave.str();
	char* savingtime = docMotionDash.allocate_string(Timesaves.c_str());

	// Adding nodes to xml
	xml_node<>* child = docMotionDash.allocate_node(node_element, "Frame");
	child->append_attribute(docMotionDash.allocate_attribute("Time", savingtime));
	root->append_node(child);
}

void WirteXmlBody4Idle(xml_node<>* root, float time) {
	// converting to string to write in xml
	std::ostringstream Timesave;
	Timesave << time;
	std::string Timesaves = Timesave.str();
	char* savingtime = docMotionIdle.allocate_string(Timesaves.c_str());

	// Adding nodes to xml
	xml_node<>* child = docMotionIdle.allocate_node(node_element, "Frame");
	child->append_attribute(docMotionIdle.allocate_attribute("Time", savingtime));
	root->append_node(child);
}

/// ------------ Start Optical Flow Method 0 ---------- . /// / / // / // 
void FlowPyrk(String path)
{
	//VideoCapture cap(0); //web cam
	VideoCapture cap(path);
	ofstream log("log.txt");
	cout << cap.get(CV_CAP_PROP_FPS) << endl;

	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
	Size subPixWinSize(10, 10), winSize(31, 31);
	const int MAX_COUNT = 500;
	bool needToInit = false;
	bool nightMode = false;

	//////////////////////////////// - - - - - - - Service - - - - -- /////////////////////////////////////////////////////////////
	// Initializing the XML
	xml_node<>* decl = docMotionService.allocate_node(node_declaration);
	decl->append_attribute(docMotionService.allocate_attribute("version", "1.0"));
	decl->append_attribute(docMotionService.allocate_attribute("encoding", "utf-8"));
	docMotionService.append_node(decl);

	xml_node<>* root = docMotionService.allocate_node(node_element, "rootnode");
	root->append_attribute(docMotionService.allocate_attribute("version", "1.0"));
	root->append_attribute(docMotionService.allocate_attribute("type", "Service-Motion"));
	docMotionService.append_node(root);


	//
	///////////////////// - - - -- - - - - - - Dash - - - - - - - - /////////////////////////////////
	xml_node<>* root2 = docMotionDash.allocate_node(node_element, "rootnode");
	root2->append_attribute(docMotionDash.allocate_attribute("version", "1.0"));
	root2->append_attribute(docMotionDash.allocate_attribute("type", "Dash-Motion"));
	docMotionDash.append_node(root2);

	///////////////////// - - - -- - - - - - - Idle - - - - - - - - /////////////////////////////////
	xml_node<>* root3 = docMotionIdle.allocate_node(node_element, "rootnode");
	root3->append_attribute(docMotionIdle.allocate_attribute("version", "1.0"));
	root3->append_attribute(docMotionIdle.allocate_attribute("type", "Dash-Motion"));
	docMotionIdle.append_node(root3);

	//////////////////////////////////////////////////////////////////////////////////////////////////


	String s = "logs-of-testData.txt";
	ofstream myfile(s);

	if (!cap.isOpened())
	{
		cout << "Could not initialize capturing...\n";
	}

	namedWindow("LK Demo", WINDOW_NORMAL);
	setMouseCallback("LK Demo", onMouse, 0); //Sets mouse handler for the specified window
	createTrackbar(trackbar1, "LK Demo", &threshold1, max_value1);
	createTrackbar(trackbar2forT, "LK Demo", &threshold2forT, max_value2forT);


	Mat gray, prevGray, image, frame1, frame;
	//coordinates of the keypoints
	vector<Point2f> points[2];

	Mat redOnly;
	int frameCount = 0;
	while (true)
	{
		cap >> frame;
		cap >> frame;

		for (size_t i = 0; i < 36; i++)
		{
			angles[i] = 0;
			BinArrayAngle[i] = 0;
		}
		for (size_t i = 0; i < 1080; i++)
		{
			lines[i] = 0;
		}

		if (frame.empty())
			break;


		refresh_count++;
		if (refresh_count > threshold1) {
			needToInit = true;
			refresh_count = 0;
		}

		//  -------------------------------            Detection ROI      --------------------------------
		// Filter Background Area - > PINK AREA
		//	B	G	R
		inRange(frame, Scalar(51, 81, 213), Scalar(211, 203, 255), redOnly);
		GaussianBlur(redOnly, redOnly, Size(5, 5), 3, 4, 4);

		int x1, y1, x2, y2;
		int height = redOnly.rows;
		int width = redOnly.cols;

		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				if ((int)redOnly.at<uchar>(i, j) > 245) {
					y1 = i;
					break;
				}
			}
		}

		for (size_t i = (height - 1); i >0; i--)
		{
			for (size_t j = (width - 1); j > 0; j--)
			{
				if ((int)redOnly.at<uchar>(i, j)> 245) {
					y2 = i;
					break;
				}
			}
		}

		for (size_t j = 0; j < width; j++)
		{
			for (size_t i = 0; i < height; i++)
			{
				if ((int)redOnly.at<uchar>(i, j)> 245) {
					x1 = j;
					break;
				}
			}
		}

		for (size_t j = (width - 1); j >0; j--)
		{
			for (size_t i = 0; i < height; i++)
			{
				if ((int)redOnly.at<uchar>(i, j) > 245) {
					x2 = j;
					break;
				}
			}
		}

		//cout << "x1 , Y1 :" << x1 << "," << y1 << "x2, Y2 :" << x2 << "," << y2 << endl;

		//	Mat oop = Mat( abs(x2-x1), abs(y2 - y1), CV_8UC1);
		//	B	G	R
		//circle(image, Point(x1, y1), 4, Scalar(255, 0, 0), 6, 8, 0);
		//circle(image, Point(x2, y1), 4, Scalar(0, 255, 0), 6, 8, 0);
		//circle(image, Point(x1, y2), 4, Scalar(0, 0, 255), 6, 8, 0);
		//circle(image, Point(x2, y2), 4, Scalar(255, 255, 255), 6, 8, 0);
		//line(image, Point(0, y1), Point(( width-1), y1), Scalar(255, 255, 255), 5);
		//line(image, Point(0, y2), Point((width - 1), y2), Scalar(255, 0, 0), 5);
		//line(image, Point(x1, 0), Point(x1, (height-1)), Scalar(0, 0, 255), 5);
		//line(image, Point(x2, 0), Point(x2, (height - 1)), Scalar(255, 0, 0), 5);

		// ---------------------------    R       O        I      ---------------------------

		frame.copyTo(image);
		cvtColor(image, gray, COLOR_BGR2GRAY);

		if (nightMode)
			image = Scalar::all(0); // all pixel getting black

		if (needToInit)
		{
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			// automatic initialization
			cornerSubPix(gray, points[1], subPixWinSize, Size(-1, -1), termcrit);
			addRemovePt = false;
		}
		else if (!points[0].empty())
		{
			vector<uchar> status;
			vector<float> err;
			if (prevGray.empty())
				gray.copyTo(prevGray);
			//  Calculates an optical flow for a sparse feature set using the iterative Lucas-Kanade method with pyramids
			// prevous points || // Next Points
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);
			size_t i, k;

			for (i = k = 0; i < points[1].size(); i++)
			{
				if (addRemovePt)
				{
					if (norm(point - points[1][i]) <= 5)
					{
						addRemovePt = false;
						continue;
					}
				}
				if (!status[i])
					continue;

				double tmpAngle = calcAngle(points[1][i], points[0][i]);
				int t = (int)abs(tmpAngle / 10);
				if (t < 36) {
					angles[t] = angles[t] + 1; // adding angles to histrogram
				}
				int dis = (int)calcDistance(points[0][i], points[1][i]);
				if (dis < 1080) {
					lines[dis] = lines[dis] + 1;
				}

			}

			int maxAnglebin = findHighestValueBin(angles);
			int maxDistance = findHighestValueBinInDis(lines);

			// ------ camera movement ------- /// 
			DetectDirection( maxAnglebin,  maxDistance);
			// --------- camera movement end ----- .. /// 

			double avgAngle = 0;
			double avgDis = 0;

			for (i = k = 0; i < points[1].size(); i++)
			{
				int ang = calcAngle(points[0][i], points[1][i]);
				int dis = calcDistance(points[0][i], points[1][i]);
				int angt = ang / 10;

				//// visualization  if not filtered
				////circle(image, points[1][i], 2, Scalar(0, 255, 0), -1, 8);
				////arrowedLine(image, points[0][i], points[1][i], Scalar(0, 255, 0), 1, 8, 0);
				

				if (maxAnglebin != angt) {
					if (maxDistance != dis) {
						int Px = points[1][i].x;
						int Py = points[1][i].y;

						if ((y2 <  Py) && (y1  > Py) && (x2 <  Px) && (x1  > Px)) {
							//// visualization
							circle(image, points[1][i], 2, Scalar(0, 255, 0), -1, 8);
							arrowedLine(image, points[0][i], points[1][i], Scalar(0, 255, 0), 1, 8, 0);
							BinArrayAngle[angt] += dis;

						}

					}
				}


				points[1][k++] = points[1][i];
			}


			// --- save to log file ---- for SVM training data 

			if (frameCount < 20) {

				for (size_t i = 0; i < 36; i++)
				{
					//cout << "i:"<<i << " angle: " <<BinArrayAngle[i] << endl;
					CumilativeBin.push_back(BinArrayAngle[i]);
				}

				/*for (size_t i = 0; i < CumilativeBin.size(); i++)
				{
				cout << " " << CumilativeBin.at(i) << ",";
				}
				cout << endl;*/

				frameCount++;
			}
			else {

				int milliseconds = cap.get(CAP_PROP_POS_MSEC);
				int seconds = (int)(milliseconds / 1000) % 60;
				int minutes = (int)((milliseconds / (1000 * 60)) % 60);
				int hours = (int)((milliseconds / (1000 * 60 * 60)) % 24);

				// write file to Cumulative BIn..
				int frms = cap.get(CV_CAP_PROP_POS_FRAMES);
				//cout << "frame:" << frms << "-- Time -> h: " << hours << "-m: " << minutes << "-s: " << seconds << endl;
				int size = CumilativeBin.size();

				float *pArray = new float[size];
				for (int i = 0; i < CumilativeBin.size(); i++)
					pArray[i] = CumilativeBin[i];


				int a = predictDataShot(pArray);
				cout << a << "   :  ";
				switch (a)
				{
				case 1:
					WirteXmlBody4Service(root, milliseconds);
					cout<<"*********************************************    Prediction :-" << "Service" << endl;
					break;
				case 2:
					WirteXmlBody4Dash(root2, milliseconds);
					cout << "*********************************************    Prediction :-" << "Dash" << endl;
					break;
				case -1:
					WirteXmlBody4Idle(root3, milliseconds);
					cout << "*********************************************    Prediction :-" << "Idle" << endl;
					break;
				default:
					break;
				}

				/*if (myfile.is_open())
				{
				myfile << " {";
				for (size_t i = 0; i < CumilativeBin.size(); i++)
				{
				myfile << CumilativeBin.at(i) << ",";
				}
				myfile << "},   \n \n";

				}
				else cout << "Unable to open file";

				if (log.is_open())
				{
				log << "frame:" << frms << "-- Time -> h: " << hours << "-m: " << minutes << "-s: " << seconds<<"\n";
				}
				else cout << "Unable to open file log";*/


				CumilativeBin.clear();
				frameCount = 0;
			}



			/*
			DrawChart(BinArrayAngle, "Bin Angles");
			//	cout << "motion angle :" << avgAngle / points[1].size() << " -- motion distance :" << avgDis / points[1].size() << endl;
			//DetectDirection(avgAngle/ points[1].size(), avgDis/ points[1].size());
			*/

			points[1].resize(k);

		}

		//// visualization
		//DrawChart(angles, "angles");
		//DrawChart(lines, "dis");

		rectangle(image, Point(x1 + threshold2forT, y1 + threshold2forT), Point(x2 -  threshold2forT, y2 - 3 * threshold2forT), Scalar(255, 255, 255), 5);

		if (addRemovePt && points[1].size() < (size_t)MAX_COUNT)
		{
			vector<Point2f> tmp;
			tmp.push_back(point);
			cornerSubPix(gray, tmp, winSize, Size(-1, -1), termcrit);
			points[1].push_back(tmp[0]);
			addRemovePt = false;
		}

		needToInit = false;
		//imshow("back - subs", frame1);
		imshow("LK Demo", image);
		imshow("play ground Area", redOnly);
		char c = (char)waitKey(10);
		if (c == 27)
			break;
		switch (c)
		{
			needToInit = true;
		case 'r':
			break;
		case 'c':
			points[0].clear();
			points[1].clear();
			break;
		case 'n':
			nightMode = !nightMode;
			break;
		}

		std::swap(points[1], points[0]);
		cv::swap(prevGray, gray);
	}


	//////////////////////////////////////// - - -- LOGO - - - - - ///////////////////////////////////////////////////
	//Finishing the XML
	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), docMotionService);
	// Save to the XML file
	std::ofstream file_stored("C://Users//Prabudda//Documents//Visual Studio 2015//GPro//GPro//Motion-Service.xml");
	file_stored << docMotionService;
	file_stored.close();
	docMotionService.clear();

	//Finishing the XML
	std::string xml_as_string1;
	rapidxml::print(std::back_inserter(xml_as_string1), docMotionDash);
	// Save to the XML file
	std::ofstream file_stored1("C://Users//Prabudda//Documents//Visual Studio 2015//GPro//GPro//Motion-Dash.xml");
	file_stored1 << docMotionDash;
	file_stored1.close();
	docMotionDash.clear();

	//Finishing the XML
	std::string xml_as_string2;
	rapidxml::print(std::back_inserter(xml_as_string2), docMotionIdle);
	// Save to the XML file
	std::ofstream file_stored2("C://Users//Prabudda//Documents//Visual Studio 2015//GPro//GPro//Motion-Idle.xml");
	file_stored2 << docMotionIdle;
	file_stored2.close();
	docMotionIdle.clear();


	//	myfile.close();
	log.close();

}

