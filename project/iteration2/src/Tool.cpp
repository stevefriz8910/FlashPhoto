//
// Tool.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <cmath>
#include "Tool.h"

using std::cerr;
using std::endl;
using std::fill;

// Returns the mask
float** Tool::getMask() {
	return mask;
}

// Returns the size of the mask, different for every tool
int Tool::getMaskSize() {
  return maskSize;
}

// Calculates the distance between two points, used for SprayCan and Eraser
float Tool::calculateDistance(int x, int y, float mid) {
  return sqrt(pow(((float) x - mid), 2) + pow(((float) y - mid), 2));
}

// Returns the mask at a given pixel
float Tool::getPixel(int x,int y) {
	if (x>=0 && y>=0 && x<maskSize && y<maskSize) {
		return mask[x][y];
	}
	else {
		return -1;
	}
}

// Default paintMask function, can be overwritten by tools
void Tool::paintMask(int x,int y,PixelBuffer **displayBuffer,ColorData color,ColorData backgroundColor) {
	int i,j,bufferI,bufferJ,width,height;
	width = (*displayBuffer)->getWidth();
	height = (*displayBuffer)->getHeight();
	ColorData tempPixel;

	// Applies the mask to the display buffer
	for (i=0;i<maskSize;i++) {
		for (j=0;j<maskSize;j++) {
			bufferI = x + i - (maskSize/2) - 1;
			bufferJ = y + j - (maskSize/2) - 1;

			// Make sure the width and length are correct, be prepared to swap if necessary
			if ((bufferI > 0) && (bufferI < width) && (bufferJ > 0) && (bufferJ < height)) {
				tempPixel = (**displayBuffer).getPixel(bufferI,height - bufferJ) * (1 - getPixel(i,j));
				(**displayBuffer).setPixel(bufferI,height - bufferJ,tempPixel + (color * getPixel(i,j)));
			}
		}
	}
}
