//
// XPen.h
// Created by Jonathon Meyer
//

#ifndef BLUR_H
#define BLUR_H

class Tool;

class Blur: public Tool {
public:
  Blur();
  virtual ~Blur();
  void paintMask(int x,int y,PixelBuffer** pixelBuffer,ColorData color,ColorData backgroundColor);

private:

};

#endif
