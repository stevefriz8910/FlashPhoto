//
// XPen.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Tool.h"
#include "XPen.h"

using namespace std;
using std::cerr;
using std::endl;
using std::fill;

XPen::XPen() {
	int i, j;
  maskSize = 21;

	// Allocate space for mask, double array
  	mask=(float**) malloc(maskSize*sizeof(float*));
    	for (i=0;i<maskSize;i++) {
      		mask[i]=(float*) malloc(maskSize*sizeof(float));
    	}

	// Stores the float values inside the mask
	for (i=0;i<maskSize;i++) {
			for (j=0;j<maskSize;j++) {
				mask[i][j] = 0.0;
			}
	}
  for (i=0;i<maskSize;i++) {
      for (j=0;j<maskSize;j++) {
        	if (i == j) {
          		mask[i][j] = 1.0;
          		mask[i][maskSize-j-1] = 1.0;
        	}
      }
  }
}

// Deallocates the space created by the mask
XPen::~XPen() {
  	int i;
  	for (i=0;i<maskSize;i++) {
      		free(mask[i]);
  	}
  	free(mask);
}
