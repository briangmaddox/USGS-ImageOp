#include "ImageOp/UnaryImageOperator.h"
#include "Image/Image.h"
#include "Image/ImageDebug.h"

UnaryImageOperator::UnaryImageOperator(Image *source, Image *target)
{
   _fin  = source;
   _fout = target;
   if (source->canReadRectangle() && target->canWriteRectangle())
      _method = OpRect;
   else if (source->canReadScanline() && target->canWriteScanline())
      _method = OpRow;
   else if (source->canReadPixel() && target->canWritePixel())
      _method = OpPixel;
   else
   {
      gDebug.msg("UnaryImageOperator::UnaryImageOperator(): images",0);
      gDebug.msg("are not compatible",0);
      _method = OpNone;
      return;
   }
   if ((source->getNoBands() == target->getNoBands())&&
       (source->getBitsPerBand() == target->getBitsPerBand())&&
       (source->getImageData()->getPhotometric() ==
        target->getImageData()->getPhotometric()))
      _type = OpRaw;
   else
      _type = OpCanonical;   
   if (_type == OpCanonical)
   {
      if (source->getPhotometric() != target->getPhotometric())
      {
         if (source->canReadPixel() && target->canWritePixel())
            _method = OpPixel;
         else
         {
            gDebug.msg("UnaryImageOperator::UnaryImageOperator():",0);
            gDebug.msg("   No compatible copy methods.",0);
            _method = OpNone;
            return;
         }
      }
   }
   if (gDebug.getDebugLevel() >= 3)
   {
      cerr << "UnaryImageOperator::UnaryImageOperator(): Method is: ";
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
      cerr << "UnaryImageOperator::UnaryImageOperator(): Type is: ";
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

Image* UnaryImageOperator::getSource(void)
{
   return _fin;
}

Image* UnaryImageOperator::getTarget(void)
{
   return _fout;
}

UnaryImageOperator::OpMethods UnaryImageOperator::getMethod(void)
{
   return _method;
}

UnaryImageOperator::OpTypes UnaryImageOperator::getType(void)
{
   return _type;
}

void UnaryImageOperator::setMethod(UnaryImageOperator::OpMethods m)
{
   _method = m;
}

void UnaryImageOperator::setType(UnaryImageOperator::OpTypes t)
{
   _type = t;
}

bool UnaryImageOperator::copyPalletteIfNeeded(void)
{
   bool ret = false;

   if (getSource()->hasPallette()&&getTarget()->hasPallette())
   {
      Pallette *pal = getSource()->getPallette();
      getTarget()->setPallette(pal);
      delete pal;
      ret = true;
   }
   if (!getSource()->hasPallette()&&!getTarget()->hasPallette())
      ret = true;
   return ret;
}


bool UnaryImageOperator::operate(void)
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
      gDebug.msg("UnaryImageOperator::operate(): _type is not correct.",0);
      ret = false;
   }
   return ret;
}
