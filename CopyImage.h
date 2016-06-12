#ifndef _COPYIMAGE_H_
#define _COPYIMAGE_H_

#include "ImageOp/UnaryImageOperator.h"
#include "Image/Image.h"

class CopyImage:public UnaryImageOperator
{
   bool _canCopy;

 public:
    
   CopyImage(Image *source, Image *target);
   virtual ~CopyImage() {};

 protected:
   virtual bool operateRawByPixel(void);
   virtual bool operateRawByRow(void);
   virtual bool operateRawByRect(void);
   virtual bool operateByPixel(void);
   virtual bool operateByRow(void);
   virtual bool operateByRect(void);
};


#endif
