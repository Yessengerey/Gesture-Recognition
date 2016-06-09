#include "Image.h"

Image::Image() {

}

Image::Image(int camera) {
	camSource = camera;
	cap = VideoCapture(camera);
}

void Image::printText(Mat destination, std::string text, Scalar colour) {
	putText(destination, text, Point(destination.cols / 6, destination.rows / 10), FONT_FACE, 0.6f, colour, 1);
}

std::string Image::int2String(int number) {
	std::stringstream stream;

	stream << number;

	std::string str = stream.str();

	return str;
}

void Image::formatImage() {
	this->cap >> this->sourceMat;
	
	flip(this->sourceMat, this->sourceMat, 1);

	pyrDown(this->sourceMat, this->sourceCopy);

	blur(this->sourceCopy, this->sourceCopy, Size(3,3));
}

