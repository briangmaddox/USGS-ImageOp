#include "ImageOp/BlurFilter.h"
#include "Image/ImageDebug.h"
#include "Image/ImageCommon.h"

BlurFilter::BlurFilter(unsigned char photo,int size):
ToPixelFilter(size,size,photo)
{
   int i,j;
   int **kernel = new (int *)[size];
   for (i=0;i<size;i++)
   { 
      kernel[i] = new int[size];
      for(j=0;j<size;j++)
         kernel[i][j] = 1;
   }

   setKernel(kernel);
   for (i=0;i<size;i++)
      delete[] kernel[i];
   delete[] kernel;
}

unsigned char* BlurFilter::getFillPixel(void)
{
   return NULL;
} 
