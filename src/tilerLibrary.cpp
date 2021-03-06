#include "tilerLibrary.hpp"

void tilerLibrary::searchDirectory(std::vector<std::string> *imageLocations,const char* directoryName)
{
    DIR *dp;
    struct dirent *entry = NULL;
    dp = opendir(directoryName);
    if(dp == NULL)
    {
	cerr << directoryName << ": Does not exist" << endl;
    }

    struct stat buf;
    struct stat *s = &buf;
    chdir(directoryName);

    while((entry = readdir(dp)) != NULL)
    {
	if(stat(entry->d_name,s) == 0)
	{
	    int path_length;
	    char new_path[PATH_MAX];
	    path_length = snprintf(new_path, PATH_MAX, "%s/%s", directoryName, entry->d_name);

	    if(path_length >= PATH_MAX)
	    {
		cerr << "Path length has gotten too long. Stopped at " << new_path <<endl;
	    }

	    if(s->st_mode & S_IFDIR)
	    {
		if(strcmp(entry->d_name,"..") != 0 && strcmp(entry->d_name,".") != 0)
		{
		    searchDirectory(imageLocations,new_path);
		}
	    }

	    else if(s->st_mode & S_IFREG)
	    {
		std::string fullPath = new_path;

		size_t pos_ndpi = fullPath.rfind(".ndpi");
		if(pos_ndpi!= std::string::npos && pos_ndpi==fullPath.size()-5)
		{
		    imageLocations->push_back(fullPath);
		}
		size_t pos = fullPath.rfind(".vms");
		if(pos!= std::string::npos && pos==fullPath.size()-4)
		{
		    if(openslide_can_open(fullPath.c_str()))
			imageLocations->push_back(fullPath);
		}
		
		size_t pos_svs = fullPath.rfind(".svs");
		if(pos_svs!= std::string::npos && pos_svs==fullPath.size()-4)
		{
		    if(openslide_can_open(fullPath.c_str()))
			imageLocations->push_back(fullPath);
		}

		size_t pos_mrx = fullPath.rfind(".mrxs");
		if(pos_mrx!= std::string::npos && pos_mrx==fullPath.size()-5)
		{
		    if(openslide_can_open(fullPath.c_str()))
			imageLocations->push_back(fullPath);
		}
		
		size_t pos_tres = fullPath.rfind(".tif");
		if(pos_tres!= std::string::npos && pos_tres==fullPath.size()-4)
		{
		    if(openslide_can_open(fullPath.c_str()))
			imageLocations->push_back(fullPath);
		}
	    }
	}

	else
	{
	    cerr << "Stat Failed on file" << endl;
	}
    }

    closedir(dp);
    chdir("../");
}

openslide_t* tilerLibrary::loadImage(std::string &fileName_s)
{
    const char* fileName = fileName_s.c_str();
    openslide_t *osr = openslide_open(fileName);
    return osr;
}
string convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

void tilerLibrary::create_metadata_file(openslide_t *osr,std::string &fileName_s)
{
    const char* const *property_names = openslide_get_property_names(osr);
    while(*property_names)
    {
	const char *name = *property_names;
	const char *value = openslide_get_property_value(osr,name);
	cout << "Property: " << name << endl << "Value: " << value << endl << endl;
	property_names++;
    }
}

void tilerLibrary::ndpi_partition_overlap(int mag, int w, int l, int overlap_percent, std::string imagePath)
{
    std::string ndpi_call = "/home/akazerouni/Desktop/MIA/./ndpisplit -x";
    std::stringstream sstm;
    sstm << ndpi_call << mag << " -v -M -o" << overlap_percent << " -g" << w << "x" << l << " " << imagePath;
    
    std::string system_call = sstm.str();

    system(system_call.c_str());
}

void tilerLibrary::ndpi_partition_no_overlap(int mag, int w, int l, std::string imagePath)
{
    std::string ndpi_call = "/home/akazerouni/Desktop/MIA/./ndpisplit -x";
    std::stringstream sstm;
    sstm << ndpi_call << mag << " -v -M  -g" << w << "x" << l << " " << imagePath;
    
    std::string system_call = sstm.str();

    system(system_call.c_str());
}

void tilerLibrary::partition_overlap(std::string fileName, openslide_t *osr, int x, int y, int w, int h, int ow, int oh, int z)
{
    int64_t level_w;
    int64_t level_h;

    openslide_get_level_dimensions(osr,z,&level_w,&level_h);
    int downsample = openslide_get_level_downsample(osr,z);
    int x_change = (w - ow)*downsample;
    int y_change = (h - oh)*downsample;

    mkdir(fileName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    int horizontalTilesNoOverlap = (level_w - x)/w;
    int verticalTilesNoOverlap = (level_h - y)/h;

    int virtualWidth = (level_w - x) + (horizontalTilesNoOverlap * ow);
    int virtualHeigth = (level_h - y) + (verticalTilesNoOverlap * oh);

    int horizontalTiles = virtualWidth/w;
    int verticalTiles = virtualHeigth/h;
    
    int tile_x = x;
    int tile_y = y;
    
    std::string temp = "/tile";
    int count = 0;
    for(int i = 0; i < verticalTiles; i++)
    {
	tile_x = x;
	for(int j = 0; j < horizontalTiles; j++)
	{
	    count++;
	    std::string counts = convertInt(count);
	    std::string t2 = fileName + temp + counts;
	    tilerLibrary::extract_region(t2,osr,tile_x,tile_y,w,h,z);
	    tile_x += x_change;
	}
	tile_y+=y_change;
    }

    int finalTileW = (level_w-x)%w;
    int finalTileH = (level_h-y)%h;
    if(finalTileW != 0)
    {
	int finalTileY = y;
	for(int k = 0; k < verticalTiles; k++)
	{
	    count++;
	    std::string counts = convertInt(count);
	    std::string t2 = fileName + temp + counts;
	    tilerLibrary::extract_region(t2,osr,tile_x,finalTileY,finalTileW,h,z);
	    finalTileY += y_change;
	}
    }

    if(finalTileH != 0)
    {
	int finalTileX = x;
	for(int p = 0; p < horizontalTiles; p++)
	{
	    count++;
	    std::string counts = convertInt(count);
	    std::string t2 = fileName + temp + counts;
	    tilerLibrary::extract_region(t2,osr,finalTileX,tile_y,w,finalTileH,z);
	    finalTileX += x_change;
	}
    }


    if(finalTileW != 0 && finalTileH != 0)
    {
	int finalTileW = (level_w-x)%w;
	int finalTileH = (level_h-y)%h;
	count++;
	std::string counts = convertInt(count);
	std::string t2 = fileName + temp + counts;
    }
}
void tilerLibrary::partition_no_overlap(std::string fileName, openslide_t *osr, int x, int y, int w, int h, int z)
{
    int64_t level_w;
    int64_t level_h;
    openslide_get_level_dimensions(osr,z,&level_w,&level_h);
    int downsample = openslide_get_level_downsample(osr,z);
    int x_change = w*downsample;
    int y_change = h*downsample;
    mkdir(fileName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    int horizontalTiles = (level_w -x)/w;
    int verticalTiles = (level_h-y)/h;

    int tile_x = x;
    int tile_y = y;
    
    std::string temp = "/tile";
    int count = 0;
    for(int i = 0; i < verticalTiles; i++)
    {
	tile_x = x;
	for(int j = 0; j < horizontalTiles; j++)
	{
	    count++;
	    std::string counts = convertInt(count);
	    std::string t2 = fileName + temp + counts;
	    tilerLibrary::extract_region(t2,osr,tile_x,tile_y,w,h,z);
	    tile_x += x_change;
	}
	tile_y+=y_change;
    }

    int finalTileW = (level_w-x)%w;
    int finalTileH = (level_h-y)%h;
    if(finalTileW != 0)
    {
	int finalTileY = y;
	for(int k = 0; k < verticalTiles; k++)
	{
	    count++;
	    std::string counts = convertInt(count);
	    std::string t2 = fileName + temp + counts;
	    tilerLibrary::extract_region(t2,osr,tile_x,finalTileY,finalTileW,h,z);
	    finalTileY += y_change;
	}
    }

    if(finalTileH != 0)
    {
	int finalTileX = x;
	for(int p = 0; p < horizontalTiles; p++)
	{
	    count++;
	    std::string counts = convertInt(count);
	    std::string t2 = fileName + temp + counts;
	    tilerLibrary::extract_region(t2,osr,finalTileX,tile_y,w,finalTileH,z);
	    finalTileX += x_change;
	}
    }


    if(finalTileW != 0 && finalTileH != 0)
    {
	int finalTileW = (level_w-x)%w;
	int finalTileH = (level_h-y)%h;
	count++;
	std::string counts = convertInt(count);
	std::string t2 = fileName + temp + counts;
	tilerLibrary::extract_region(t2,osr,tile_x,tile_y,finalTileW,finalTileH,z);
    }
}

void tilerLibrary::extract_region(std::string filename, openslide_t *osr, int x, int y, int w, int h, int z)
{
    size_t regionSize = w * h * sizeof(uint32_t);
    char *buffer = (char *)malloc(regionSize);
    memset(buffer,0,regionSize);

    cout << filename << endl;
    openslide_read_region(osr,(uint32_t*)buffer,x,y,z,w,h);

    TIFF *out = TIFFOpen(filename.c_str(),"w");
    int perpixel = 4;
    
    TIFFSetField(out,TIFFTAG_IMAGEWIDTH,w);
    TIFFSetField(out,TIFFTAG_IMAGELENGTH,h);
    TIFFSetField(out,TIFFTAG_SAMPLESPERPIXEL,perpixel);
    TIFFSetField(out,TIFFTAG_BITSPERSAMPLE,8);
    TIFFSetField(out,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    TIFFSetField(out,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    TIFFSetField(out,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);

    tsize_t line = perpixel * w;
    unsigned char *buf = NULL;
    if(TIFFScanlineSize(out)==line)
    {
	buf = (unsigned char*)_TIFFmalloc(line);
    }

    else
    {
	buf = (unsigned char*)_TIFFmalloc(TIFFScanlineSize(out));
    }

    TIFFSetField(out,TIFFTAG_ROWSPERSTRIP,TIFFDefaultStripSize(out,w*perpixel));
    for(uint32_t row = 0; row < h; row++)
    {
	memcpy(buf,&buffer[(h-row-1)*line],line);
	if(TIFFWriteScanline(out,buf,row,0) <0)
	    break;
    }

    (void) TIFFClose(out);
    if(buf)
	_TIFFfree(buf);

    free(buffer);
}


