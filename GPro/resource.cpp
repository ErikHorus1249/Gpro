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

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"


using namespace std;
using namespace cv;
using namespace rapidxml;

//Initializing Global Variable for xml ->  Logo Transition
xml_document<> docLogo;
//Initializing Global Variable for xml ->  Cut Transition
xml_document<> docCut;

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";



int threshold_type = 1;

////   -------    640 X 360
///// -> LOGO TRANSITION PARAMETERS
char* trackbar_value_msd = "T for logo";
int threshold_value_msd = 26;
int const max_value_msd = 60;

 //////- > CUT TRANSITION PARAMETERS
int const max_value = 100;
int const max_type = 4;
int threshold_value = 16;
char* trackbar_value = "T for cut";


////// ---------   854 x 480
//// -> LOGO TRANSITION PARAMETERS
//char* trackbar_value_msd = "T for logo";
//int threshold_value_msd = 26;
//int const max_value_msd = 60;
//
//
//// - > CUT TRANSITION PARAMETERS
//int const max_value = 100;
//int const max_type = 4;
//int threshold_value = 13;
//char* trackbar_value = "T for cut";

//// ---------   720p
////// -> LOGO TRANSITION PARAMETERS
////char* trackbar_value_msd = "T for logo";
////int threshold_value_msd = 23;
////int const max_value_msd = 60;
////
////
////// - > CUT TRANSITION PARAMETERS
////int const max_value = 100;
////int const max_type = 4;
////int threshold_value = 13;
////char* trackbar_value = "T for cut";

// ////    ---------    1920 x 1080
//// -> LOGO TRANSITION PARAMETERS
//char* trackbar_value_msd = "T for logo";
//int threshold_value_msd = 37;
//int const max_value_msd = 60;
//
//// - > CUT TRANSITION PARAMETERS
//int const max_value = 100;
//int const max_type = 4;
//int threshold_value = 20;
//char* trackbar_value = "T for cut";



int const max_BINARY_value = 255;
static string filename = "xmlFile.xml";
bool DebugMode = false;



//arrays
double** imd; // store MSD array..
// vectors
vector<double> msdVector;


//tempory for right gradient
vector<double> rightGradientVector;
// Methods
void DrawHist11(int, int, Mat, int);
void DrawChart(vector<double>,String);

void WirteXmlBodyinLogo(xml_node<>*, float);
void WirteXmlBodyinCut(xml_node<>* , float );

int SearchMax(vector<double>);
double tmpMsd=0;

// Boundary Detection Algorithm.....
void runProgramX(String path) {
	
	VideoCapture cap(path);
	//VideoCapture cap("E:/opencvPracticles/sample-1- France vs Brazil- 360p.mp4"); // 640 X 360
	//VideoCapture cap("E:/opencvPracticles/sample-3-USA vs Russia - World League .mp4"); // for 854 x 480
	//VideoCapture cap("E:/opencvPracticles/sample-2-Brazil vs USA-1080p.mp4"); // for 1920x1080
	/*
	VideoCapture cap("E:/opencvPracticles/720pUSAITALYFINAL.mp4");
	VideoCapture cap("E:/opencvPracticles/sample 1.mp4.mp4");
	VideoCapture cap("E:/opencvPracticles/sample 1.mp4.mp4");*/

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "ERROR: Cannot open the video file" << endl;
	}

	//////////////////////////////// - - - - - - - LOGO - - - - -- /////////////////////////////////////////////////////////////
	// Initializing the XML
	xml_node<>* decl = docLogo.allocate_node(node_declaration);
	decl->append_attribute(docLogo.allocate_attribute("version", "1.0"));
	decl->append_attribute(docLogo.allocate_attribute("encoding", "utf-8"));
	docLogo.append_node(decl);

	xml_node<>* root = docLogo.allocate_node(node_element, "rootnode");
	root->append_attribute(docLogo.allocate_attribute("version", "1.0"));
	root->append_attribute(docLogo.allocate_attribute("type", "Logo-transition"));
	docLogo.append_node(root);


	
	/////////////////// - - - -- - - - - - - CUT - - - - - - - - /////////////////////////////////
	xml_node<>* root2 = docCut.allocate_node(node_element, "rootnode");
	root2->append_attribute(docCut.allocate_attribute("version", "1.0"));
	root2->append_attribute(docCut.allocate_attribute("type", "Cut-transition"));
	docCut.append_node(root2);

	//////////////////////////////////////////////////////////////////////////////////////////////////


	namedWindow("MainVids", CV_WINDOW_NORMAL); //create a window called "MyVideo"

	createTrackbar(trackbar_value, "MainVids", &threshold_value, max_value);
	createTrackbar(trackbar_value_msd, "MainVids", &threshold_value_msd, max_value_msd);

	Mat frameColor;
	Mat frameGray1;
	Mat frameGray2;
	Mat diffFrame;

	bool bSuccess1 = cap.read(frameColor); // read a new frame from video
	if (!bSuccess1) //if not success, break loop
	{
		cout << "ERROR: Cannot read a frame from video file" << endl;
		
	}

	//cap.set(CV_CAP_PROP_POS_FRAMES, 15000);
	int height = frameColor.rows;
	int width = frameColor.cols;

	

	//imageDiffArray
	imd = new double*[height ];
	for (int i = 0; i < height ; ++i)
		imd[i] = new double[width];

	double maxValueAtLogoTransition=0;

	bool startTracking = false;

	int lastLogoFrameId = 0;
	//  * * * * * * * * * * * * * * * L O O P * * * S T A R T * * * * * * * * * * * * * * * * * * * * * * * * //
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
			int timeUtillXML = (int) milliseconds / 1000;

			int seconds = (int)(milliseconds / 1000) % 60;
			int minutes = (int)((milliseconds / (1000 * 60)) % 60);
			int hours = (int)((milliseconds / (1000 * 60 * 60)) % 24);
			
	//	}
		//cout <<"MSD:  "<< MSD << endl;
		msdVector.push_back(MSD);
		DrawChart(msdVector,"1");

		// -------------------------    CUT DETECTION ALGORITHM  ------------------- //
		int sizeAr = msdVector.size();
		// for cut detection technic
		if (sizeAr > 3) {

			int rightGradient = abs((int)msdVector.at(sizeAr - 3) - (int)msdVector.at(sizeAr - 2));
			int leftGradient = abs((int)msdVector.at(sizeAr - 2) - (int)msdVector.at(sizeAr - 1));
			//cout << "rightGradient : " << rightGradient << "   ---   leftGradient : " << leftGradient << endl;

			/*      ---->   1920x1080
			rightGradientVector.push_back(rightGradient);
			DrawChart(rightGradientVector, "rightGradientVector");
			int size = rightGradientVector.size();
			if (size > 4) {
				int deltaL = abs((int)rightGradientVector.at(size - 3) - (int)rightGradientVector.at(size - 2));
				int deltaR = abs((int)rightGradientVector.at(size - 2) - (int)rightGradientVector.at(size - 1));
				if (((int)rightGradientVector.at(size - 3) <(int)rightGradientVector.at(size - 2)) && ((int)rightGradientVector.at(size - 2) > (int)rightGradientVector.at(size - 1))) {
					if (deltaL > threshold_value && deltaR > threshold_value) {
						cout << "@@@  CUT DETECTED " << endl;
						WirteXmlBodyinCut(root2, milliseconds);
					}
				}
			}*/
	
			if ((rightGradient > threshold_value) && ( leftGradient  > threshold_value)) {
				cout << "@@@@@@@@@  CUT DETECTED - " << frameNo << " ||   At :: " << hours << ":" << minutes << ":" << seconds << endl;
				WirteXmlBodyinCut(root2, timeUtillXML);
				
				
			}else if (( rightGradient > threshold_value ) && ( leftGradient > ( threshold_value/4) )) {
				cout << "@@@@@@@@@  CUT DETECTED - " << frameNo << " ||   At :: " << hours << ":" << minutes << ":" << seconds << endl;
				WirteXmlBodyinCut(root2, timeUtillXML);
				
			}

		}
			// ........      LOGO TRANSION ALGORITHM     ....... // 
			double avgMSD = 0;
			avgMSD = tmpMsd / msdVector.size();
			int vecSize = msdVector.size();
			//cout << "avgMSD :" << avgMSD << "        MSD :      " << MSD << "   |>     diff :       "<< abs(avgMSD-MSD) << endl;
			cout <<"time " << hours << ":" << minutes << ":" << seconds << endl;
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
							//cout << "maxPosition     " << maxPosition <<" | maxValueAtLogoTransition :" << maxValueAtLogoTransition << "  (double)*biggest :"<< (double)*biggest <<endl;
							
							
							if (    maxValueAtLogoTransition != 0 ) {
								if (maxValueAtLogoTransition != (double)*biggest) {
									if (maxPosition < 14 && maxPosition>0) {
										int rightGradientz = abs((int)tmp.at(maxPosition) - (int)tmp.at(maxPosition - 1));
										int leftGradientz  = abs((int)tmp.at(maxPosition + 1) - (int)tmp.at(maxPosition));
										// << "rightGradient : " << rightGradientz << "   ---   leftGradient : " << leftGradientz << endl;
										
													// 51								//52                  -> not filterd
													//20					// 24        //14                    //24   -> correct
													//29														//19
													// 7                                 // 12                          -> incorrect
										if (!( rightGradientz > threshold_value_msd && leftGradientz > threshold_value_msd )) {
											cout << "rightGradient : " << rightGradientz << "   --- XXXXX   leftGradient : " << leftGradientz << endl;
																				
											WirteXmlBodyinLogo(root, timeUtillXML);
											cout << "***********------ LOGO TRANSION - " << frameNo << " ||   At :: " << hours << ":" << minutes << ":" << seconds <<" MSD:"<<MSD<< endl;
											
											DrawChart(tmp, "2");
											rectangle(
												frameColor,
												Point(0, 0),
												Point(width, height),
												Scalar(255, 255, 255), 50
												);
											
											if ((frameNo - lastLogoFrameId) > 10) {
												startTracking = !startTracking;
											}

											lastLogoFrameId = frameNo; 
										}
									}
								}
							}
							maxValueAtLogoTransition = (double)*biggest;						
					}
					tmp.clear();
		}

			// save to text
			/*if (startTracking) {
				WirteXmlBodyinLogo(root, timeUtillXML);
				cout << "@ LOGO TRANSION - " << frameNo << " ||   At :: " << hours << ":" << minutes << ":" << seconds << endl;
			}*/

			/*if (DebugMode) {
				cout << "# Mean Square Deviation - :" << MSD << " | avg MSD: " << avgMSD << " ||   At :: " << hours << ":" << minutes << ":" << seconds << endl;
			}*/
			//myfile << "Frame ID " << frameNo << " | msd :" << MSD << " " << " | avg MSD: " << avgMSD << " ||   At :: " << hours << ":" << minutes << ":" << seconds << " \n";
			
		imshow("MainVids", frameColor); //show the frame in "MyVideo" window
	
		if (waitKey(50) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	/////////////////////// ending while Loop ///////////////////////////////////////


	try {
		//////////////////////////////////////// - - -- LOGO - - - - - ///////////////////////////////////////////////////
		//Finishing the XML
		std::string xml_as_string;
		rapidxml::print(std::back_inserter(xml_as_string), docLogo);
		// Save to the XML file
		std::ofstream file_stored("C://Users//Prabudda//Documents//Visual Studio 2015//GPro//GPro//logotransition.xml");
		//std::ofstream file_stored("F:/DELLLAP/doc/FinalYearProject/RunningScripts/Scripts/LogoDetection.xml");
		//F:\DELLLAP\doc\FinalYearProject\RunningScripts\Scripts\LogoDetection.xml
		file_stored << docLogo;
		file_stored.close();
		docLogo.clear();

		//////////////////////////////////////- - - - - CUT - - - - - - -/////////////////////////////////////////////////////

		//Finishing the XML
		std::string xml_as_string_1;
		rapidxml::print(std::back_inserter(xml_as_string_1), docCut);
		// Save to the XML file
		std::ofstream file_stored1("C://Users//Prabudda//Documents//Visual Studio 2015//GPro//GPro//cuttransition.xml");
		//std::ofstream file_stored1("F:/DELLLAP/doc/FinalYearProject/RunningScripts/Scripts/CutDetection.xml");
		file_stored1 << docCut;
		file_stored1.close();
		docCut.clear();
		///////////////////////////////////////////////////////////////////////////////////////////
	}
	catch (Exception e) {
		cout <<" Error Occur "<< endl;
		cout << e.msg<< endl;
	}

	exit(0);


}

// Writing to xml body 
void WirteXmlBodyinLogo(xml_node<>* root, float time) {
	// converting to string to write in xml
	std::ostringstream Timesave;
	Timesave << time;
	std::string Timesaves = Timesave.str();
	char* savingtime = docLogo.allocate_string(Timesaves.c_str());

	// Adding nodes to xml
	xml_node<>* child = docLogo.allocate_node(node_element, "Frame");
	child->append_attribute(docLogo.allocate_attribute("Time", savingtime));
	root->append_node(child);
}

// Writing to xml body
void WirteXmlBodyinCut(xml_node<>* root, float time) {
	// converting to string to write in xml
	std::ostringstream Timesave;
	Timesave << time;
	std::string Timesaves = Timesave.str();
	char* savingtime = docCut.allocate_string(Timesaves.c_str());

	// Adding nodes to xml
	xml_node<>* child = docCut.allocate_node(node_element, "Frame");
	child->append_attribute(docCut.allocate_attribute("Time", savingtime));
	root->append_node(child);
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
	
	imshow("HistGram--MSD "+a, histImage);

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
	

	string h = "MeanSquare Diviation Histro" + count;
	namedWindow(h, 1);
	imshow(h, histImage);

}
