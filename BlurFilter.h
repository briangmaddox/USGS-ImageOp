#ifndef _BLURFILTER_H_
#define _BLURFILTER_H_

#include "ImageOp/ToPixelFilter.h"

class BlurFilter : public ToPixelFilter
{
 public:
   BlurFilter(unsigned char photo,int size);
   virtual ~BlurFilter() {};

   virtual unsigned char* getFillPixel(void);
};

#endif
