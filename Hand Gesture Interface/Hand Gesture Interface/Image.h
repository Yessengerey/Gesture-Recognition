#pragma once

#include <opencv2\opencv.hpp>

#define FONT_FACE FONT_HERSHEY_COMPLEX

using namespace cv;

class Image {
public:
	Image();			//default constructor
	Image(int camera);	//defined constructor (initalizes video capture)
	
	//An arbitrary function that prints out a given text, with a given colour, onto a given matrix
	void printText(Mat destination, std::string text, Scalar colour);	


	//Function that reads capture into a matrix, then blurs and downsamples it
	void formatImage();

	std::string int2String(int n);

	Mat sourceMat;		//Main matrix
	Mat sourceCopy;		//Matrix to which the main is copied to; extra features are applied onto the copy
	Mat threshold;		//Threshold image (black and white)

	std::vector<Mat> threshList;	//Vector to store thresholds of each possible HSL combination

	VideoCapture cap;	//Main video capture object

	int camSource;		//The number indicated which camera is in use (0 is default)
};