//
// ComicBook.cpp
// Created by Jacob Grafenstein
//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "Filter.h"
#include "ComicBook.h"
#include "Quantize.h"

using namespace std;

ComicBook::ComicBook() {

}

ComicBook::~ComicBook() {

}

ColorData ComicBook::modifyPixel(ColorData pixel) {
  ColorData myPixel = ColorData(80.0/255,80.0/255,80.0/255);
  return myPixel;
}

void ComicBook::applyFilter(PixelBuffer *buf) {
  int i,j,k,l,width,height;
	ColorData tempPixel;
	width = buf -> getWidth();
	height = buf -> getHeight();
  Quantize *myQuantize = new Quantize();
  myQuantize->setBins(2);
  myQuantize->applyFilter(buf);
  for (i = 0; i < width; i+=8) {
    for (j = 0; j < height; j+=8) {
      for (k = 0; k < 2; k++) {
        for (l = 0; l < 2; l++) {
          cout << "Editing pixel: x=" << i << " & y=" << j << "!" << endl;
          tempPixel = modifyPixel(buf -> getPixel(i+k,j+l));
    			buf -> setPixel(i+k,j+l,tempPixel);
        }
      }
    }
  }
}
