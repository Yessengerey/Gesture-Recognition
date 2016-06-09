#include "ROI.h"

ROI::ROI() {
	bot_corner = Point(0, 0);
	top_corner = Point(0, 0);
}

ROI::ROI(Image* img, std::string windowName) {
	namedWindow(windowName, WINDOW_AUTOSIZE);
	displayROI(img, windowName);
	gm.GetTimedMS();
	recordValues(img, windowName);
	std::cout << gm.GetTimedMS() << std::endl;
	destroyWindow(windowName);
}

ROI::ROI(Point lower_p, Point upper_p, Mat src) {
	
	bot_corner = lower_p;
	top_corner = upper_p;

	colour = Scalar(255, 255, 0);

	thickness = 2;

	width = top_corner.x - bot_corner.x;
	height = top_corner.y - bot_corner.y;

	pointer = src(Rect(bot_corner.x, bot_corner.y, width, height));

}

void ROI::generateRectangle(Mat m) {
	rectangle(m, bot_corner, top_corner, colour, thickness);
}

int ROI::findMedian(std::vector<int> v) {
	int median;
	size_t size = v.size();
	sort(v.begin(), v.end());
	if (size % 2 == 0) {
		median = v[size / 2 - 1];
	}
	else {
		median = v[size / 2];
	}
	return median;
}

void ROI::displayROI(Image* m, std::string windowName) {

	m->cap.read(m->sourceMat);
	flip(m->sourceMat, m->sourceMat, 1);

	vecROI.push_back(ROI(Point(m->sourceMat.cols / 3, m->sourceMat.rows / 6), Point(m->sourceMat.cols / 3 + lengthOfRoi, m->sourceMat.rows / 6 + lengthOfRoi), m->sourceMat));
	vecROI.push_back(ROI(Point(m->sourceMat.cols / 4, m->sourceMat.rows / 2), Point(m->sourceMat.cols / 4 + lengthOfRoi, m->sourceMat.rows / 2 + lengthOfRoi), m->sourceMat));
	vecROI.push_back(ROI(Point(m->sourceMat.cols / 3, m->sourceMat.rows / 1.5), Point(m->sourceMat.cols / 3 + lengthOfRoi, m->sourceMat.rows / 1.5 + lengthOfRoi), m->sourceMat));
	vecROI.push_back(ROI(Point(m->sourceMat.cols / 2, m->sourceMat.rows / 2), Point(m->sourceMat.cols / 2 + lengthOfRoi, m->sourceMat.rows / 2 + lengthOfRoi), m->sourceMat));
	vecROI.push_back(ROI(Point(m->sourceMat.cols / 2.5, m->sourceMat.rows / 2.5), Point(m->sourceMat.cols / 2.5 + lengthOfRoi, m->sourceMat.rows / 2.5 + lengthOfRoi), m->sourceMat));
	vecROI.push_back(ROI(Point(m->sourceMat.cols / 2, m->sourceMat.rows / 1.5), Point(m->sourceMat.cols / 2 + lengthOfRoi, m->sourceMat.rows / 1.5 + lengthOfRoi), m->sourceMat));
	vecROI.push_back(ROI(Point(m->sourceMat.cols / 2.5, m->sourceMat.rows / 1.8), Point(m->sourceMat.cols / 2.5 + lengthOfRoi, m->sourceMat.rows / 1.8 + lengthOfRoi), m->sourceMat));

	for (int k = 0; k < 50; ++k) {
		m->cap.read(m->sourceMat);

		flip(m->sourceMat, m->sourceMat, 1);

		for (int i = 0; i < 7; ++i) {
			vecROI[i].generateRectangle(m->sourceMat);
		}

		std::string text = "Please cover rectangles with your palm";
		accessImg.printText(m->sourceMat, text, Scalar(0, 0, 255));
		
		imshow(windowName, m->sourceMat);

		if (waitKey(30) >= 0) break;
	}
}

void ROI::findAverageColour(Image *m, ROI r, int average[3]) {
	Mat img;
	r.pointer.copyTo(img);

	std::vector<int> Hue;
	std::vector<int> Sat;
	std::vector<int> Lig;

	for (int i = 2; i < img.rows - 2; i++) {
		for (int j = 2; j < img.cols - 2; j++) {
			Hue.push_back(img.data[img.channels()*(img.cols*i + j) + 0]); //channels() returns the number of channels in the matrix
			Sat.push_back(img.data[img.channels()*(img.cols*i + j) + 1]);
			Lig.push_back(img.data[img.channels()*(img.cols*i + j) + 2]);
		}
	}

	average[0] = findMedian(Hue);
	average[1] = findMedian(Sat);
	average[2] = findMedian(Lig);
}

void ROI::recordValues(Image* m, std::string windowName) {

	m->cap >> m->sourceMat;
	flip(m->sourceMat, m->sourceMat, 1);

	for (int i = 0; i < 30; i++) {

		m->cap >> m->sourceMat;
		flip(m->sourceMat, m->sourceMat, 1);

		
		//cvtColor(m->sourceMat, m->sourceMat, CV_BGR2HLS);
		cvtColor(m->sourceMat, m->sourceMat, CV_BGR2HSV);

		for (int j = 0; j < 7; j++) {
			findAverageColour(m, vecROI[j], averageCol[j]);
			vecROI[j].generateRectangle(m->sourceMat);
		}

		//cvtColor(m->sourceMat, m->sourceMat, CV_HLS2BGR);
		cvtColor(m->sourceMat, m->sourceMat, CV_HSV2BGR);

		std::string imgText = "Finding average color of hand...";
		accessImg.printText(m->sourceMat, imgText, Scalar(0, 0, 255));
		
		imshow(windowName, m->sourceMat);

		if (cv::waitKey(30) >= 0) break;
	}
}

void ROI::normalizeCol(Trackbars *t) {
	for (int i = 0; i < 7; i++) {
		if ((averageCol[i][0] - t->lower_HSL[i][0]) < 0) {
			t->lower_HSL[i][0] = averageCol[i][0];

		}
		if ((averageCol[i][1] - t->lower_HSL[i][1]) < 0) {
			t->lower_HSL[i][1] = averageCol[i][1];

		}
		if ((averageCol[i][2] - t->lower_HSL[i][2]) < 0) {
			t->lower_HSL[i][2] = averageCol[i][2];

		}
		if ((averageCol[i][0] + t->upper_HSL[i][0]) > 255) {
			t->upper_HSL[i][0] = 255 - averageCol[i][0];

		}
		if ((averageCol[i][1] + t->upper_HSL[i][1]) > 255) {
			t->upper_HSL[i][1] = 255 - averageCol[i][1];

		}
		if ((averageCol[i][2] + t->upper_HSL[i][2]) > 255) {
			t->upper_HSL[i][2] = 255 - averageCol[i][2];
		}
	}
}

void ROI::determineThreshold(Image *img, Trackbars *t) {
	//cvtColor(img->sourceCopy, img->sourceCopy, CV_BGR2HLS);
	cvtColor(img->sourceCopy, img->sourceCopy, CV_BGR2HSV);

	for (int i = 0; i < 7; i++) {

		normalizeCol(t);

		lower_boundary = Scalar(averageCol[i][0] - t->lower_HSL[i][0], averageCol[i][1] - t->lower_HSL[i][1], averageCol[i][2] - t->lower_HSL[i][2]);

		upper_boundary = Scalar(averageCol[i][0] + t->upper_HSL[i][0], averageCol[i][1] + t->upper_HSL[i][1], averageCol[i][2] + t->upper_HSL[i][2]);

		img->threshList.push_back(Mat(img->sourceCopy.rows, img->sourceCopy.cols, CV_8U));
	
		inRange(img->sourceCopy, lower_boundary, upper_boundary, img->threshList[i]);
	}

	img->threshList[0].copyTo(img->threshold);

	for (int j = 1; j < 7; ++j) {
		img->threshold += img->threshList[j];
	}

	medianBlur(img->threshold, img->threshold, 7);
}

