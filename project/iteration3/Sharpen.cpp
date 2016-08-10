//
// Sharpen.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "ConvolutionFilter.h"
#include "Sharpen.h"
#include "EdgeDetection.h"

Sharpen::Sharpen() {
  kernelSize = 3;
  kernel = (float **) malloc(kernelSize * sizeof(float *));
  for (int i = 0; i < kernelSize; i++) {
    kernel[i] = (float *) malloc(kernelSize + sizeof(float));
  }

  midPoint = floor(kernelSize/2);

  kernel[midPoint][midPoint] = (float) kernelSize*kernelSize;
  for (int j = 0; j < kernelSize; j++) {
    for (int k = 0; k < kernelSize; k++) {
      if ((j == k) == midPoint) {
        // Do Nothing
      } else {
        kernel[j][k] = -1.0;
      }
    }
  }
}

Sharpen::~Sharpen() {
  for (int i = 0; i < kernelSize; i++) {
    free(kernel[i]);
  }
  free(kernel);
}

void Sharpen::adjustKernel(float amount, int direction) {
  int i,j;
  resizeKernel(amount);
  midPoint = floor(kernelSize/2);

  kernel[midPoint][midPoint] = (float) kernelSize*kernelSize;
  for (i=0; i < kernelSize; i++) {
    for (j = 0; j < kernelSize; j++) {
      if ((i == midPoint) && (j==midPoint)) {
        // Do Nothing
      } else {
        kernel[i][j] = -1.0;
      }
    }
  }
  for (i=0; i < kernelSize; i++) {
    for (j = 0; j < kernelSize; j++) {
      std::cout << kernel[i][j] << std::endl;
    }
  }
}

void Sharpen::resizeKernel(float amount) {
  int i;
  int myAmount = (int) amount;
  for (i=0;i<kernelSize;i++) {
    delete[] kernel[i];
  }
  delete[] kernel;

  float myModulo = myAmount % 2;

  if (myModulo==0.0) {
    myAmount++;
  } else if (myModulo==1.0) {
    // Do nothing
  } else if (myModulo>1.0) {
    myAmount = floor(myAmount);
  } else if (myModulo<1.0) {
    myAmount=ceil(myAmount);
  }

  kernelSize = myAmount;
  kernel = (float **) malloc(kernelSize * sizeof(float *));
  for (int i = 0; i < kernelSize; i++) {
    kernel[i] = (float *) malloc(kernelSize * sizeof(float));
  }

}
