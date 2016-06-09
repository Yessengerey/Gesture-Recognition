#include "Gesture.h"

Gesture::Gesture()
{
	nFrames = 0;
	nFingers = 0;
}

//Erode the image to remove the background noise
//Dilate the threshold with larger structuring elements to ensure that the hand object is nicely visible
void Gesture::applyMorphs(Image *img) {

	pyrUp(img->threshold, img->threshold);

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));

	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(img->threshold, img->threshold, erodeElement);
	erode(img->threshold, img->threshold, erodeElement);

	dilate(img->threshold, img->threshold, dilateElement);
	dilate(img->threshold, img->threshold, dilateElement);

	img->threshold.copyTo(threshCopy);
}

//find the biggest contour out of all
//Unless there is a lot of background noise or interference, 
//the biggest contour should be of the hand
int Gesture::findBiggestCon(std::vector<std::vector<Point>> contours) {

	int biggest_con_idx = -1; //initialize index to -1, in case things go wrong and no biggest contour is found
	int biggest_con_size = 0;

	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > biggest_con_size) {
			biggest_con_size = contours[i].size();
			biggest_con_idx = i;
		}
	}

	return biggest_con_idx;
}

float Gesture::distanceBetweenPoints(Point a, Point b) {
	return sqrt(fabs(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

void Gesture::eliminateConvexDefects(Image *img) {
	int minDist = boundingBox.height / 5;

	//Acceptable maximum angle between two tip points
	//float maxAngle = 95;

	//Store all uneliminated defect points in this vector
	std::vector<Vec4i> new_defects;

	int start_index;
	int end_index;
	int far_index;

	//Iterator to go through the elements of the defects vector
	std::vector<Vec4i>::iterator it = this->hull_defects[this->biggestContourIndex].begin();
	while (it != this->hull_defects[this->biggestContourIndex].end()) {
		Vec4i& x = (*it);

		start_index = x[0];
		Point start_point(this->contours[this->biggestContourIndex][start_index]);

		end_index = x[1];
		Point end_point(this->contours[this->biggestContourIndex][end_index]);

		far_index = x[2];
		Point far_point(this->contours[this->biggestContourIndex][far_index]);

		if (distanceBetweenPoints(start_point, far_point) > minDist
			&& distanceBetweenPoints(end_point, far_point) > minDist)
		{

			if ((end_point.y > (boundingBox.y + boundingBox.height - (boundingBox.height / 4)))
				|| start_point.y > (boundingBox.y + boundingBox.height - (boundingBox.height / 4))) {

			}
			else {
				new_defects.push_back(x);
			}
		}
		it++;
	}
	n_defects = new_defects.size();
	this->hull_defects[this->biggestContourIndex].swap(new_defects);
	removeEndPoints(img, hull_defects[biggestContourIndex]);
}

std::string Gesture::intToString(int number_to_display) {
	std::stringstream ss;
	ss << number_to_display;
	std::string display = ss.str();
	return display;
}

void Gesture::showHandGestInfo(Mat m) {
	Image acc;
	Scalar text_colour(255, 255, 255);

	int x = m.cols / 8;
	int y = m.rows / 1.1;

	float font_size = 0.6;

	int next_line = 20;

	std::string gen_info = "Hand Gesture Information:";
	putText(m, gen_info, Point(x, y), FONT_FACE, font_size, text_colour);
	y += next_line;
	
	std::string info = std::string("Number of defects: ") + std::string(intToString(n_defects));
	putText(m, info, Point(x, y), FONT_FACE, font_size, text_colour);
	y += next_line;

	gen_info = std::string("Bounding box height, width: ") + std::string(acc.int2String(boundingBox.height)) + std::string(" , ") + std::string(acc.int2String(boundingBox.width));
	putText(m, gen_info, Point(x, y), FONT_FACE, font_size, text_colour);

}

void Gesture::oneFingerCheck(Image *img) {
	int minHeight = boundingBox.height / 6;

	Point highest_point;

	highest_point.y = img->sourceMat.rows;

	std::vector<Point>::iterator it = contours[this->biggestContourIndex].begin();

	while (it != contours[this->biggestContourIndex].end()) {
		Point x = (*it);

		if (x.y < highest_point.y) {
			highest_point = x;
		}
		it++;
	}
	int n = 0;

	it = hull_points[this->biggestContourIndex].begin();
	while (it != hull_points[this->biggestContourIndex].end()) {
		Point v = (*it);

		if (v.y < highest_point.y + minHeight
			&& v.y != highest_point.y
			&& v.x != highest_point.x) {
			n++;
		}
		it++;
	}
	if (n == 0) {
		finger_tips.push_back(highest_point);
	}
}

void Gesture::findFingerTips(Image *img) {
	finger_tips.clear();

	bool first = true;

	std::vector<Vec4i>::iterator it = hull_defects[this->biggestContourIndex].begin();

	while (it != hull_defects[this->biggestContourIndex].end()) {
		Vec4i& x = (*it);

		int start_index = x[0];
		Point start_point(this->contours[this->biggestContourIndex][start_index]);

		int end_index = x[1];
		Point end_point(this->contours[this->biggestContourIndex][end_index]);

		int far_index = x[2];
		Point far_point(this->contours[this->biggestContourIndex][far_index]);

		if (first == true) {
			finger_tips.push_back(start_point);
			first = false;
		}

		finger_tips.push_back(end_point);
		it++;
		first = false;

		if (finger_tips.size() == 0) {
			oneFingerCheck(img);
		}
	}
}

void Gesture::drawFingerTips(Image *img) {
	Point p;
	removeCloseTips();
	for (int i = 0; i < finger_tips.size(); ++i) {
		p = finger_tips[i];

		putText(img->sourceMat, img->int2String(i), p - Point(0, 30), FONT_FACE, 1.2f, Scalar(200, 200, 200), 2);
		circle(img->sourceMat, p, 5, Scalar(0, 255, 0), 4);
	}
}

void Gesture::displayContours(Image *img) {
	drawContours(img->sourceMat, this->hull_points, this->biggestContourIndex, cv::Scalar(200, 0, 0), 2, 8, std::vector<Vec4i>(), 0, Point());

	rectangle(img->sourceMat, this->boundingBox.tl(), this->boundingBox.br(), cv::Scalar(0, 0, 200));

	std::vector<Vec4i>::iterator d = this->hull_defects[this->biggestContourIndex].begin();
	int fontFace = FONT_HERSHEY_PLAIN;


	while (d != this->hull_defects[this->biggestContourIndex].end()) {
		Vec4i& v = (*d);
		int startidx = v[0]; Point ptStart(this->contours[this->biggestContourIndex][startidx]);
		int endidx = v[1]; Point ptEnd(this->contours[this->biggestContourIndex][endidx]);
		int faridx = v[2]; Point ptFar(this->contours[this->biggestContourIndex][faridx]);
		float depth = v[3] / 256;

		line(img->sourceMat, ptStart, ptFar, Scalar(0, 255, 0), 1);
		line(img->sourceMat, ptEnd, ptFar, Scalar(0, 255, 0), 1);
		circle(img->sourceMat, ptFar, 4, Scalar(0, 255, 0), 2);
		circle(img->sourceMat, ptEnd, 4, Scalar(0, 0, 255), 2);
		circle(img->sourceMat, ptStart, 4, Scalar(255, 0, 0), 2);

		//circle(result, ptFar, 9, Scalar(0, 205, 0), 5);


		d++;

	}


}

void Gesture::determineContours(Image *img) {
	cvtColor(img->sourceCopy, img->sourceCopy, CV_HLS2BGR);
	//cvtColor(img->sourceCopy, img->sourceCopy, CV_HSV2BGR);


	//Apply structuring elements to make the thresholded image more precise
	applyMorphs(img);

	//Finds contour points from the threshold and saves them in the contours vector of points
	findContours(threshCopy, this->contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//Initalize all vetors
	hull_indices = std::vector<std::vector<int>>(this->contours.size());
	hull_points = std::vector<std::vector<Point>>(this->contours.size());
	hull_defects = std::vector<std::vector<Vec4i>>(this->contours.size());

	//Find the index of the biggest contour from the vector of contours
	this->biggestContourIndex = findBiggestCon(this->contours);

	//If the index is found proceed
	if (this->biggestContourIndex != -1) {
		//Create a bounding box (rectangle) around the biggest contour
		this->boundingBox = boundingRect(Mat(this->contours[this->biggestContourIndex]));

		//Find all convex points in the biggest contour set of points and save them to Hull_points
		convexHull(Mat(this->contours[this->biggestContourIndex]), this->hull_points[this->biggestContourIndex], false, true);

		//Find all indeces of the convex points within the contours vector
		convexHull(Mat(this->contours[this->biggestContourIndex]), this->hull_indices[this->biggestContourIndex], false, false);

		//Find the approxymate polygon cure around the biggest contour with a given precision of 15
		approxPolyDP(Mat(this->hull_points[this->biggestContourIndex]), this->hull_points[this->biggestContourIndex], 15, false);

		//then eliminate convex defect points
		if (this->contours[this->biggestContourIndex].size() > 3) {
			convexityDefects(this->contours[this->biggestContourIndex], this->hull_indices[this->biggestContourIndex], this->hull_defects[this->biggestContourIndex]);
			eliminateConvexDefects(img);
		}

		showHandGestInfo(img->sourceMat);

		findFingerTips(img);
		drawFingerTips(img);
		displayContours(img);
	}
}

void Gesture::removeCloseTips() {
	std::vector<Point> new_tips;

	for (int i = 0; i < finger_tips.size(); i++) {
		for (int j = 0; j < finger_tips.size(); j++) {
			if (distanceBetweenPoints(finger_tips[i], finger_tips[j]) < 20 && i != j) {

			}
			else if(distanceBetweenPoints(finger_tips[i], finger_tips[j]) > 20) {
				new_tips.push_back(finger_tips[i]);
				break;
			}
		}
	}
	finger_tips.swap(new_tips);
}

void Gesture::removeEndPoints(Image *img, std::vector<Vec4i> new_defects) {
	float minWidth = this->boundingBox.width / 6;

	int start_index, start_index2, end_index, end_index2;

	for (int i = 0; i < new_defects.size(); ++i) {
		for (int j = i; j < new_defects.size(); ++j) {
			start_index = new_defects[i][0];
			Point start_point(contours[biggestContourIndex][start_index]);

			end_index = new_defects[i][1];
			Point end_point(contours[biggestContourIndex][end_index]);

			start_index2 = new_defects[j][0];
			Point start_point2(contours[biggestContourIndex][start_index2]);

			end_index2 = new_defects[j][1];
			Point end_point2(contours[biggestContourIndex][end_index2]);

			if (distanceBetweenPoints(start_point, end_point2) < minWidth) {
				contours[biggestContourIndex][start_index] = end_point2;
				break;
			}
			if (distanceBetweenPoints(end_point, start_point2) < minWidth) {
				contours[biggestContourIndex][start_index2] = end_point;
			}

		}
	}
}

void Gesture::displayFingerNumber(Image *img) {
	int x_position = 10;
	int y_position = 10;

	int height = 30;

	float font_size = 1.0f;

	for (int j = 0; j < finger_numbers_to_display.size(); j++) {
		if (finger_numbers_to_display[j] == 1) {
		//if(n_defects == 1){
			rectangle(img->sourceMat, Point(x_position, y_position), Point(x_position + height, y_position + height), col_of_numbers, 2);

			putText(img->sourceMat, "0", Point(x_position + 7, y_position + height - 3), FONT_FACE, font_size, col_of_numbers);
		}

		if (finger_numbers_to_display[j] == 2) {
		//if(n_defects == 2){
			rectangle(img->sourceMat, Point(x_position, y_position), Point(x_position + height, y_position + height), col_of_numbers, 2);

			putText(img->sourceMat, "I", Point(x_position + 7, y_position + height - 3), FONT_FACE, font_size, col_of_numbers);
		}

		if (finger_numbers_to_display[j] == 3) {
		//if(n_defects == 3){
			rectangle(img->sourceMat, Point(x_position, y_position), Point(x_position + height, y_position + height), col_of_numbers, 2);

			putText(img->sourceMat, "L", Point(x_position + 7, y_position + height - 3), FONT_FACE, font_size, col_of_numbers);

		}	

		if (finger_numbers_to_display[j] == 4) {
		//if(n_defects == 4){
			rectangle(img->sourceMat, Point(x_position, y_position), Point(x_position + height, y_position + height), col_of_numbers, 2);
			putText(img->sourceMat, "W", Point(x_position + 7, y_position + height - 3), FONT_FACE, font_size, col_of_numbers);

		}
		if (finger_numbers_to_display[j] == 5) {
			//if(n_defects == 4){
			rectangle(img->sourceMat, Point(x_position, y_position), Point(x_position + height, y_position + height), col_of_numbers, 2);
			putText(img->sourceMat, "4", Point(x_position + 7, y_position + height - 3), FONT_FACE, font_size, col_of_numbers);

		}
		if (finger_numbers_to_display[j] == 6) {
			//if(n_defects == 4){
			rectangle(img->sourceMat, Point(x_position, y_position), Point(x_position + height, y_position + height), col_of_numbers, 2);
			putText(img->sourceMat, "5", Point(x_position + 7, y_position + height - 3), FONT_FACE, font_size, col_of_numbers);

		}
		if (finger_numbers_to_display[j] != 1 && finger_numbers_to_display[j] != 2 && finger_numbers_to_display[j] != 3 && finger_numbers_to_display[j] != 4 && finger_numbers_to_display[j] != 5 && finger_numbers_to_display[j] != 6) {
			rectangle(img->sourceMat, Point(x_position, y_position), Point(x_position + height, y_position + height), col_of_numbers, 2);
			putText(img->sourceMat, img->int2String(finger_numbers_to_display[j]), Point(x_position + 7, y_position + height - 3), FONT_FACE, font_size, col_of_numbers);

		}

		x_position += 40;
		if (x_position > (img->sourceMat.cols - img->sourceMat.cols / 3.2)) {
			y_position += 40;
			x_position = 10;
		}
	}
}

void Gesture::determineFingerNumbers(Image *img) {
	removeCloseTips();
	if (this->boundingBox.height > img->sourceMat.rows / 2 && nFingers > 12) {
		finger_numbers.push_back(finger_tips.size()); //how manye fingers are extended?
		if (nFrames > 12) {
			nFingers = 0;
			nFrames = 0;
			calcFingerNumber();
			finger_numbers_to_display.push_back(mostFrequentFN);
			finger_numbers.clear();
		}
		else nFrames++;
	}
	else {
		nFingers++;
	}
	displayFingerNumber(img);
}

void Gesture::calcFingerNumber() {
	std::sort(finger_numbers.begin(), finger_numbers.end());

	int mostFreqNum;
	int thisNFrequency = 1;
	int highestFreq = 1;

	mostFreqNum = finger_numbers[0];

	for (int i = 1; i < finger_numbers.size(); ++i) {
		if (finger_numbers[i - 1] != finger_numbers[i]) {
			if (thisNFrequency > highestFreq) {
				mostFreqNum = finger_numbers[i - 1];
				highestFreq = thisNFrequency;
			}
			thisNFrequency = 0;
		}
		thisNFrequency++;
	}
	if (thisNFrequency > highestFreq) {
		mostFreqNum = finger_numbers[finger_numbers.size() - 1];
	}
	mostFrequentFN = mostFreqNum;
}
