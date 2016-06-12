#include "ImageOp/CopyImage.h"
#include "Image/Image.h"
#include "Image/ImageDebug.h"
#include "Image/RGBPixel.h"
#include "Image/GreyPixel.h"
#include "FrodoLib/SpinnerThingy.h"
#include "Image/ImageCommon.h"

#define DRECTHEIGHT 64

CopyImage::CopyImage(Image *source, Image *target):
UnaryImageOperator(source,target)
{
   if (source->getWidth() != target->getWidth())
   {
      gDebug.msg("CopyImage::CopyImage(): Images must be same size.",0);
      _canCopy = false;
      return;
   }
   if (source->getHeight() != target->getHeight())
   {
      gDebug.msg("CopyImage::CopyImage(): Images must be same size.",0);
      _canCopy = false;
      return;
   }
   if ((source->getFileType()==IMAGE_DOQ)&&
       (target->getFileType()==IMAGE_TIFF))
      setType(OpRaw);
   _canCopy = (getMethod()!=OpNone);
}

bool CopyImage::operateRawByPixel()
{
   SpinnerThingy *thingy;
   unsigned char *pix;
   int i,j;
   bool ret;

   if (!_canCopy) return false;
   if (!copyPalletteIfNeeded())
   {
      gDebug.msg("CopyImage::copyRawByPixel(): Images are incompatible",0);
      return false;
   }
   if (getStatus())
   { 
      thingy = new SpinnerThingy;
      cout << "Copying " << getSource()->getHeight() << " scanlines." << endl;
   }
   for (j=0;j<getSource()->getHeight();j++)
   {
      if (getStatus())
         if ((j % 20) == 0)
            thingy->update(j);
      for (i=0;i<getSource()->getWidth();i++)
      {
         pix = (unsigned char *) getSource()->getRawPixel(i,j);
         if (!getSource()->good())
            return false;
         getTarget()->putRawPixel(pix,i,j);
         if (!getTarget()->good())
            return false;
         delete pix;
      }
   }
   ret = true;
   if (getStatus())
   {
      thingy->done("Done.");
      delete thingy;
   }
   return ret;
}

bool CopyImage::operateRawByRow(void)
{
   SpinnerThingy *thingy;
   unsigned char *line;
   int i;
   bool ret;

   if (!_canCopy) return false;
   if (!copyPalletteIfNeeded())
   {
      gDebug.msg("CopyImage::copyRawByPixel(): Images are incompatible",0);
      return false;
   }
   if (getStatus())
   {
      thingy = new SpinnerThingy;
      cout << "Copying " << getSource()->getHeight() << " scanlines." << endl;
   }
   for (i=0;i<getSource()->getHeight();i++)
   {
      if (getStatus())
         if ((i%20) == 0)
            thingy->update(i);
      line = (unsigned char *) getSource()->getRawScanline(i);
      if (!getSource()->good())
         return false;
      getTarget()->putRawScanline(line,i);
      if (!getTarget()->good())
         return false;
      delete[] line;
   }
   ret = true;
   if (getStatus())
   {
      thingy->done("Done.");
      delete thingy;
   }
   return ret;
}

bool CopyImage::operateRawByRect(void)
{
   bool ret;
   SpinnerThingy *thingy;
   unsigned char *rect;
   int i,j;
   int iw,ih,ow,oh;
   int w,h;
   int aw,ah;
   int rcase;

   if (!_canCopy) return false;
   if (!copyPalletteIfNeeded())
   {
      gDebug.msg("CopyImage::copyRawByRect(): Images are incompatible",0);
      return false;
   }
   if (
       !( getSource()->getImageData()->getOptimalRectangle(iw,ih)   &&
          getTarget()->getImageData()->getOptimalRectangle(ow,oh)
        )
      )
   {
      gDebug.msg("CopyImage::copyRawByRect(): Somebody don't got rect..???",0);
      return false;
   }
   rcase = 0;
   if (iw>0) rcase += 8;
   if (ih>0) rcase += 4;
   if (ow>0) rcase += 2;
   if (oh>0) rcase += 1;
   switch (rcase)
   {
      case 0:      w = getSource()->getWidth();
                   h = DRECTHEIGHT;
                   break;
      case 1:
      case 5:      w = getSource()->getWidth();
                   h = oh;
                   break;
      case 2:
      case 8:
      case 10:     w = ow;
                   h = DRECTHEIGHT;
                   break;
      case 3:
      case 7:
      case 11:
      case 15:     w = ow;
                   h = oh;
                   break;
      case 4:      w = getSource()->getWidth();
                   h = ih;
                   break;
      case 6:
      case 14:     w = ow;
                   h = ih;
                   break;
      case 9:
      case 13:     w = iw;
                   h = oh;
                   break;
      case 12:     w = iw;
                   h = ih;
                   break;
   } 
   if (getStatus())
   {
      cout << "Converting " << getSource()->getHeight() << " scanlines." << endl;
      thingy = new SpinnerThingy;
   }
   for (j=0;j<getSource()->getHeight();j+=h)
   {
      if (getStatus())
         thingy->update(j);
      for (i=0;i<getSource()->getWidth();i+=w)
      {
         aw = w;
         ah = h;
         if ((i+w-1)>(getSource()->getWidth()-1))
            aw = getSource()->getWidth() - i;
         if ((j+h-1)>(getSource()->getHeight()-1))
            ah = getSource()->getHeight() -j;
         rect = (unsigned char *) getSource()->getRawRectangle(i,j,i+aw-1,j+ah-1);
         if (!getSource()->good())
            return false;
         getTarget()->putRawRectangle(rect,i,j,i+aw-1,j+ah-1);
         if (!getTarget()->good())
            return false;
         delete[] rect;
      }
   }
   ret = true;
   if (getStatus())
   {
      thingy->update("Done.");
      delete thingy;
   }
   return ret;
}

bool CopyImage::operateByPixel(void)
{
   SpinnerThingy *thingy;
   AbstractPixel *p1;
   int i,j;
   bool ret;

   if (!_canCopy) return false;
   if (getStatus())
   {
      thingy = new SpinnerThingy;
      cout << "Converting " << getSource()->getHeight() << " scanlines." << endl;
   }
   for (j=0;j<getSource()->getHeight();j++)
   {
      if (getStatus())
         if ((j%20)==0)
            thingy->update(j);
      for (i=0;i<getSource()->getWidth();i++)
      {
         p1 = getSource()->getPixel(i,j);
         if (!getSource()->good())
            return false;
         getTarget()->putPixel(p1,i,j);
         if (!getTarget()->good())
            return false;
         delete p1;
      }
   }
   ret = true;
   if (getStatus())
   {
      thingy->done("Done.");
      delete thingy;
   }
   return ret;
}

bool CopyImage::operateByRow(void)
{
   SpinnerThingy *thingy;
   int i;
   bool ret;
   AbstractPixel *line;
   
   if (!_canCopy) return false;
   if (getSource()->getPhotometric()==getTarget()->getPhotometric()) 
   {
      if (getStatus())
      {  
         thingy = new SpinnerThingy;
         cout << "Converting " << getSource()->getHeight() << " scanlines." << endl;
      }
      for (i=0;i<getSource()->getHeight();i++)
      {
         if (getStatus())
            if ((i%20)==0)
               thingy->update(i);
         line =  getSource()->getScanline(i);
         if (!getSource()->good())
            return false;
         getTarget()->putScanline(line,i);
         if (!getTarget()->good())
            return false;
         if (getSource()->getPhotometric() == DRG_RGB)
         {
            RGBPixel *foo = (RGBPixel *) line;
            delete[] foo;
         }
         else if (getSource()->getPhotometric() == DRG_GREY)
         {
            GreyPixel *foo = (GreyPixel *) line;
            delete[] foo;
         }
      }
      ret = true;
      if (getStatus())
      {
         thingy->done("Done.");
         delete thingy;
      }
   }
   else
   {
      gDebug.msg("CopyImage::copyByRow(): can't copy different Photometrics by row",0);
      ret = false;
   }
   return ret;
}
      

bool CopyImage::operateByRect(void)
{
   bool ret;
   SpinnerThingy *thingy;
   AbstractPixel *rect;
   int i,j;
   int iw,ih,ow,oh;
   int w,h;
   int aw,ah;
   int rcase;

   if (!_canCopy) return false;
   if (
       !( getSource()->getImageData()->getOptimalRectangle(iw,ih)   &&
          getTarget()->getImageData()->getOptimalRectangle(ow,oh)
        )
      )
   {
      gDebug.msg("CopyImage::copyRawByRect(): Somebody don't got rect..???",0);
      return false;
   }
   if (getSource()->getPhotometric()==getTarget()->getPhotometric())
   {
      rcase = 0;
      if (iw>0) rcase += 8;
      if (ih>0) rcase += 4;
      if (ow>0) rcase += 2;
      if (oh>0) rcase += 1;
      switch (rcase)
      {
         case 0:      w = getSource()->getWidth();
                      h = DRECTHEIGHT;
                      break;
         case 1:
         case 5:      w = getSource()->getWidth();
                      h = oh;
                      break;
         case 2:
         case 8:
         case 10:     w = ow;
                      h = DRECTHEIGHT;
                      break;
         case 3:
         case 7:
         case 11:
         case 15:     w = ow;
                      h = oh;
                      break;
         case 4:      w = getSource()->getWidth();
                      h = ih;
                      break;
         case 6:
         case 14:     w = ow;
                      h = ih;
                      break;
         case 9:
         case 13:     w = iw;
                      h = oh;
                      break;
         case 12:     w = iw;
                      h = ih;
                      break;
      }
      if (getStatus())
      {
         cout << "Converting " << getSource()->getHeight() << " scanlines." << endl;
         thingy = new SpinnerThingy;
      }
      for (j=0;j<getSource()->getHeight();j+=h)
      {
         if (getStatus())
            thingy->update(j);
         for (i=0;i<getSource()->getWidth();i+=w)
         {
            aw = w;
            ah = h;
            if ((i+w-1)>(getSource()->getWidth()-1))
               aw = getSource()->getWidth() - i;
            if ((j+h-1)>(getSource()->getHeight()-1))
               ah = getSource()->getHeight() -j;
            rect = getSource()->getRectangle(i,j,i+aw-1,j+ah-1);
            if (!getSource()->good())
               return false;
            getTarget()->putRectangle(rect,i,j,i+aw-1,j+ah-1);
            if (!getTarget()->good())
               return false;
            if (getSource()->getPhotometric() == DRG_RGB)
            {
               RGBPixel *foo = (RGBPixel *) rect;
               delete[] foo;
            }
            else if (getSource()->getPhotometric() == DRG_GREY)
            {
               GreyPixel *foo = (GreyPixel *) rect;
               delete[] foo;
            }
         }
      }
      ret = true;
      if (getStatus())
      {
         thingy->update("Done.");
         delete thingy;
      }
   }
   else
   {
      gDebug.msg("CopyImage::copyByRect(): can't copy different Photometrics by row",0);
      ret = false;
   }
   return ret;
}
