#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

//
// MotionBlur.h
// Created by Jacob Grafenstein
//

class ConvolutionFilter;

class MotionBlur : public ConvolutionFilter {
public:
  MotionBlur();
  ~MotionBlur();
  void adjustKernel(float amount, int direction);
private:
  void resizeKernel(float amount);


};

#endif
