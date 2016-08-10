// Eraser.cpp
// Created by Stevie Frisbie

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <cmath>
#include "Tool.h"
#include "Eraser.h"

using namespace std;
using std::cerr;
using std::endl;
using std::fill;

Eraser::Eraser() {
	int i, j;
	maskSize = 21;
	
	// Allocate space for mask
	mask = (float**) malloc(maskSize * sizeof(float*));
	for (i = 0; i < maskSize; i++) {
		mask[i] = (float*) malloc(maskSize * sizeof(float));
	}
	
	// Store float values in the mask
	for (i = 0; i < maskSize; i++) {
		for (j = 0; j < maskSize; j++) {
			if (calculateDistance(i, j, 10.0) <= 10.0) {
				mask[i][j] = 1;
			}
			else {
				mask[i][j] = 0;
			}
		}
	}
}

// Deallocate space created by the Eraser
Eraser::~Eraser() {
	int i;
	for (i = 0; i < maskSize; i++) {
		free(mask[i]);
	}
	free(mask);
}

// Redefines the paintMask function from Tool.cpp so that it uses the backgroundColor instead of the colorData set by the GLUI
void Eraser::paintMask(int x,int y,PixelBuffer **displayBuffer,ColorData color,ColorData backgroundColor) {
        int i,j,bufferI,bufferJ,width,height;
        width = (*displayBuffer)->getWidth();
        height = (*displayBuffer)->getHeight();
        ColorData tempPixel;

        for (i=0;i<maskSize;i++) {
                for (j=0;j<maskSize;j++) {
                        bufferI = x + i - (maskSize/2) - 1;
                        bufferJ = y + j - (maskSize/2) - 1;
			// Make sure width and length are correct, be prepared to swap if necessary
                        if ((bufferI > 0) && (bufferI < width) && (bufferJ > 0) && (bufferJ < height)) {
                                tempPixel = (**displayBuffer).getPixel(bufferI,height - bufferJ) * (1 - getPixel(i,j));
                                (**displayBuffer).setPixel(bufferI,height - bufferJ,tempPixel + (backgroundColor * getPixel(i,j)));
                        }
                }
        }
}
