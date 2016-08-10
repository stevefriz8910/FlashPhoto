#ifndef ERASER_H
#define ERASER_H

// Eraser.h
// Created by Stevie Frisbie

class PixelBuffer;
class ColorData;
class Tool;

class Eraser: public Tool
{

  public:
    Eraser();
    virtual ~Eraser();
    void paintMask(int x,int y,PixelBuffer **displayBuffer,ColorData color,ColorData backgroundColor);
};

#endif
