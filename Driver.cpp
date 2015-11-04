// Driver.cpp			Author: Sam Hoover
// This program takes an image and linearly transforms it according to specifications
// from the user via command line arguments. The input image must be titled "test.gif"
// and be located in the same file location as Driver.cpp. 
//
// Linear transformations are performed according to the formulas discussed in
// class (UWB CSS487, Spring 2015):
// q = SKRp + t, or more accurately, it's inverse form:
// p = S^(-1) * K^(-1) * R^(-1) * (p - t - c) + c
//
// This program utilizes Image.h and Image.lib, provided by Prof. Olson, UWB
// CSS487 Spring 2015.
//
#define _USE_MATH_DEFINES
#include <cmath> 
#include <iostream>
#include <string>
#include "Image.h"

using namespace std;

// createOutputImage
// output_img becomes a copy of input_img, linearly transformed according to
// the given parameters.
// preconditions:	input_img and output_img must point to valid Image objects
//					and must not be equal to nullptr
// postconditions:	output_img becomes a copy of input_img, linearly transformed according to
//					the given parameters
//
void createOutputImage(const Image *input_img, 
					   Image *output_img,
					   double x_scale,
					   double y_scale,
					   double x_trans,
					   double y_trans,
					   double theta,
					   double k_val);

// linearTransformation (createOutputImage Helper)
// Performs a linear transformation on x and y according to the given parameters:
// x_scale, y_scale, x_trans, y_trans, theta, k_val.
// Uses the following formula for tranformation:
// p = S^(-1) * K^(-1) * R^(-1) * (p - t - c) + c
// preconditions:	requires inclusion of <cmath> and #define _USE_MATH_DEFINES
// postcondition:	Performs a linear transformation on x and y according to the given
//					parameters: x_scale, y_scale, x_trans, y_trans, theta, k_val
//
void linearTransformation(double &x,
						  double &y,
						  int x_max,
						  int y_max,
						  double x_scale,
						  double y_scale,
						  double x_trans,
						  double y_trans,
						  double theta,
						  double k_val);

// bilinerInterpolation (createOutputImage helper)
// performs bilinear interpolation on x and y and sets the pixel value at 
// output_img->pixel[x][y] to the resulting values
// preconditions:	input_img and output_img must point to valid Image objects
//					and must not be equal to nullptr
// postconditions:	sets the pixel value at output_img->pixel[x][y] to the bilinear
//					interpolation of x and y
//
void bilinearInterpolation(double x,
						   double y,
						   int x_outLoc,
						   int y_outLoc,
						   const Image *input_img,
						   Image *output_img);

// main
// performs a linear transformation on "test.gif" and saves the new image as 
// "output.gif." Transformations correspond to the commandline arguments.
// preconditions:	argv must contain 6 numerical inputs, in this order: x_scale,
//					y_scale, x_translation, y_translation, theta, and k (shear)
//					value. 
//					An image titled "test.gif" must be included in the same file
//					location as Driver.cpp.
//					Image.h and Image.lib must be included.
// postconditions:	creates an .gif image, titled "output.gif" which is a linearly
//					transformed version of "test.gif." Transformations correspond
//					to values in the command line arguments. output.gif will be 
//					created in the same file location as Driver.cpp.
//
int main(int argc, char *argv[]) {
	double x_scale;
	double y_scale;
	double x_trans;
	double y_trans;
	double theta;
	double k_val;
	Image input_img("test.gif");
	Image output_img(input_img.getRows(), input_img.getCols());

	sscanf_s(argv[1], "%lf", &x_scale);
	sscanf_s(argv[2], "%lf", &y_scale);
	sscanf_s(argv[3], "%lf", &x_trans);
	sscanf_s(argv[4], "%lf", &y_trans);
	sscanf_s(argv[5], "%lf", &theta);
	sscanf_s(argv[6], "%lf", &k_val);
	
	createOutputImage(&input_img, &output_img, x_scale, y_scale, x_trans, y_trans, theta, k_val);
	output_img.writeImage("output.gif");
	return(0);
}

// createOutputImage
// output_img becomes a copy of input_img, linearly transformed according to
// the given parameters.
// preconditions:	input_img and output_img must point to valid Image objects
//					and must not be equal to nullptr
// postconditions:	output_img becomes a copy of input_img, linearly transformed according to
//					the given parameters
//
void createOutputImage(const Image *input_img,
					   Image *output_img,
					   double x_scale,
					   double y_scale,
					   double x_trans,
					   double y_trans,
					   double theta,
					   double k_val) {

	for(int i = 0; i < output_img->getCols(); i++) {
		for(int j = 0; j < output_img->getRows(); j++) {
			double x = i;
			double y = j;
			linearTransformation(x, y, output_img->getCols(), output_img->getRows(),
								 x_scale, y_scale, x_trans, y_trans, theta, k_val);
			bilinearInterpolation(x, y, i, j, input_img, output_img);
		}
	}
}

// linearTransformation (createOutputImage Helper)
// Performs a linear transformation on x and y according to the given parameters:
// x_scale, y_scale, x_trans, y_trans, theta, k_val.
// Uses the following formula for tranformation:
// p = S^(-1) * K^(-1) * R^(-1) * (p - t - c) + c
// preconditions:	requires inclusion of <cmath> and #define _USE_MATH_DEFINES
// postcondition:	Performs a linear transformation on x and y according to the given
//					parameters: x_scale, y_scale, x_trans, y_trans, theta, k_val
//
void linearTransformation(double &x,
						  double &y,
						  int x_max,
						  int y_max,
						  double x_scale,
						  double y_scale,
						  double x_trans,
						  double y_trans,
						  double theta,
						  double k_val) {
	// go to center
	x -= (x_max / 2);
	y -= (y_max / 2);

	// inverse translation
	x -= x_trans;
	y -= y_trans;

	// inverse scale
	if(x_scale > 0 || y_scale > 0) {
		x = x / x_scale;
		y = y / y_scale;
	}

	// inverse shear
	x -= (k_val * y);

	// inverse rotation
	double x_val = x;
	double y_val = y;
	x = (x_val * cos(theta * M_PI / 180)) + (y_val * sin(theta * M_PI / 180));
	y = (-x_val * sin(theta * M_PI / 180)) + (y_val * cos(theta * M_PI / 180));

	// return to origin
	x += (x_max / 2);
	y += (y_max / 2);
}

// bilinerInterpolation (createOutputImage helper)
// performs bilinear interpolation on x and y and sets the pixel value at 
// output_img->pixel[x][y] to the resulting values
// preconditions:	input_img and output_img must point to valid Image objects
//					and must not be equal to nullptr
// postconditions:	sets the pixel value at output_img->pixel[x][y] to the bilinear
//					interpolation of x and y
//
void bilinearInterpolation(double x,
						   double y,
						   int x_outLoc,
						   int y_outLoc,
						   const Image* input_img,
						   Image *output_img) {
	// bilinear interpolation
	int x1 = static_cast<int>(floor(x));
	int x2 = static_cast<int>(ceil(x));
	int y1 = static_cast<int>(floor(y));
	int y2 = static_cast<int>(ceil(y));
	if(x >= x2) {
		x2 = static_cast<int>(ceil(x + 1));
	}
	if(y >= y2) {
		y2 = static_cast<int>(ceil(y + 1));
	}

	if(x1 >= 0 && x1 < input_img->getCols() && x2 >= 0 && x2 < input_img->getCols() &&
		y1 >= 0 && y1 < input_img->getRows() && y2 >= 0 && y2 < input_img->getRows()) {

		byte red_val = static_cast<byte> ((1 - (x - x1)) * (1 - (y - y1)) * input_img->getPixel(y1, x1).red +
										  (1 - (x - x1)) * (1 - (y2 - y)) * input_img->getPixel(y2, x1).red +
										  (1 - (x2 - x)) * (1 - (y - y1)) * input_img->getPixel(y1, x2).red +
										  (1 - (x2 - x)) * (1 - (y2 - y)) * input_img->getPixel(y2, x2).red);

		byte green_val = static_cast<byte> ((1 - (x - x1)) * (1 - (y - y1)) * input_img->getPixel(y1, x1).green +
											(1 - (x - x1)) * (1 - (y2 - y)) * input_img->getPixel(y2, x1).green +
											(1 - (x2 - x)) * (1 - (y - y1)) * input_img->getPixel(y1, x2).green +
											(1 - (x2 - x)) * (1 - (y2 - y)) * input_img->getPixel(y2, x2).green);

		byte blue_val = static_cast<byte> ((1 - (x - x1)) * (1 - (y - y1)) * input_img->getPixel(y1, x1).blue +
										   (1 - (x - x1)) * (1 - (y2 - y)) * input_img->getPixel(y2, x1).blue +
										   (1 - (x2 - x)) * (1 - (y - y1)) * input_img->getPixel(y1, x2).blue +
										   (1 - (x2 - x)) * (1 - (y2 - y)) * input_img->getPixel(y2, x2).blue);

		pixel pix = input_img->getPixel(static_cast<int>(y), static_cast<int>(x));
		pix.red = red_val;
		pix.green = green_val;
		pix.blue = blue_val;
		output_img->setPixel(y_outLoc, x_outLoc, pix);
	}
}
