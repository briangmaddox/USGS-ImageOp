#ifndef _UNARYIMAGEOPERATOR_H_
#define _UNARYIMAGEOPERATOR_H_
 
#include "Image/Image.h"
#include "ImageOp/ImageOperator.h"

class UnaryImageOperator : public ImageOperator
{
   Image *_fin;
   Image *_fout;

 protected:

   enum OpMethods { OpNone=0, OpPixel=1, OpRow=2, OpRect=3 };
   enum OpTypes   { OpRaw=0,   OpCanonical=1 };

 private:

   OpMethods _method;
   OpTypes   _type; 
   
    
 public:

   UnaryImageOperator(Image *source, Image *target);
   virtual ~UnaryImageOperator() { };
   virtual bool operate(void);
   

 protected:

   virtual bool operateByPixel(void)=0;
   virtual bool operateByRow(void)=0;
   virtual bool operateByRect(void)=0;
   virtual bool operateRawByPixel(void)=0;
   virtual bool operateRawByRow(void)=0;
   virtual bool operateRawByRect(void)=0;

   Image *getSource(void);
   Image *getTarget(void);
   OpMethods getMethod(void);
   OpTypes   getType(void);
   void      setMethod(OpMethods m);
   void      setType(OpTypes t);
   bool copyPalletteIfNeeded(void);
};
   
#endif
