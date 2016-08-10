//
// ConvolutionFilter.h
// Created by Jonathon Meyer
//
#ifndef CON_FILTER_H
#define CON_FILTER_H

#include "ColorData.h"
#include "PixelBuffer.h"

class ConvolutionFilter
{
public:

	virtual void applyFilter(PixelBuffer *buf, float amount, int direction);
	virtual void applyKernel(int x,int y,PixelBuffer *buf,PixelBuffer *temp);

protected:
	float **kernel;
	int kernelSize;
	int midPoint;
	virtual void adjustKernel(float amount, int direction) = 0;
	virtual void resizeKernel(float amount);
};
#endif
