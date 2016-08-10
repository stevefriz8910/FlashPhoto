//
// Emboss.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "ConvolutionFilter.h"
#include "Emboss.h"


Emboss::Emboss() {
  kernelSize = 3;
  int i;

  kernel = (float **) malloc(kernelSize * sizeof(float *));
  for (i = 0; i < kernelSize; i++) {
    kernel[i] = (float *) malloc(kernelSize * sizeof(float));
  }

  kernel[0][0] = 2.0;
  kernel[0][1] = 1.0;
  kernel[0][2] = 0.0;
  kernel[1][0] = 1.0;
  kernel[1][1] = 1.0;
  kernel[1][2] = -1.0;
  kernel[2][0] = 0.0;
  kernel[2][1] = -1.0;
  kernel[2][2] = -2.0;
}

Emboss::~Emboss() {
  int i;

  for (i=0; i < kernelSize; i++) {
    delete[] kernel[i];
  }
  delete[] kernel;
}

void Emboss::adjustKernel(float amount, int direction) {
  // Do Nothing
}
