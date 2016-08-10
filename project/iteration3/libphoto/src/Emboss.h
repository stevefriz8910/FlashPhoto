//
// Emboss.h
// Created by Jacob Grafenstein
//

#ifndef EMBOSS_H
#define EMBOSS_H

class ConvolutionFilter;

class Emboss : public ConvolutionFilter {
public:
  Emboss();
  ~Emboss();
  void adjustKernel(float amount, int direction);

private:

};

#endif
