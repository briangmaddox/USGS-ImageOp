#ifndef _LAPLACIANFILTER_H_
#define _LAPLACIANFILTER_H_

#include "ImageOp/ToPixelFilter.h"

class LaplacianFilter : public ToPixelFilter
{
 public:
   LaplacianFilter(unsigned char photo);
   virtual ~LaplacianFilter() {};

   virtual unsigned char* getFillPixel(void);
   virtual unsigned char* getPixel(unsigned char *rect);
};

#endif
