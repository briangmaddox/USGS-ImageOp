#include "ImageOp/ToPixelFilter.h"
#include "Image/ImageCommon.h"
#include "Image/ImageDebug.h"

ToPixelFilter::ToPixelFilter(int width, int height, unsigned char photo)
{
   int i,j;

   _width       = width;
   _height      = height;
   _photometric = photo;

   if (gDebug.getDebugLevel()>3)
   {
      cerr << "_width  = " << _width << endl;
      cerr << "_height = " << _height << endl;
      cerr << "_photometric = " << (int) _photometric << endl;
   }
   _kernel = new (int *)[_height];
   for (i=0;i<_height;i++)
   {
       _kernel[i] = new int[_width];
       for (j=0;j<_width;j++)
          _kernel[i][j] = 0;
   }
   _divisor = 0;
}


ToPixelFilter::~ToPixelFilter()
{
   int i;
   for (i=0;i<_height;i++)
       delete[] _kernel[i];
   delete[] _kernel;
}

unsigned char* ToPixelFilter::getFillPixel(void)
{
   return NULL;
}

unsigned char *ToPixelFilter::getPixel(unsigned char *rect)
{
   int i,j;
   unsigned char *ret;
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
             rtot += rect[3*(j*getWidth()+i)]   * _kernel[j][i];
             gtot += rect[3*(j*getWidth()+i)+1] * _kernel[j][i];
             btot += rect[3*(j*getWidth()+i)+2] * _kernel[j][i];
          }
      }
      ret = new unsigned char[3];
      if (_divisor!=0)
      {
         ret[0] = rtot/_divisor;
         ret[1] = gtot/_divisor;
         ret[2] = btot/_divisor;
      }
      else
      {
         ret[0] = 255;
         ret[1] = 255;
         ret[2] = 255;
      }
   }
   else if (getPhotometric()==DRG_GREY)
   {
      int gtot;
      gtot = 0;
      for (j=0;j<getHeight();j++)
      {
         for (i=0;i<getWidth();i++)
            gtot += rect[j*getWidth()+i] * _kernel[j][i];
      }
      ret = new unsigned char;
      if (_divisor != 0)
         *ret = gtot/_divisor;
      else
         *ret = 255;
   }
   return ret;
}

int ToPixelFilter::getWidth()
{
   return _width;
}

int ToPixelFilter::getHeight()
{
   return _height;
}

int** ToPixelFilter::getKernel()
{
   return _kernel;
}

unsigned char ToPixelFilter::getPhotometric()
{
   return _photometric;
}
 
void ToPixelFilter::setPhotometric(unsigned char p)
{
   _photometric = p;
}

void ToPixelFilter::setKernel(int **k)
{
   int i,j;

   _divisor = 0;
   for (j=0;j<getHeight();j++)
      for (i=0;i<getWidth();i++)
      {
         _kernel[j][i] = k[j][i];
         _divisor += k[j][i]; 
         if (gDebug.getDebugLevel()>3)
         {
            cerr << "Kernel[" << j << "][" << i << "] = " << _kernel[j][i];
            cerr << endl;
         }
      }
}

