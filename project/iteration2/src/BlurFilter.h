#ifndef BLURFILTER_H
#define BLURFILTER_H

//
// BlurFilter.h
// Created by Jacob Grafenstein
//

class ConvolutionFilter;

class BlurFilter : public ConvolutionFilter {
public:
  BlurFilter();
  ~BlurFilter();
  void adjustKernel(float amount, int direction);


private:
  int calculateXValue(int x, int y);
  int calculateYValue(int x, int y);
  void calculateKernel(float amount);
};

#endif
