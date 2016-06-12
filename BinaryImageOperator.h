#ifndef _BINARYIMAGEOPERATOR_H_
#define _BINARYIMAGEOPERATOR_H_
 
#include "Image/Image.h"
#include "ImageOp/ImageOperator.h"
#include "Image/Pallette.h"

class BinaryImageOperator : public ImageOperator
{
   Image *_fin1;
   Image *_fin2;
   Image *_fout;
   
 protected:
   enum OpMethods { OpNone=0, OpPixel=1, OpRow=2, OpRect=3 };
   enum OpTypes   { OpRaw=0,  OpCanonical=1 };

 private:

   OpMethods _method;
   OpTypes   _type;

 public:

   BinaryImageOperator(Image *source1, Image *source2, Image *target);
   virtual ~BinaryImageOperator() { };
   virtual bool operate(void);
   
 protected:

   virtual bool operateByPixel(void)=0;
   virtual bool operateByRow(void)=0;
   virtual bool operateByRect(void)=0;
   virtual bool operateRawByPixel(void)=0;
   virtual bool operateRawByRow(void)=0;
   virtual bool operateRawByRect(void)=0;

   Image *getSource1(void);
   Image *getSource2(void);
   Image *getTarget(void);

   OpMethods getMethod(void);
   OpTypes   getType(void);

   bool copyPalletteIfNeeded(void);
   bool comparePallettes(Pallette*,Pallette*,unsigned char);
};
   
#endif
