#include "ImageOp/SubtractNeighborFilter.h"
#include "Image/ImageDebug.h"
#include "Image/ImageCommon.h"

SubtractNeighborFilter::SubtractNeighborFilter(unsigned char photo,
                                               int direction):
ToPixelFilter(3,3,photo)
{
   int i,j;
   int **kernel = new (int *)[3];
   for (i=0;i<3;i++)
   { 
      kernel[i] = new int[3];
      for(j=0;j<3;j++)
         kernel[i][j] = 0;
   }

   kernel[1][1] = 1;
   switch (direction)
   {
      case 0:   kernel[0][1] = -1;
                break; 
      case 1:   kernel[0][2] = -1;
                break; 
      case 2:   kernel[1][2] = -1;
                break; 
      case 3:   kernel[2][2] = -1;
                break; 
      case 4:   kernel[2][1] = -1;
                break; 
      case 5:   kernel[2][0] = -1;
                break; 
      case 6:   kernel[1][0] = -1;
                break; 
      case 7:   kernel[0][0] = -1;
                break; 
      default:  
         gDebug.msg("SubtractNeighborFilter::SubtractNeighborFilter():",0);
         gDebug.msg("   incorrect direction.",0);
         break;
   }
   setKernel(kernel);
   for (i=0;i<3;i++)
      delete[] kernel[i];
   delete[] kernel;
}

unsigned char* SubtractNeighborFilter::getFillPixel(void)
{
   unsigned char *ret;
   
   if (getPhotometric()==DRG_RGB)
   {
      ret = new unsigned char[3];
      ret[0] = (unsigned char) 128;
      ret[1] = (unsigned char) 128;
      ret[2] = (unsigned char) 128;
   }
   else if (getPhotometric()==DRG_GREY)
   {
      ret = new unsigned char;
      *ret = (unsigned char) 128;
   }
   else
   {
      gDebug.msg("SubtractNeighborFilter::getFillPixel():",0);
      gDebug.msg("   unknown colorspace.",0);
      ret = NULL;
   }
   return ret;
}
      
unsigned char *SubtractNeighborFilter::getPixel(unsigned char *rect)
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
      ret[0] = (rtot+256)/2;
      ret[1] = (gtot+256)/2;
      ret[2] = (btot+256)/2;
   }
   else if (getPhotometric()==DRG_GREY)
   {
      int gtot;
      gtot=0;
      for (j=0;j<getHeight();j++)
      {
         for (i=0;i<getWidth();i++)
         {
            gtot += rect[j*getWidth()+i] * k[j][i];
         }
      }
      ret = new unsigned char;
      *ret = (gtot+256)/2;
   }
   else
   {
      gDebug.msg("SubtractNeighborFilter::getPixel(): unknown colorspace.",0);
      ret = NULL;
   }
   return ret;
}
