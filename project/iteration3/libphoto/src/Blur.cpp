//
// XPen.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Tool.h"
#include "Blur.h"

using namespace std;
using std::cerr;
using std::endl;
using std::fill;

Blur::Blur() {
  int i, j;
  maskSize = 15;

  // Allocate space for mask, double array
  mask=(float**) malloc(maskSize*sizeof(float*));
  for (i=0;i<maskSize;i++) {
    mask[i]=(float*) malloc(maskSize*sizeof(float));
  }

  // Stores the float values inside the mask
  for (i=0;i<maskSize;i++) {
    for (j=0;j<maskSize;j++) {
      mask[i][j] = 1;
    }
  }
}

// Deallocates the space created by the mask
Blur::~Blur() {
  int i;
  for (i=0;i<maskSize;i++) {
    delete[] mask[i];
  }
  delete[] mask;
}

void Blur::paintMask(int x,int y,PixelBuffer** pixelBuffer,ColorData color,ColorData backgroundColor) {
  ColorData mixer[maskSize][maskSize];
  int i,j,a,b,height,width;
  int xcoord,ycoord;
  float count,mult;
  height = (*pixelBuffer)->getHeight();
  width = (*pixelBuffer)->getWidth();

  //get mixed up stuff
  for (i=0;i<maskSize;i++) {
    for (j=0;j<maskSize;j++) {
      if (((i+x)<width) && ((i+x)>-1) && ((j+y)<height) && ((j+y)>-1)) {
        count = 1.0;
        mixer[i][j] = (**pixelBuffer).getPixel(i+x,height - (j+y));
        for (a = -1;a < 2;a++) {
          for (b = -1;b < 2;b++) {
            xcoord = a + i + x;
            ycoord = height - b - j - y;
            if (((xcoord)<width) && ((ycoord)<height) && ((xcoord)>-1) && ((ycoord)>-1)) {
              if (a==0 && b==0) {
                // Do Nothing
              } else {
                mixer[i][j] = mixer[i][j] + (**pixelBuffer).getPixel(xcoord,ycoord);
                count += 1.0;
              }
            }
          }
        }
      }
      mult = 1.0/count;//count should never be 0
      mixer[i][j] = mixer[i][j]*mult;
    }
  }

  for (i=0;i<maskSize;i++) {
    for (j=0;j<maskSize;j++) {
      if (((i+x)<width) && ((i+x)>-1) && ((j+y)<height) && ((j+y)>-1)) {
        (**pixelBuffer).setPixel(x+i,height - (y+j),mixer[i][j]);
      }
    }
  }
}
