#include <string>
#include <tiffio.h>
#include <openslide.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <inttypes.h>
#include <dirent.h>
#include <stdio.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/time.h>
#include <limits.h>
using namespace std;

class svsLibrary
{
    public:
	virtual void searchDirectory(std::vector<std::string> *imageLocations,const char* directoryName);
	virtual openslide_t* loadImage(std::string &fileName);
	virtual void  extract_region(std::string filename, openslide_t *osr,int x, int y, int w, int h, int level);
	virtual void partition_no_overlap(std::string filename,openslide_t *osr, int x, int y, int w, int h, int level);
	virtual void partition_overlap(std::string filename,openslide_t *osr, int x, int y, int w, int h, int ow, int oh, int level);
};
