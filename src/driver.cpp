#include <cstdlib>
#include <iostream>
#include <openslide.h>
#include "tilerLibrary.hpp"

using namespace std;

int main(int argc, char **argv)
{
    tilerLibrary tilerlib;

    std::vector<std::string> *imageLocations;
    imageLocations = new std::vector<std::string>;
    
    const char *directoryName = "/home/akazerouni/Desktop/MIA/image";
    std::string output_path = "/home/akazerouni/Desktop/MIA/";
    
    tilerlib.searchDirectory(imageLocations,directoryName);

    for(std::vector<std::string>::iterator it = imageLocations->begin(); it != imageLocations->end(); ++it)
    {
	std::string temp = *it;
	int pos = temp.rfind("/");
	std::string fileName = temp.substr(pos+1);
	std::string full = output_path + fileName;
    
	openslide_t *osr = tilerlib.loadImage(*it);
	tilerlib.partition_no_overlap(full,osr,0,0,1000,1000,2);
	//tilerlib.extract_region(fileName,osr,2000,3000,2000,2000,2);
	//tilerlib.partition_overlap(full,osr,0,0,100,100,10,10,2);
	//tilerlib.create_metadata_file(osr,*it);
    }
    
}
