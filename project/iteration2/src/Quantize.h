#ifndef QUANTIZE_H
#define QUANTIZE_H
//
// Quantize.h
// Created by Jonathon Meyer
//

class Filter;

class Quantize: public Filter
{
public:
	Quantize();
	~Quantize();
	ColorData modifyPixel(ColorData pixel);
	void setBins(float value);
private:
	float bins;
};

#endif

