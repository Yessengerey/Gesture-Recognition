#pragma once
#include <opencv2\highgui.hpp>
#include "Image.h"
#include <string>

class Trackbars {
public:
	Trackbars(bool show);

	int Heu_Min = 12; 
	int Heu_Max = 7;

	int Sat_Min = 30;
	int Sat_Max = 30;

	int Lit_Min = 80;
	int Lit_Max = 80;

	const int width = 640;
	const int height = 480;


	int lower_HSL[7][3];
	int upper_HSL[7][3];

	const std::string windowNameTrackbars = "Trackbars";

	void makeTrackbars(bool);		



};