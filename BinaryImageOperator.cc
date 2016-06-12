#include "ImageOp/BinaryImageOperator.h"
#include "Image/Image.h"
#include "Image/ImageDebug.h"
#include "Image/Pallette.h"
#include "Image/RGBPallette.h"
#include "Image/GreyPallette.h"
#include "Image/ImageCommon.h"

BinaryImageOperator::BinaryImageOperator(Image *source1, Image *source2,
                                         Image *target)
{
   _fin1 = source1;
   _fin2 = source2;
   _fout = target;

   if (source1->canReadRectangle() &&
       source2->canReadRectangle() &&
       target->canWriteRectangle())
      _method = OpRect;
   else if (source1->canReadScanline() &&
            source2->canReadScanline() &&
            target->canWriteScanline())
      _method = OpRow;
   else if (source1->canReadPixel() &&
            source2->canReadPixel() &&
            target->canWritePixel())
      _method = OpPixel;
   else
   {
      gDebug.msg("BinaryImageOperator::BinaryImageOperator():",0);
      gDebug.msg("   images are not compatible",0);
      _method = OpNone;
      return;
   }
   if ((source1->getNoBands() == source2->getNoBands()) &&
       (source2->getNoBands() == target->getNoBands()) &&
       (source1->getBitsPerBand() == source2->getBitsPerBand())&&
       (source2->getBitsPerBand() == target->getBitsPerBand())&&
       (source1->getImageData()->getPhotometric() ==
        source2->getImageData()->getPhotometric()) &&
       (source2->getImageData()->getPhotometric() ==
        target->getImageData()->getPhotometric()))
      _type = OpRaw;
       
   else
      _type = OpCanonical;
   if (_type == OpCanonical)
   {
      if (!((source1->getPhotometric()==source2->getPhotometric())&&
           (source2->getPhotometric()==target->getPhotometric())))
      {
         if (source1->canReadPixel() &&
             source2->canReadPixel() &&
             target->canWritePixel())
            _method = OpPixel;
         else
         {
            gDebug.msg("BinaryImageOperater::BinaryImageOperator():",0);
            gDebug.msg("   No compatible copy methods.",0);
            _method = OpNone;
            return;
         }
      }
   }
   if (gDebug.getDebugLevel() >= 3)
   {
      cerr << "BinaryImageOperator::BinaryImageOperator(): Method is: ";
      switch (_method)
      {
         case OpPixel:   cerr << " pixel.";
                         break;
         case OpRow:     cerr << " row.";
                         break;
         case OpRect:    cerr << " rect.";
                         break;
         case OpNone:    cerr << " none.";
                         break;
      }
      cerr << endl;
      cerr << "BinaryImageOperator::BinaryImageOperator(): Type is: ";
      switch (_type)
      {
         case OpRaw:        cerr << "raw.";
                            break;
         case OpCanonical:  cerr << "canonical.";
                            break;
      }
      cerr << endl;
   }
}

bool BinaryImageOperator::operate(void)
{
   bool ret;

   if (getMethod()==OpNone) return false;

   if (getType() == OpRaw)
   {
      switch (getMethod())
      {
         case OpPixel:   ret = operateRawByPixel();
                         break;
         case OpRow:     ret = operateRawByRow();
                         break;
         case OpRect:    ret = operateRawByRect();
                         break;
      }
   }
   else if (getType() == OpCanonical)
   {
      switch (getMethod())
      {
         case OpPixel:   ret = operateByPixel();
                         break;
         case OpRow:     ret = operateByRow();
                         break;
         case OpRect:    ret = operateByRect();
                         break;
      }
   }
   else
   {
      gDebug.msg("BinaryImageOperator::operate(): _type is not correct.",0);
      ret = false;
   }
   return ret;
}
 
Image* BinaryImageOperator::getSource1(void)
{
   return _fin1;
}

Image* BinaryImageOperator::getSource2(void)
{
   return _fin2;
}

Image* BinaryImageOperator::getTarget(void)
{
   return _fout;
}

BinaryImageOperator::OpMethods BinaryImageOperator::getMethod(void)
{
   return _method;
}

BinaryImageOperator::OpTypes BinaryImageOperator::getType(void)
{
   return _type;
}

bool BinaryImageOperator::copyPalletteIfNeeded(void)
{
   bool ret = false;

   if (getSource1()->hasPallette()&&
       getSource2()->hasPallette()&&
       getTarget()->hasPallette())
   {
      Pallette *pal1 = getSource1()->getPallette();
      Pallette *pal2 = getSource2()->getPallette();
      if (comparePallettes(pal1,pal2,
                           getSource1()->getImageData()->getPhotometric()))
      {
         getTarget()->setPallette(pal1);
         ret = true;
      }
      delete pal1;
      delete pal2;
   }
   if (!getSource1()->hasPallette() &&
       !getSource2()->hasPallette() &&
       !getTarget()->hasPallette())
      ret = true;

   return ret;
}

bool BinaryImageOperator::comparePallettes(Pallette *p1, Pallette *p2,
                                           unsigned char photo)
{
   int i;
   bool ret=true;

   if (p1->getNoEntries()!=p2->getNoEntries())
      return false; 
   if (photo==DRG_RGB)
   {
      RGBPallette* pal1 = (RGBPallette *)p1;
      RGBPallette* pal2 = (RGBPallette *)p1;
      for (i=0;(i<pal1->getNoEntries())&&(ret);i++)
      {
         if ((*pal1)[i].getRed() != (*pal2)[i].getRed())
            ret = false;
         if ((*pal1)[i].getGreen() != (*pal2)[i].getGreen())
            ret = false;
         if ((*pal1)[i].getBlue() != (*pal2)[i].getBlue())
            ret = false;
      }
   }
   else if (photo == DRG_GREY)
   {
      GreyPallette* pal1 = (GreyPallette *)p1;
      GreyPallette* pal2 = (GreyPallette *)p2;
      for (i=0;(i<pal1->getNoEntries())&&(ret);i++)
      {
         if ((*pal1)[i].getGrey() != (*pal2)[i].getGrey())
            ret = false;
      }
   }
   return ret;
}
       
      

