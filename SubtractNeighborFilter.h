#ifndef _SUBTRACTNEIGHBORFILTER_H_
#define _SUBTRACTNEIGHBORFILTER_H_

#include "ImageOp/ToPixelFilter.h"

class SubtractNeighborFilter : public ToPixelFilter
{
 public:
   SubtractNeighborFilter(unsigned char photo, int direction);
   virtual ~SubtractNeighborFilter() {};

   virtual unsigned char* getPixel(unsigned char *);
   virtual unsigned char* getFillPixel(void);
};

#endif
