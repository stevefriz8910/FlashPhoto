//
// CalligraphyPen.cpp
// Created by Jonathon Meyer
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Tool.h"
#include "CalligraphyPen.h"

using namespace std;
using std::cerr;
using std::endl;
using std::fill;

CalligraphyPen::CalligraphyPen() {
	int i,j;
	maskSize = 15;
	
	// Allocate space for mask
	mask=(float**) malloc(maskSize*sizeof(float*));
	for (i=0;i<maskSize;i++) {
		mask[i]=(float*) malloc(maskSize*sizeof(float));
	}
	
	// Store the float values for the mask
	for (i=0;i<maskSize;i++) {
		for (j=0;j<maskSize;j++) {
			if ((i<5) || (i>9)) {
				mask[i][j]=0;
			}
			else {
				mask[i][j]=1;
			}
		}
	}
}

CalligraphyPen::~CalligraphyPen() {
	int i;
	// Deallocate space created for mask
	for (i=0;i<maskSize;i++) {
		free(mask[i]);
	}
	free(mask);
}
