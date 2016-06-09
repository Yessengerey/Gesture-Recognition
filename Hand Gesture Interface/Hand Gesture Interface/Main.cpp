#include "TimerPerf.h"
#include "Main.h"
#include "Trackbars.h"
#include "ROI.h"
#include "Gesture.h"
#include <time.h>

/*YESSENGEREY BOLATOV 130545567
This code was submitted as supplementary material to my dissertation*/

//using namespace cv;
bool showTracks = true;
//FPS COUNTER VARIABLES
static double timepassed = 0;
static int frames = 0;
static double starttime = 0;
static bool first = true;
static float fps = 0.0f;

void displayFPS(double timepassed) {
	if (first)
	{
		frames = 0;
		starttime = timepassed;
		first = false;
	}
	frames++;
	if (timepassed - starttime > 0.25 && frames > 10)
	{
		fps = (double)frames / (timepassed - starttime);
		starttime = timepassed;
		frames = 0;
	}

	std::cout << "FPS: " << 1000 / fps << std::endl;

}

void showWindows(Image img) {
	//Found a method to put the threshold image within the original video feed
	/*pyrDown(img.threshold, img.threshold);
	pyrDown(img.threshold, img.threshold);

	Rect roi(Point(3 * img.sourceMat.cols / 4, 0), img.threshold.size());
	std::vector<Mat> channels;
	Mat result;
	for (int i = 0; i < 3; i++)
		channels.push_back(img.threshold);
	merge(channels, result);
	result.copyTo(img.sourceMat(roi));*/


	imshow(windowNameOriginal, img.sourceMat);
	imshow(windowNameThresh, img.threshold);
}

int main() {
	int counter = 0;
	//Initialise the VideoCapture object provided the number of the camera source (0 is default)
	Image mainImg(0); 

	//Initialise the Gesture object to access functions to compute hand gesture information
	Gesture handGesture;

	//Basically save frames from video capture of the camera to a matrix image (analogous to cap.read(sourceMat))
	mainImg.cap >> mainImg.sourceMat;

	//Create ROIs on the screen to scan the hand then record values of HLS in order to properly define the colour of the skin
	ROI handScan(&mainImg, windowNameOriginal);

	//Initialise windows (one for the video capture and one for the trackbars) and trackbars 
	namedWindow(windowNameOriginal, CV_WINDOW_AUTOSIZE);

	//Initalise the trackbars control window (the exact values of HSL will be seen in this window)
	Trackbars initTracks(showTracks);
	
	while (1) {
		timepassed = clock() / 1000;
		//Count the frame rate 
		handGesture.nFrames++;

		//Fromat the image: blur it and downsample it,
		//and convert to the HSL for further use
		mainImg.formatImage();

		//Determine the threshold based on HSL values
		//This step is necessary in order to find the
		//contours of the hand object
		handScan.determineThreshold(&mainImg, &initTracks);

		//Find the contours of the main hand object 
		handGesture.determineContours(&mainImg);

		//Find and display the finger tips on the screen
		handGesture.determineFingerNumbers(&mainImg);

		//Sample down the threshold (minimize) and insert it
		//into the original feed (threshold can be seen in the 
		//original feed in the top right corner)
		
		showWindows(mainImg);
		if(counter < 100){
			displayFPS(timepassed);
			counter++;
		}
		if (cv::waitKey(30) == 27) break;
	}
	cvDestroyAllWindows();
	mainImg.cap.release();
	return 0;
}