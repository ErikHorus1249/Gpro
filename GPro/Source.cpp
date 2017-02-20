#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include "header.h"
#include <vector>
#include <string>


using namespace cv;
using namespace std;

 string filen = "ui.xml";

int main(int ac, char** av)
{
	
	
	//mainly();
	//ProcessImage();
	//runProgram();
	//runImage();
	ProcessImage();

	//runProgramX();
	//mainText();


	system("pause");
	return 0;
}

//void FileReaderXMLMsd() {
//	//read
//	cout << endl << "Reading: " << endl;
//	FileStorage fs;
//	fs.open(filename, FileStorage::READ);
//
//	FileNode n = fs["MSD"];  
//	// Read mappings from a sequence
//	
//	cout << "frameID  " << (int)(n["frameID"]) << "; ";
//	cout << "time   " << (string)(n["time"]) << endl;
//	cout << "MSD Value  " << (double)n["msd-value"] << endl;
//
//}
//
//static void FileWriteXMLMsd(int frameID, string time, double msdvalue) {
//	//write
//
//	FileStorage fs(filename, FileStorage::WRITE);
//	fs << "MSD";                              // text - mapping
//	fs << "{" << "frameID" << frameID;
//	fs << "time" << time;
//	fs << "msd-value" << msdvalue << "}";
//	fs.release();                                       // explicit close
//
//
//}

/*
//...
vector<char> array;

char c = 0;
while (c != 'x') {
std::cin >> c;
array.push_back(c);
}

for (char a : array) {
cout << a << endl;
}
int i = 999;          // some integer value
array.reserve(10);    // make room for 10 elements
array.push_back(i);
std::cout << array.capacity() << std::endl;
std::cout << array.size() << std::endl;



Mat histImage(200, 200, CV_8UC1, Scalar(0, 0, 0));

//line(histImage, point, point, Scalar, Thickness, lineType, shift);

for (size_t i = 0; i < 200; i++)
{
line(histImage, Point(i, 200), Point(i, 200-i*i), Scalar(255, 0, 0), 2, 8, 0);
}



imshow("chart", histImage);
waitKey(0);
system("pause");

*/
// no need to delete anything

