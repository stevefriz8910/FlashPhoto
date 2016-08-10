//
// Quantize.h
// Created by Jonathon Meyer
//

#ifndef QUANTIZE_H
#define QUANTIZE_H

class Filter;

class Quantize: public Filter {
public:
	Quantize();
	~Quantize();
	ColorData modifyPixel(ColorData pixel);
	void setBins(float value);

private:
	float bins;
	
};

#endif
