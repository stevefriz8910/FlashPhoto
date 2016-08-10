//
// SprayCan.cpp
// Created by Stevie Frisbie
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <cmath>
#include "Tool.h"
#include "SprayCan.h"

using std::cout;
using std::cerr;
using std::endl;
using std::fill;

SprayCan::SprayCan() {
	int i, j;
	maskSize = 41;

	// Allocate space for mask
	mask = (float**) malloc(maskSize * sizeof(float*));
	for (i = 0; i < maskSize; i++) {
		mask[i] = (float*) malloc(41 * sizeof(float));
	}

	// Set float values for mask
	for (i = 0; i < maskSize; i++) {
		for (j = 0; j < maskSize; j++) {
			float distance = calculateDistance(i, j, 20.0);
			if (distance >= 20.0) {
				mask[i][j] = 0;
			}	else {
				mask[i][j] = (20.0 - distance) * .01;
			}
		}
	}
}

// Deallocate Space created by the Spray Can Mask
SprayCan::~SprayCan() {
  int i;
  for (i=0;i<maskSize;i++) {
    delete[] mask[i];
  }
  delete[] mask;
}
