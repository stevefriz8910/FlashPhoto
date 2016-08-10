//
// EdgeDetection.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "ConvolutionFilter.h"
#include "EdgeDetection.h"

using std::cout;
using std::endl;


EdgeDetection::EdgeDetection() {
  kernelSize = 3;
  xKernel = (float**) malloc(kernelSize*sizeof(float *));
  for (int i = 0; i < kernelSize; i++) {
    xKernel[i] = (float *) malloc(kernelSize * sizeof(float));
  }
  yKernel = (float**) malloc(kernelSize*sizeof(float *));
  for (int i = 0; i < kernelSize; i++) {
    yKernel[i] = (float *) malloc(kernelSize * sizeof(float));
  }

  xKernel[0][0] = -1.0;
  xKernel[0][1] = 0.0;
  xKernel[0][2] = 1.0;
  xKernel[1][0] = -2.0;
  xKernel[1][1] = 0.0;
  xKernel[1][2] = 2.0;
  xKernel[2][0] = -1.0;
  xKernel[2][1] = 0.0;
  xKernel[2][2] = 1.0;

  yKernel[0][0] = -1.0;
  yKernel[0][1] = -2.0;
  yKernel[0][2] = -1.0;
  yKernel[1][0] = 0.0;
  yKernel[1][1] = 0.0;
  yKernel[1][2] = 0.0;
  yKernel[2][0] = 1.0;
  yKernel[2][1] = 2.0;
  yKernel[2][2] = 1.0;

}

EdgeDetection::~EdgeDetection() {
  for (int i = 0; i < kernelSize; i++) {
    delete[] xKernel[i];
    delete[] yKernel[i];
  }
  delete[] xKernel;
  delete[] yKernel;
}

void EdgeDetection::applyKernel(int x, int y,PixelBuffer* buf, PixelBuffer* temp) {
	float r=0,g=0,b=0;
	int i,j,xloc,yloc,w,h;
	ColorData pixel;
	w = buf->getWidth();
	h = buf->getHeight();
	for (i = 0; i < kernelSize; i++) {
		for (j = 0; j < kernelSize; j++) {
			xloc = x+i-(kernelSize/2);
			yloc = y+j-(kernelSize/2);
			if (xloc > -1 && xloc < w && yloc > -1 && yloc < h) {
				pixel = buf -> getPixel(x + i - (kernelSize/2),y + j - (kernelSize/2));
				r += (xKernel[i][j] * pixel.getRed());
				g += (xKernel[i][j] * pixel.getGreen());
				b += (xKernel[i][j] * pixel.getBlue());
        r += (yKernel[i][j] * pixel.getRed());
				g += (yKernel[i][j] * pixel.getGreen());
				b += (yKernel[i][j] * pixel.getBlue());
			}
		}
	}
	temp -> setPixel(x,y,ColorData(r,g,b).clampedColor());
}

void EdgeDetection::adjustKernel(float amount, int direction) {
  // Do Nothing
}
