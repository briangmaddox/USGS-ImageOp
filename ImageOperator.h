#ifndef _IMAGEOPERATOR_H_
#define _IMAGEOPERATOR_H_
 
#include "Image/Image.h"

class ImageOperator
{
   bool statusIndicator;

 public:

   ImageOperator();
   virtual ~ImageOperator() { };
   virtual bool operate(void)=0;
   
   void giveStatus(void);
   void noStatus(void);
   void toggleStatus(void);
   bool getStatus(void);
};
   
#endif
