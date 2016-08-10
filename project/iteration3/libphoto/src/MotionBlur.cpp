//
// MotionBlur.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "ConvolutionFilter.h"
#include "MotionBlur.h"

using std::cout;
using std::endl;

MotionBlur::MotionBlur() {
  int i, j;
  kernelSize = 9;
  midPoint = floor(kernelSize/2);

  kernel = (float **) malloc(kernelSize*sizeof(float *));
  for (i = 0; i < kernelSize; i++) {
    kernel[i] = (float *) malloc(kernelSize*sizeof(float));
  }

  for (i = 0; i < kernelSize; i++) {
    for (j = 0; j < kernelSize; j++) {
      kernel[i][j] = 0.0;
    }
  }

}

MotionBlur::~MotionBlur() {
  int i;

  for (i = 0; i < kernelSize; i++) {
    delete[] kernel[i];
  }
  delete[] kernel;
}

void MotionBlur::resizeKernel(float amount) {
  int i;
  int myAmount = (int) amount;
  for (i=0;i<kernelSize;i++) {
    delete[] kernel[i];
  }
  delete[] kernel;

  if ((myAmount%2)==0) {
    myAmount++;
  } else {
    myAmount=ceil(myAmount);
  }

  kernelSize = myAmount;
  kernel = (float **) malloc(kernelSize * sizeof(float *));
  for (int i = 0; i < kernelSize; i++) {
    kernel[i] = (float *) malloc(kernelSize * sizeof(float));
  }
}


void MotionBlur::adjustKernel(float amount, int direction) {
  int i, j;
  float blurRatio = 1.0 / amount;
  resizeKernel(amount);
  midPoint = floor(kernelSize/2);

  // Reinitialize kernel to 0.0
  for (i = 0; i < kernelSize; i++) {
    for (j = 0; j < kernelSize; j++) {
      kernel[i][j] = 0.0;
    }
  }

  if (direction == 0) {
    for (i = 0; i < kernelSize; i++) {
      kernel[midPoint][i] = blurRatio;
    }
  } else if (direction == 1) {
    for (i = 0; i < kernelSize; i++) {
      kernel[i][midPoint] = blurRatio;
    }
  } else if (direction == 2) {
    for (i = 0; i < kernelSize; i++) {
      for (j = 0; j < kernelSize; j++) {
        if (i == j) {
          kernel[i][j] = blurRatio;
        }
      }
    }
  } else if (direction == 3) {
    for (i = 0; i < kernelSize; i++) {
      for (j = 0; j < kernelSize; j++) {
        if ((kernelSize - 1 - j) == i) {
          kernel[i][j] = blurRatio;
        }
      }
    }
  }
}
