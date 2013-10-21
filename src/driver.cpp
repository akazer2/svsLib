#include <cstdlib>
#include <iostream>
#include <openslide.h>
#include "svsLibrary.hpp"

using namespace std;

int main(int argc, char **argv)
{
    svsLibrary svslib;

    std::vector<std::string> *imageLocations;
    imageLocations = new std::vector<std::string>;
    const char *directoryName = "/home/ameen/MIA/image";
    svslib.searchDirectory(imageLocations,directoryName);
    std::string path = "/home/ameen/MIA/";

    for(std::vector<std::string>::iterator it = imageLocations->begin(); it != imageLocations->end(); ++it)
    {
	openslide_t *osr = svslib.loadImage(*it);
	std::string temp = *it;
	int pos = temp.rfind("/");
	std::string fileName = temp.substr(pos+1);
	std::string full = path + fileName;
	svslib.partition_no_overlap(full,osr,0,0,100,100,2);
	//svslib.partition_overlap(full,osr,0,0,100,100,10,10,2);
	//svslib.extract_region(fileName,osr,0,0,1000,1000,2);
	break;
    }
    
}
