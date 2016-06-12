#ifndef _FILTERIMAGE_H_
#define _FILTERIMAGE_H_

#include "ImageOp/UnaryImageOperator.h"
#include "Image/Image.h"
#include "ImageOp/ToPixelFilter.h"

class FilterImage:public UnaryImageOperator
{
   ToPixelFilter *_filter;
   bool _canFilter;

 public:
   
   FilterImage(Image *source, Image *target, ToPixelFilter *filter);
   virtual ~FilterImage() {};

 protected:

   virtual bool operateRawByPixel(void);
   virtual bool operateRawByRow(void);
   virtual bool operateRawByRect(void);
   virtual bool operateByPixel(void);
   virtual bool operateByRow(void);
   virtual bool operateByRect(void);
};

#endif
