//
// BlurFilter.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "ConvolutionFilter.h"
#include "BlurFilter.h"

using namespace std;

BlurFilter::BlurFilter() {
  int i;
  kernelSize = 5;

  kernel = (float **) malloc(kernelSize*sizeof(float *));
  for (i = 0; i < kernelSize; i++) {
    kernel[i] = (float *) malloc(kernelSize*sizeof(float));
  }
}

BlurFilter::~BlurFilter() {
  int i;

  for (i = 0; i < kernelSize; i++) {
    free(kernel[i]);
  }
  free(kernel);
}

// Frees the old kernel and creates a new one with a larger kernelSize
void BlurFilter::adjustKernel(float amount, int direction) {
  int i;
  for (i = 0; i < kernelSize; i++) {
    free(kernel[i]);
  }
  free(kernel);

  kernelSize = amount;
  kernel = (float **) malloc(kernelSize*sizeof(float *));
  for (i = 0; i < kernelSize; i++) {
    kernel[i] = (float *) malloc(kernelSize*sizeof(float));
  }
  calculateKernel(amount);
}

// Calculates the values of the Gaussian Blur for the kernel
void BlurFilter::calculateKernel(float amount) {
  int i, j, xVal, yVal;
  float firstArgument, secondArgument, sum;
  float e = 2.71828182846;
  float pi = 22.0/7.0;
  sum = 0.0;

  firstArgument = e/(2.0*pi*pow(amount,2.0));

  // Calculate the values for each spot in the kernel, based on their distance from the origin and the specified user amount;
  for (i=0; i < kernelSize; i++) {
    for (j=0; j < kernelSize; j++) {
      xVal = calculateXValue(i,j);
      yVal = calculateYValue(i,j);
      secondArgument = (pow(xVal,2) + pow(yVal,2))/(2*pow(amount,2));
      kernel[i][j] = firstArgument-secondArgument;
      sum += kernel[i][j];
    }
  }

  // Make values in the kernel add up to 1.0
  for (i=0; i < kernelSize; i++) {
    for (j=0; j < kernelSize; j++) {
      kernel[i][j] = (1.0/sum) * kernel[i][j];
    }
  }
}

int BlurFilter::calculateXValue(int x, int y){
  int midPoint = floor(kernelSize/2);
  return abs(x - midPoint);
}

int BlurFilter::calculateYValue(int x, int y){
  int midPoint = floor(kernelSize/2);
  return abs(y - midPoint);
}
