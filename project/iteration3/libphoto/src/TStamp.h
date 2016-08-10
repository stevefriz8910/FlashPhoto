//
//  TStamp.h
//  PhotoShop Proj
//
//  Created by Seth Johnson on 4/4/15.
//  Copyright (c) 2015 Seth Johnson. All rights reserved.
//
//  For educational purposes only.  Please do not post online.

#ifndef TSTAMP_H
#define TSTAMP_H

#include <stdio.h>
#include "Tool.h"

class PixelBuffer;

class TStamp: public Tool {
public:
    TStamp();
    virtual ~TStamp();
    virtual int getWidth();
    virtual int getHeight();

    void setStampBuffer(PixelBuffer* stampBuffer);
    PixelBuffer* getStampBuffer();
    void paintMask(int x,int y,PixelBuffer** displayBuffer,ColorData color, ColorData background);

protected:
    static PixelBuffer *m_stampBuffer;

};

#endif
