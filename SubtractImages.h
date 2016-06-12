#ifndef _SUBTRACTIMAGES_H_
#define _SUBTRACTIMAGES_H_

#include "ImageOp/BinaryImageOperator.h"

class SubtractImages: public BinaryImageOperator
{
   bool _canSubtract;

 public:

   SubtractImages(Image *s1, Image *s2, Image *target);
   ~SubtractImages() {};

 protected:

   virtual bool operateRawByPixel(void);
   virtual bool operateRawByRow(void);
   virtual bool operateRawByRect(void);
   virtual bool operateByPixel(void);
   virtual bool operateByRow(void);
   virtual bool operateByRect(void);
};

#endif
