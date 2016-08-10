#ifndef BLUR_H
#define BLUR_H
//
// XPen.h
// Created by Jonathon Meyer
//

class Tool;

class Blur: public Tool
{
public:
  Blur();
  virtual ~Blur();
  void paintMask(int x,int y,PixelBuffer **pixelBuffer,ColorData color,ColorData backgroundColor);
};

#endif



