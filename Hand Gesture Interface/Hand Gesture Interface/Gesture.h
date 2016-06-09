#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include "Image.h"

//using namespace cv;

//Class that defines the gand gestures themselves and everything
//that will be done with them.
class Gesture {
public:
	Gesture(); //Class constructor

	void determineContours(Image *img);

	void applyMorphs(Image *img);

	int findBiggestCon(std::vector<std::vector<Point>> contours);

	void eliminateConvexDefects(Image *img);

	float distanceBetweenPoints(Point x, Point y);

	void showHandGestInfo(Mat m);

	void findFingerTips(Image *img);

	void oneFingerCheck(Image *img);

	void drawFingerTips(Image* img);

	void displayContours(Image *img);

	void determineFingerNumbers(Image *img);

	void calcFingerNumber();

	void displayFingerNumber(Image *img);

	void removeCloseTips();

	void removeEndPoints(Image *img, std::vector<Vec4i> new_defects);

	std::string intToString(int number_to_display);

	std::vector<std::vector<Point>> contours;		//vector to store the contour points found using CV function findContours
	std::vector<std::vector<int>> hull_indices;		//Vector storing the indices of a contour
	std::vector<std::vector<Point>> hull_points;	//Vector to store coordinates of a contour
	std::vector<std::vector<Vec4i>> hull_defects;	//Vector to store the defect convexity points of a contour
	std::vector<Point> finger_tips;					//Vector to store the coordinates of finger tips
	std::vector<int> finger_numbers;				
	std::vector<int> finger_numbers_to_display;

	int nFrames;	//variable to store number of frames
	int nFingers;	//variable to track how many fingers are up
	int biggestContourIndex;
	int mostFrequentFN;
	int n_defects;

	Scalar col_of_numbers = Scalar(0, 0, 255);
	Mat threshCopy;
	Rect boundingBox;

	Image access;

};