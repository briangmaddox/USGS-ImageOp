#include "ImageOp/CopyImage.h"
#include "Image/RGBImage.h"
#include "Image/ImageDebug.h"
#include "Image/TIFFImageOFile.h"
#include "Image/TIFFImageIFile.h"
#include <iostream.h>
#include <GetOpt.h>

void usage(char *msg)
{
   cerr << "Error: " << msg << endl;
   cerr << "Usage: tiffCopy [ -icnh ] [ -Dxysd <option> ] <file> <newfile>";
   cerr << endl;
   exit(42);
}

void help(void)
{
   cout << "Usage: tiffCopy [ options ] <file> <newfile>" << endl << endl;
   cout << "Where options may be:" << endl;
   cout << "   -x <res>      Specify x resolution" << endl;
   cout << "   -y <res>      Specify y resolution" << endl;
   cout << "   -s <string>   Specify SOFTWARE_TAG" << endl;
   cout << "   -d <string>   Specify DESCRIPTION_TAG" << endl;
   cout << "   -i            Specify inches as resolution unit" << endl;
   cout << "   -c            Specify centimeters as resolution unit" << endl;
   cout << "   -n            Specify none as resolution unit" << endl;
   cout << "   -D <level>    Specify debugging level" << endl;
   cout << "   -h            This help message" << endl;
   exit(0);
}

int main(int argc, char **argv)
{
   RGBImage *fin;
   RGBImage *fout;
   CopyImage *copier;
   float xRes=72.0;
   float yRes=72.0;
   unsigned short unit = RESUNIT_INCH;
   char *software=NULL;
   char *description=NULL;
   int debuglevel=0;
   int option_char;
   GetOpt getopt(argc,argv,"D:x:y:icns:d:h");
   TIFFImageIFile* tiffin;
   TIFFImageOFile* tiffout;

   while ((option_char = getopt())!=EOF)
   {
      switch (option_char)
      {
         case 'D': debuglevel = atoi(getopt.optarg); break;
         case 'x': xRes = atof(getopt.optarg); break;
         case 'y': yRes = atof(getopt.optarg); break;
         case 'i': unit = RESUNIT_INCH; break;
         case 'c': unit = RESUNIT_CENTIMETER; break;
         case 'n': unit = RESUNIT_NONE; break;
         case 's': software = new char[strlen(getopt.optarg)+1];
                   strcpy(software,getopt.optarg);
                   break;
         case 'd': description = new char[strlen(getopt.optarg)+1];
                   strcpy(description,getopt.optarg);
                   break;
         case 'h': help(); break;
         case '?': usage("unknown option"); break;
      }
   }
   gDebug.setDebugLevel(debuglevel);
   if (getopt.optind+1 < argc) 
   {
      fin = new RGBImage(argv[getopt.optind]);
      if (!fin->good())
      {
         cerr << "Can't open " << argv[getopt.optind] << endl;
         exit(42);
      }
      fout = new RGBImage(argv[getopt.optind+1],fin->getWidth(),
                          fin->getHeight());
      if (!fout->good())
      {
         cerr << "Can't open " << argv[getopt.optind+1] << endl;
         exit(42);
      }
   }
   else
      usage("missing <file> argument.");
   if ((!fin->getFileType()==IMAGE_TIFF) || !(fout->getFileType()==IMAGE_TIFF))
   {
      cerr << "tiffCopy only works for TIFF files." << endl;
      exit(42);
   }
   tiffin  = (TIFFImageIFile *)(fin->getImageData());
   tiffout = (TIFFImageOFile *)(fout->getImageData());
   tiffout->setTag(TIFFTAG_XRESOLUTION,xRes); 
   tiffout->setTag(TIFFTAG_YRESOLUTION,yRes); 
   tiffout->setTag(TIFFTAG_RESOLUTIONUNIT,unit);
   if (software != NULL)
      tiffout->setTag(TIFFTAG_SOFTWARE,software);
   else
   {
      if (tiffin->getTag(TIFFTAG_SOFTWARE,&software))
         tiffout->setTag(TIFFTAG_SOFTWARE,software);
   }
   if (description != NULL)
      tiffout->setTag(TIFFTAG_IMAGEDESCRIPTION,description);
   else
   {
      if (tiffin->getTag(TIFFTAG_IMAGEDESCRIPTION,&description))
         tiffout->setTag(TIFFTAG_IMAGEDESCRIPTION,description);
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
   
