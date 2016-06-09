#include "Trackbars.h"

Trackbars::Trackbars(bool show){

	for (int i = 0; i < 7; ++i) {
		lower_HSL[i][0] = Heu_Min;
		upper_HSL[i][0] = Heu_Max;

		lower_HSL[i][1] = Sat_Min;
		upper_HSL[i][1] = Sat_Max;

		lower_HSL[i][2] = Lit_Min;
		upper_HSL[i][2] = Lit_Max;
	}

	makeTrackbars(show);
};

void Trackbars::makeTrackbars(bool show) {
	if (show) {
		cv::namedWindow(windowNameTrackbars, cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Hue Min\n", windowNameTrackbars, &lower_HSL[0][0], 255);
		cv::createTrackbar("Hue Max\n", windowNameTrackbars, &upper_HSL[0][0], 255);
		cv::createTrackbar("Sat Min\n", windowNameTrackbars, &lower_HSL[0][1], 255);
		cv::createTrackbar("Sat Max\n", windowNameTrackbars, &upper_HSL[0][1], 255);
		cv::createTrackbar("Val Min\n", windowNameTrackbars, &lower_HSL[0][2], 255);
		cv::createTrackbar("Val Max\n", windowNameTrackbars, &upper_HSL[0][2], 255);
	}
}

