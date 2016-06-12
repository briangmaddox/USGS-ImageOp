#ifndef _TOPIXELFILTER_H_
#define _TOPIXELFILTER_H_


class ToPixelFilter
{
   int** _kernel;
   int   _width;
   int   _height;
   int   _divisor;
   unsigned char _photometric;

 public:
   ToPixelFilter(int width, int height, unsigned char photo);
   virtual ~ToPixelFilter();
   virtual unsigned char* getPixel(unsigned char* rect); 
                       // Wants raw RGB or Grey
                       // Returns same
   virtual unsigned char* getFillPixel(void);
   int getWidth(void); 
   int getHeight(void);
   unsigned char getPhotometric(void);
   void setPhotometric(unsigned char);

 protected:
   int **getKernel(void);
   void  setKernel(int **);
};
#endif
