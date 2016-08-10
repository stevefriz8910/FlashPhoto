//
// Filter.h
// Created by Jonathon Meyer
//

#ifndef FILTER_H
#define FILTER_H

#include "ColorData.h"
#include "PixelBuffer.h"

class Filter {
public:
	virtual void applyFilter(PixelBuffer* buf);
	virtual ColorData modifyPixel(ColorData pixel)=0;

protected:

};

#endif
