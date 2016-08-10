//
// Pen.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Tool.h"
#include "Pen.h"

using namespace std;
using std::cerr;
using std::endl;
using std::fill;

Pen::Pen() {
	int i, j;
	maskSize = 7;
    
	// Allocate space for mask, double array
  	mask=(float**) malloc(7*sizeof(float*));
	for (i=0;i<maskSize;i++) {
		mask[i]=(float*) malloc(maskSize*sizeof(float));
    	}
	
	// Store float values within the mask
    	for (i=0;i<maskSize;i++) {
      		for (j=0;j<maskSize;j++) {
        		if (((2 < i) && (i < 6)) && ((2 < j) && (j < 6))) {
          			mask[i][j] = 1.0;
        		}
        		else {
          			mask[i][j] = 0;
        		}
      		}
    	}
}

// Deallocate space created by the Pen mask
Pen::~Pen() {
  	int i;
  	for (i=0;i<maskSize;i++) {
      		free(mask[i]);
  	}
  	free(mask);
}
