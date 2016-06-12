#include "ImageOp/FilterImage.h"
#include "Image/Image.h"
#include "Image/ImageDebug.h"
#include "Image/RGBPixel.h"
#include "Image/GreyPixel.h"
#include "FrodoLib/SpinnerThingy.h"

#define DRECTHEIGHT 64


FilterImage::FilterImage(Image *source, Image *target, ToPixelFilter *filter):
UnaryImageOperator(source,target)
{
   _filter = filter;
   if (source->getWidth() != target->getWidth())
   {
      gDebug.msg("FilterImage::FilterImage(): Images must be same size.",0);
      _canFilter = false;
      return;
   }
   if (source->getHeight() != target->getHeight())
   {
      gDebug.msg("FilterImage::FilterImage(): Images must be same size.",0);
      _canFilter = false;
      return;
   }
   setType(OpCanonical);
   // This is set for testing only.
   setMethod(OpRow);
   _canFilter = (getMethod()!=OpNone);

/*
   if ((target->getFileType()==source->getFileType())&&
       ((target->getFileType()==DRG_RGB)||
        (target->getFileType()==DRG_GREY)))
      setType(OpRaw);
   else
      setType(OpCanonical);
*/
     
   if ((target->getFileType()!=IMAGE_RGB)&&
       (target->getFileType()!=IMAGE_GREY))
   {
      gDebug.msg("FilterImage::FilterImage(): possibly incorrect",0);
      gDebug.msg("   output format.  Unwanted quantization may occur",0);
   }
}

bool FilterImage::operateRawByPixel(void)
{
   return false;
}

bool FilterImage::operateRawByRow(void)
{
   return false;
}

bool FilterImage::operateRawByRect(void)
{
   return false;
}

bool FilterImage::operateByPixel(void)
{
   SpinnerThingy *thingy;
   bool ret;
   int i,j,k,l;
   int filter_row_start, filter_row_end;
   int filter_col_start, filter_col_end;
   int next_row_needed;
   unsigned char *rect_to_filter;
   unsigned char *filter_result;

   if (!_canFilter) return false;

   if (getStatus())
   {
      thingy = new SpinnerThingy;
      cout << "Filtering " << getSource()->getHeight() << " scanlines";
      cout << endl;
   }
   filter_row_start = (_filter->getHeight()-1)/2;
   filter_row_end   = (getTarget()->getHeight()-1)-(_filter->getHeight()-1)/2;
   filter_col_start = (_filter->getWidth()-1)/2;
   filter_col_end   = (getTarget()->getWidth()-1)-(_filter->getWidth()-1)/2;
   if ((getSource()->getPhotometric()==DRG_RGB)&&
       (getTarget()->getPhotometric()==DRG_RGB))
   {
      RGBPixel *out_row = new RGBPixel[getTarget()->getWidth()];
      RGBPixel **in_rows = new (RGBPixel *)[_filter->getHeight()];
      RGBPixel *tmp;

      _filter->setPhotometric(DRG_RGB);
      rect_to_filter = 
           new unsigned char[3*_filter->getHeight()*_filter->getWidth()];
      
      for (j=0;j<_filter->getHeight();j++)
      {
         in_rows[j] = new RGBPixel[getSource()->getWidth()]; 
         for (k=0;k<getSource()->getWidth();k++)
         {
            tmp = (RGBPixel *)getSource()->getPixel(k,j);
            in_rows[j][k].setRed(tmp->getRed());
            in_rows[j][k].setGreen(tmp->getGreen());
            in_rows[j][k].setBlue(tmp->getBlue());
            delete tmp;
         }
         if (j<filter_row_start)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               for (k=0;k<getSource()->getWidth();k++)
               {
                  out_row[k].setRed(filter_result[0]);
                  out_row[k].setGreen(filter_result[1]);
                  out_row[k].setBlue(filter_result[2]);
               }
               delete[] filter_result;
               for (k=0;k<getTarget()->getWidth();k++)
                  getTarget()->putPixel(&out_row[k],k,j); 
            }
            else    // replicate the source pixels
               for (k=0;k<getTarget()->getWidth();k++)
                  getTarget()->putPixel(&in_rows[j][k],k,j);
         }
      } 
      next_row_needed = j;
      for (j=filter_row_start;j<=filter_row_end;j++)
      {
         if (getStatus())
            if ((j%4)==0)
              thingy->update(j);      
         for (i=0;i<filter_col_start;i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setRed(filter_result[0]);
               out_row[i].setGreen(filter_result[1]);
               out_row[i].setBlue(filter_result[2]);
               delete[] filter_result;
            }
            else        // replicate the source pixels
            {
               out_row[i].setRed(in_rows[filter_row_start][i].getRed());
               out_row[i].setGreen(in_rows[filter_row_start][i].getGreen());
               out_row[i].setBlue(in_rows[filter_row_start][i].getBlue());
            }
         }
         for (i=filter_col_start;i<=filter_col_end;i++)
         {
            for (l=0;l<_filter->getHeight();l++)
            {
               for (k=0;k<_filter->getWidth();k++)
               {
                  rect_to_filter[3*(l*_filter->getWidth()+k)]   =
                     in_rows[l][i+(k-filter_col_start)].getRed();
                  rect_to_filter[3*(l*_filter->getWidth()+k)+1] =
                     in_rows[l][i+(k-filter_col_start)].getGreen();
                  rect_to_filter[3*(l*_filter->getWidth()+k)+2] =
                     in_rows[l][i+(k-filter_col_start)].getBlue();
               }
            }
            filter_result = _filter->getPixel(rect_to_filter);
            out_row[i].setRed(filter_result[0]);
            out_row[i].setGreen(filter_result[1]);
            out_row[i].setBlue(filter_result[2]);
            delete[] filter_result;
         }
         for (i=filter_col_end+1;i<getTarget()->getWidth();i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setRed(filter_result[0]);
               out_row[i].setGreen(filter_result[1]);
               out_row[i].setBlue(filter_result[2]);
               delete[] filter_result;
            }
            else         // Replicate the source pixels
            {
               out_row[i].setRed(in_rows[filter_row_start][i].getRed());
               out_row[i].setGreen(in_rows[filter_row_start][i].getGreen());
               out_row[i].setBlue(in_rows[filter_row_start][i].getBlue());
            }
         }
         for (k=0;k<getTarget()->getWidth();k++)
            getTarget()->putPixel(&out_row[k],k,j);
         if (next_row_needed < getSource()->getHeight())
         {
            delete[] in_rows[0];
            for (i=1;i<_filter->getHeight();i++)
               in_rows[i-1] = in_rows[i];
            in_rows[i-1] = new RGBPixel[getSource()->getWidth()];
            for (k=0;k<getSource()->getWidth();k++)
            {
               tmp = (RGBPixel *)getSource()->getPixel(k,next_row_needed);
               in_rows[i-1][k].setRed(tmp->getRed());
               in_rows[i-1][k].setGreen(tmp->getGreen());
               in_rows[i-1][k].setBlue(tmp->getBlue());
               delete tmp;
            }
            next_row_needed++;
         }
      }
      k = ((_filter->getHeight()-1)/2) + 1;
      for (j=filter_row_end+1;j<getTarget()->getHeight();j++)
      {
         if ((filter_result=_filter->getFillPixel())!=NULL)
         {
            for (l=0;l<getSource()->getWidth();l++)
            {
               out_row[l].setRed(filter_result[0]);
               out_row[l].setGreen(filter_result[1]);
               out_row[l].setBlue(filter_result[2]);
            }
            delete[] filter_result;
            for (l=0;l<getTarget()->getWidth();l++)
               getTarget()->putPixel(&out_row[l],l,j);
         }
         else
            for (l=0;l<getTarget()->getWidth();l++)
               getTarget()->putPixel(&in_rows[k][l],l,j);
         k++;
      }
      for (i=0;i<_filter->getHeight();i++)
         delete[] in_rows[i];
      delete[] in_rows;
      delete[] out_row;
      delete[] rect_to_filter;
      ret = (getSource()->good()&&getTarget()->good());
   }
   else if ((getSource()->getPhotometric()==DRG_GREY)&&
            (getTarget()->getPhotometric()==DRG_GREY))
   {

      GreyPixel *out_row = new GreyPixel[getTarget()->getWidth()];
      GreyPixel **in_rows = new (GreyPixel *)[_filter->getHeight()];
      GreyPixel *tmp;

      _filter->setPhotometric(DRG_GREY);
      rect_to_filter = 
           new unsigned char[_filter->getHeight()*_filter->getWidth()];
      
      for (j=0;j<_filter->getHeight();j++)
      {
         in_rows[j] = new GreyPixel[getSource()->getWidth()]; 
         for (k=0;k<getSource()->getWidth();k++)
         {
            tmp = (GreyPixel *)getSource()->getPixel(k,j);
            in_rows[j][k].setGrey(tmp->getGrey());
            delete tmp;
         }
         if (j<filter_row_start)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               for (k=0;k<getSource()->getWidth();k++)
                  out_row[k].setGrey(*filter_result);
               delete filter_result;
               for (k=0;k<getTarget()->getWidth();k++)
                  getTarget()->putPixel(&out_row[k],k,j); 
            }
            else    // replicate the source pixels
               for (k=0;k<getTarget()->getWidth();k++)
                  getTarget()->putPixel(&in_rows[j][k],k,j);
         }
      } 
      next_row_needed = j;
      for (j=filter_row_start;j<=filter_row_end;j++)
      {
         if (getStatus())
            if ((j%4)==0)
              thingy->update(j);      
         for (i=0;i<filter_col_start;i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setGrey(*filter_result);
               delete filter_result;
            }
            else        // replicate the source pixels
               out_row[i].setGrey(in_rows[filter_row_start][i].getGrey());
         }
         for (i=filter_col_start;i<=filter_col_end;i++)
         {
            for (l=0;l<_filter->getHeight();l++)
               for (k=0;k<_filter->getWidth();k++)
                  rect_to_filter[l*_filter->getWidth()+k]   =
                     in_rows[l][i+(k-filter_col_start)].getGrey();
            filter_result = _filter->getPixel(rect_to_filter);
            out_row[i].setGrey(*filter_result);
            delete filter_result;
         }
         for (i=filter_col_end+1;i<getTarget()->getWidth();i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setGrey(*filter_result);
               delete filter_result;
            }
            else         // Replicate the source pixels
               out_row[i].setGrey(in_rows[filter_row_start][i].getGrey());
         }
         for (k=0;k<getTarget()->getWidth();k++)
            getTarget()->putPixel(&out_row[k],k,j);
         if (next_row_needed < getSource()->getHeight())
         {
            delete[] in_rows[0];
            for (i=1;i<_filter->getHeight();i++)
               in_rows[i-1] = in_rows[i];
            in_rows[i-1] = new GreyPixel[getSource()->getWidth()];
            for (k=0;k<getSource()->getWidth();k++)
            {
               tmp = (GreyPixel *)getSource()->getPixel(k,next_row_needed);
               in_rows[i-1][k].setGrey(tmp->getGrey());
               delete tmp;
            }
            next_row_needed++;
         }
      }
      k = ((_filter->getHeight()-1)/2) + 1;
      for (j=filter_row_end+1;j<getTarget()->getHeight();j++)
      {
         if ((filter_result=_filter->getFillPixel())!=NULL)
         {
            for (l=0;l<getSource()->getWidth();l++)
               out_row[l].setGrey(*filter_result);
            delete filter_result;
            for (l=0;l<getTarget()->getWidth();l++)
               getTarget()->putPixel(&out_row[l],l,j);
         }
         else
            for (l=0;l<getTarget()->getWidth();l++)
               getTarget()->putPixel(&in_rows[k][l],l,j);
         k++;
      }
      for (i=0;i<_filter->getHeight();i++)
         delete[] in_rows[i];
      delete[] in_rows;
      delete[] out_row;
      delete[] rect_to_filter;
      ret = (getSource()->good()&&getTarget()->good());
   }
   else
   {
      // either don't do, or try to figure out another way.
      gDebug.msg("FilterImage::operateByPixel(): sorry, but filtering",0);
      gDebug.msg("   on differing colorspaces not implemented.",0);
      ret = false;
   }
   if (getStatus())
   {
      thingy->done("Done.");
      delete[] thingy;
   }
   return ret;
}

bool FilterImage::operateByRow(void)
{
   bool ret;
   SpinnerThingy *thingy;
   int i,j,k,l;
   int filter_row_start, filter_row_end;
   int filter_col_start, filter_col_end;
   int next_row_needed;
   unsigned char *rect_to_filter;
   unsigned char *filter_result;

   if (!_canFilter) return false;

   if (getStatus())
   {
      thingy = new SpinnerThingy;
      cout << "Filtering " << getSource()->getHeight() << " scanlines.";
      cout << endl;
   }
   filter_row_start = (_filter->getHeight()-1)/2;
   filter_row_end   = (getTarget()->getHeight()-1)-(_filter->getHeight()-1)/2;
   filter_col_start = (_filter->getWidth()-1)/2;
   filter_col_end   = (getTarget()->getWidth()-1)-(_filter->getWidth()-1)/2;
   if ((getSource()->getPhotometric()==DRG_RGB)&&
       (getTarget()->getPhotometric()==DRG_RGB))
   {
      RGBPixel *out_row = new RGBPixel[getTarget()->getWidth()];
      RGBPixel **in_rows = new (RGBPixel *)[_filter->getHeight()];

      _filter->setPhotometric(DRG_RGB);
      rect_to_filter = 
           new unsigned char[3*_filter->getHeight()*_filter->getWidth()];
      
      for (j=0;j<_filter->getHeight();j++)
      {
         in_rows[j] = (RGBPixel *) getSource()->getScanline(j);
         if (j<filter_row_start)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               for (k=0;k<getSource()->getWidth();k++)
               {
                  out_row[k].setRed(filter_result[0]);
                  out_row[k].setGreen(filter_result[1]);
                  out_row[k].setBlue(filter_result[2]);
               }
               delete[] filter_result;
               getTarget()->putScanline(out_row,j);
            }
            else    // replicate the source pixels
               getTarget()->putScanline(in_rows[j],j);
         }
      } 
      next_row_needed = j;
      for (j=filter_row_start;j<=filter_row_end;j++)
      {
         if (getStatus())
            if ((j%4)==0)
              thingy->update(j);      
         for (i=0;i<filter_col_start;i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setRed(filter_result[0]);
               out_row[i].setGreen(filter_result[1]);
               out_row[i].setBlue(filter_result[2]);
               delete[] filter_result;
            }
            else        // replicate the source pixels
            {
               out_row[i].setRed(in_rows[filter_row_start][i].getRed());
               out_row[i].setGreen(in_rows[filter_row_start][i].getGreen());
               out_row[i].setBlue(in_rows[filter_row_start][i].getBlue());
            }
         }
         for (i=filter_col_start;i<=filter_col_end;i++)
         {
            for (l=0;l<_filter->getHeight();l++)
            {
               for (k=0;k<_filter->getWidth();k++)
               {
                  rect_to_filter[3*(l*_filter->getWidth()+k)]   =
                     in_rows[l][i+(k-filter_col_start)].getRed();
                  rect_to_filter[3*(l*_filter->getWidth()+k)+1] =
                     in_rows[l][i+(k-filter_col_start)].getGreen();
                  rect_to_filter[3*(l*_filter->getWidth()+k)+2] =
                     in_rows[l][i+(k-filter_col_start)].getBlue();
               }
            }
            filter_result = _filter->getPixel(rect_to_filter);
            out_row[i].setRed(filter_result[0]);
            out_row[i].setGreen(filter_result[1]);
            out_row[i].setBlue(filter_result[2]);
            delete[] filter_result;
         }
         for (i=filter_col_end+1;i<getTarget()->getWidth();i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setRed(filter_result[0]);
               out_row[i].setGreen(filter_result[1]);
               out_row[i].setBlue(filter_result[2]);
               delete[] filter_result;
            }
            else         // Replicate the source pixels
            {
               out_row[i].setRed(in_rows[filter_row_start][i].getRed());
               out_row[i].setGreen(in_rows[filter_row_start][i].getGreen());
               out_row[i].setBlue(in_rows[filter_row_start][i].getBlue());
            }
         }
         getTarget()->putScanline(out_row,j);
         if (next_row_needed < getSource()->getHeight())
         {
            delete[] in_rows[0];
            for (i=1;i<_filter->getHeight();i++)
               in_rows[i-1] = in_rows[i];
            in_rows[i-1] = (RGBPixel *)getSource()->getScanline(next_row_needed);
            next_row_needed++;
         }
      }
      k = ((_filter->getHeight()-1)/2) + 1;
      for (j=filter_row_end+1;j<getTarget()->getHeight();j++)
      {
         if ((filter_result=_filter->getFillPixel())!=NULL)
         {
            for (l=0;l<getSource()->getWidth();l++)
            {
               out_row[l].setRed(filter_result[0]);
               out_row[l].setGreen(filter_result[1]);
               out_row[l].setBlue(filter_result[2]);
            }
            delete[] filter_result;
            getTarget()->putScanline(out_row,j);
         }
         else
            getTarget()->putScanline(in_rows[k],j);
         k++;
      }
      for (i=0;i<_filter->getHeight();i++)
         delete[] in_rows[i];
      delete[] in_rows;
      delete[] out_row;
      delete[] rect_to_filter;
      ret = (getSource()->good()&&getTarget()->good());
   }
   else if ((getSource()->getPhotometric()==DRG_GREY)&&
            (getTarget()->getPhotometric()==DRG_GREY))
   {
      GreyPixel *out_row = new GreyPixel[getTarget()->getWidth()];
      GreyPixel **in_rows = new (GreyPixel *)[_filter->getHeight()];

      _filter->setPhotometric(DRG_GREY);
      rect_to_filter =
           new unsigned char[_filter->getHeight()*_filter->getWidth()];

      for (j=0;j<_filter->getHeight();j++)
      {
         in_rows[j] = (GreyPixel *) getSource()->getScanline(j);
         if (j<filter_row_start)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               for (k=0;k<getSource()->getWidth();k++)
                  out_row[k].setGrey(*filter_result);
               delete filter_result;
               getTarget()->putScanline(out_row,j);
            }
            else    // replicate the source pixels
               getTarget()->putScanline(in_rows[j],j);
         }
      }
      next_row_needed = j;
      for (j=filter_row_start;j<=filter_row_end;j++)
      {
         if (getStatus())
            if ((j%4)==0)
              thingy->update(j);
         for (i=0;i<filter_col_start;i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setGrey(*filter_result);
               delete filter_result;
            }
            else        // replicate the source pixels
               out_row[i].setGrey(in_rows[filter_row_start][i].getGrey());
         }
         for (i=filter_col_start;i<=filter_col_end;i++)
         {
            for (l=0;l<_filter->getHeight();l++)
               for (k=0;k<_filter->getWidth();k++)
                  rect_to_filter[l*_filter->getWidth()+k]   =
                     in_rows[l][i+(k-filter_col_start)].getGrey();
            filter_result = _filter->getPixel(rect_to_filter);
            out_row[i].setGrey(*filter_result);
            delete filter_result;
         }
         for (i=filter_col_end+1;i<getTarget()->getWidth();i++)
         {
            if ((filter_result=_filter->getFillPixel())!=NULL)
            {
               out_row[i].setGrey(*filter_result);
               delete filter_result;
            }
            else         // Replicate the source pixels
               out_row[i].setGrey(in_rows[filter_row_start][i].getGrey());
         }
         getTarget()->putScanline(out_row,j);
         if (next_row_needed < getSource()->getHeight())
         {
            delete[] in_rows[0];
            for (i=1;i<_filter->getHeight();i++)
               in_rows[i-1] = in_rows[i];
            in_rows[i-1] = (GreyPixel *)getSource()->getScanline(next_row_needed);
            next_row_needed++;
         }
      }
      k = ((_filter->getHeight()-1)/2) + 1;
      for (j=filter_row_end+1;j<getTarget()->getHeight();j++)
      {
         if ((filter_result=_filter->getFillPixel())!=NULL)
         {
            for (l=0;l<getSource()->getWidth();l++)
               out_row[l].setGrey(*filter_result);
            delete filter_result;
            getTarget()->putScanline(out_row,j);
         }
         else
            getTarget()->putScanline(in_rows[k],j);
         k++;
      }
      for (i=0;i<_filter->getHeight();i++)
         delete[] in_rows[i];
      delete[] in_rows;
      delete[] out_row;
      ret = (getSource()->good()&&getTarget()->good());
   }
   else
   {
      gDebug.msg("FilterImage::operateByRow(): Images must have same",0);
      gDebug.msg("   photometric interpretation.",0);
      ret = false;
   }
   if (getStatus())
   {
      thingy->done("Done.");
      delete thingy;
   }
   return ret;
}

bool FilterImage::operateByRect(void)
{
   bool ret;
   int i,j;

   if ((getSource()->getPhotometric()==DRG_RGB)&&
       (getTarget()->getPhotometric()==DRG_RGB))
   {
      // do RGB
   }
   else if ((getSource()->getPhotometric()==DRG_GREY)&&
            (getTarget()->getPhotometric()==DRG_GREY))
   {
      // do Grey
   }
   else
   {
      // error...can't do this.
      ret = false;
   }
   return ret;
}


