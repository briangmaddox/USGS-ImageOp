#include "ImageOp/LaplacianFilter.h"
#include "Image/ImageDebug.h"
#include "Image/ImageCommon.h"

LaplacianFilter::LaplacianFilter(unsigned char photo):
ToPixelFilter(3,3,photo)
{
   int i,j;
   int **kernel = new (int *)[3];
   for (i=0;i<3;i++)
   { 
      kernel[i] = new int[3];
      for(j=0;j<3;j++)
         kernel[i][j] = -1;
   }

   kernel[1][1] = 8;

   setKernel(kernel);
   for (i=0;i<3;i++)
      delete[] kernel[i];
   delete[] kernel;
}

unsigned char* LaplacianFilter::getPixel(unsigned char *rect)
{
   int i,j;
   unsigned char *ret;
   int **k = getKernel();

   if (getPhotometric()==DRG_RGB)
   {
      int rtot,gtot,btot;
      rtot=0;
      gtot=0;
      btot=0;
      for (j=0;j<getHeight();j++)
      {
          for (i=0;i<getWidth();i++)
          {
             rtot += rect[3*(j*getWidth()+i)]   * k[j][i];
             gtot += rect[3*(j*getWidth()+i)+1] * k[j][i];
             btot += rect[3*(j*getWidth()+i)+2] * k[j][i];
          }
      }
      ret = new unsigned char[3];
      ret[0] = (rtot/8);
      ret[1] = (gtot/8);
      ret[2] = (btot/8);
   }
   else if (getPhotometric()==DRG_GREY)
   {
      int gtot;
      int gval;
      gtot=0;
      for (j=0;j<getHeight();j++)
      {
         for (i=0;i<getWidth();i++)
         {
            gtot += rect[j*getWidth()+i] * k[j][i];
         }
      }
      gval = rect[(getHeight()/2)*getWidth()+getWidth()/2];
      gtot = gtot/16;
      gval = gval - gtot;
      ret = new unsigned char;
      if (gval > 255) 
         *ret = 255;
      else if (gval < 0)
         *ret = 0;
      else
         *ret = 0;
   }
   else
   {
      gDebug.msg("LaplacianFilter::getPixel(): unknown colorspace.",0);
      ret = NULL;
   }
   return ret;
}

unsigned char* LaplacianFilter::getFillPixel(void)
{
   unsigned char *ret;
   if (getPhotometric()==DRG_RGB)
   {
      ret = new unsigned char[3];
      ret[0] = 128;
      ret[1] = 128;
      ret[2] = 128;
   }
   else if (getPhotometric()==DRG_GREY)
   {
      ret = new unsigned char;
      *ret = 128;
   }
   else
      ret = NULL;
   return ret;
} 

