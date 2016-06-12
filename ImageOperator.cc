#include "ImageOp/ImageOperator.h"
#include "Image/Image.h"

ImageOperator::ImageOperator()
{
   statusIndicator = false;
}

void ImageOperator::giveStatus(void)
{
   statusIndicator = true;
}

void ImageOperator::noStatus(void)
{
   statusIndicator = false;
}

void ImageOperator::toggleStatus(void)
{
   statusIndicator = !statusIndicator;
}

bool ImageOperator::getStatus(void)
{
   return statusIndicator;
}
