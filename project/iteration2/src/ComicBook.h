#ifndef COMICBOOK_H
#define COMICBOOK_H

//
// ComicBook.h
// Created by Jacob Grafenstein
//

class Filter;
class Quantize;

class ComicBook : public Filter {
public:
  ComicBook();
  ~ComicBook();
  void applyFilter(PixelBuffer *buf);
  ColorData modifyPixel(ColorData pixel);


private:

};

#endif
