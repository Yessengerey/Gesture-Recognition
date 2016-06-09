#pragma once
#include "TimerPerf.h"
#include <vector>
#include <opencv2\opencv.hpp>
#include "Image.h"
#include "Trackbars.h"

//using namespace cv;

class ROI {
public:

	ROI();
	ROI(Image* m, std::string windowName);
	ROI(Point lower_p, Point upper_p, Mat);

	void displayROI(Image*, std::string);

	void findAverageColour(Image *m, ROI r, int average[3]);

	void recordValues(Image* img, std::string windowName);

	void generateRectangle(Mat);

	int findMedian(std::vector<int>);

	void determineThreshold(Image *img, Trackbars *t);

	void normalizeCol(Trackbars *t);

	std::vector<ROI> vecROI;

	int lengthOfRoi = 20;
	int thickness;
	int height;
	int width;

	int averageCol[7][3]; 

	Scalar lower_boundary;
	Scalar upper_boundary;

	Scalar colour;

	Point bot_corner;
	Point top_corner;

	Image accessImg;

	Mat pointer;

	GameTimer gm;
};