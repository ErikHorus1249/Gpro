#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_utils.hpp"


using namespace std;
using namespace cv;
using namespace rapidxml;

int mainText();
void ProcessImage();
void runProgram();

void runProgramX(String);
void WirteXmlBodyinLogo(xml_node<>*, float);
void WirteXmlBodyinCut(xml_node<>*, float);

void runImage();
int mainly();
void GoProcess();
void CannyThresholdw(int, void*);
void backGroundSubs();
void Threshoder(Mat);

void processVideo(char* videoFilename);
void processImages(char* firstFrameFilename);


//FlowPyrk method
void FlowPyrk(String);
//double calcAngle(Point, Point);

void train();
void predictData();