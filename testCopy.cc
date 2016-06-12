#include "ImageOp/CopyImage.h"
#include "Image/RGBImage.h"
#include "Image/ImageDebug.h"
#include <iostream.h>

int main(int argc, char **argv)
{
   RGBImage *fin;
   RGBImage *fout;
   CopyImage *copier;

   gDebug.setDebugLevel(4);
   fin = new RGBImage(argv[1]);
   if (!fin->good())
   {
      cerr << "Can't open " << argv[1] << endl;
      exit(42);
   }
   fout = new RGBImage(argv[2],fin->getWidth(),fin->getHeight());
   if (!fout->good())
   {
      cerr << "Can't open " << argv[2] << endl;
      exit(42);
   }
   copier = new CopyImage(fin,fout);
   copier->giveStatus();
   if (copier->operate())
      cout << "Copy successful." << endl;
   else
      cout << "Copy botched." << endl;
   delete copier;
   delete fin;
   delete fout;
}
   
