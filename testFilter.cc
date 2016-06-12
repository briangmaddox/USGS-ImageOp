#include "ImageOp/FilterImage.h"
#include "Image/RGBImage.h"
#include "Image/GreyImage.h"
#include "Image/ImageDebug.h"
#include "ImageOp/LaplacianFilter.h"
#include <iostream.h>

int main(int argc, char **argv)
{
   GreyImage *fin;
   GreyImage *fout;
   FilterImage *filterer;
   LaplacianFilter *filter;

   gDebug.setDebugLevel(4);
   fin = new GreyImage(argv[1]);
   if (!fin->good())
   {
      cerr << "Can't open " << argv[1] << endl;
      exit(42);
   }
   fout = new GreyImage(argv[2],fin->getWidth(),fin->getHeight());
   if (!fout->good())
   {
      cerr << "Can't open " << argv[2] << endl;
      exit(42);
   }
   filter = new LaplacianFilter(DRG_GREY);
   filterer = new FilterImage(fin,fout,filter);
   filterer->giveStatus();
   if (filterer->operate())
      cout << "Filter successful." << endl;
   else
      cout << "Filter botched." << endl;
   delete filter;
   delete filterer;
   delete fin;
   delete fout;
}
   
