#include "ImageOp/SubtractImages.h"
#include "Image/Image.h"
#include "Image/ImageDebug.h"
#include "Image/RGBPixel.h"
#include "Image/GreyPixel.h"
#include "FrodoLib/SpinnerThingy.h"

#define DRECTHEIGHT 64

SubtractImages::SubtractImages(Image *s1, Image *s2, Image *target):
BinaryImageOperator(s1,s2,target)
{
   if (source->getWidth() != target->getWidth())
   {
      gDebug.msg("SubtractImages::SubtractImages(): Images must be same size.",0);
      _canSubtract = false;
      return;
   }
   if (source->getHeight() != target->getHeight())
   {
      gDebug.msg("SubtractImages::SubtractImages(): Images must be same size.",0);
      _canSubtract = false;
      return;
   }
   _canSubtract = (getMethod()!=OpNone);
}

bool SubtractImages::operateRawByPixel(void)
{
}

bool SubtractImages::operateRawByRow(void)
{
}

bool SubtractImages::operateRawByRect(void)
{
}

bool SubtractImages::operateByPixel(void)
{
}

bool SubtractImages::operateByRow(void)
{
}

bool SubtractImages::operateByRect(void)
{
}

