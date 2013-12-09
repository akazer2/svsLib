#Report for Final
##Ameen Kazerouni

###Functionality:
The class is made of tools that leverage two popular image processing libraries. 
Using functions from both Openslide and Libtiff the class can quickly extract regions 
from a given svs,vms,ndpi,tif,mrxs image. It can also tile the image given user defined tile size and zoom 
level. In addition to this the image provides for functionality that recursively 
searches through a file system to locate and identify these images and perform 
processing on them.

###Source:
[https://github.com/akazer2/svsLib]
Make file provided, install dependencies in appropriate directories or change make 
file accordingly.

###Dependencies:
1.	Openslide
  1.	[http://openslide.org/download/]
  2.	3.3 or newer. [Install from unstable master for NDPI support]
2.	Libtiff
  1.	[http://download.osgeo.org/libtiff/]
  2.	4.0 or newer [5.0 is required for big tiffs i.e. over 4gb]
3.	Openjpeg
4.	Glib-2
5.	gdk-pixbuf2 [Needed for unstable version of Openslide for NDPI support]

###svsLibrary:

1.```searchDirectory(std::vector<string> *imageLocations, const char* directoryName);```
  * The search directory function takes a pointer to a vector of strings and 
a directory name as a parameter.
  * It recursively walks down the file system starting at the provided 
directory and adds any svs,ndpi,vms,tif, and mrxs files it finds to the vector.
  * This vector can later be iterated through to obtain handles for each 
 image which can be passed to any other function in the class for 
processing.

2.```loadImage(std::string &fileName);```
  * loadImage simply takes in the full path for an image and returns a 
pointer to the openslide_t handle for the image.
  * This handle is what is passed to other function for further processing.

3.```partition_no_overlap(std::string filename, openslide_t *osr, int x, int y, int w, int h, int level);```
  * filename :- Write Directory
  * osr :- Image handle
  * x, y :- Start coordinates
  * w, h :- Tile width and height
  * level :- Zoom level
  * This function tiles the svs image starting from the x,y coordinate 
provided by the user. The tiles are of width and height provided by 
the user and at a user defined zoom level.
  *There is no overlap between the tiles.
  *Each tile is written to a directory in tiff format.
  *The extraction of each tile is done by calling the extract_region 
function.

4.```partition_overlap(std::string filename, openslide t*osr, int x, int y, int w, int h, int ow, int oh, int level)```
  * filename :- Write Directory
  * osr :- Image handle
  * x, y :- Start coordinates
  * w, h :- Tile width and height
  * ow, oh :- Overlap width and height
  * level :- Zoom level
  * This function performs the same function has partition_no_overlap. 
The only difference is that the user can provide two additional 
parameters that define a region that overlaps on each tile.
  * The overlapped tiling can be useful for further analysis that requires 
identification of boundaries.

5.```extract_region(std::string filename, openslide_t *osr, int x, int y, int w, int h, int level)```
  * filename :- Write location
  * osr :- Image handle
  * x, y :- Start coordinates
  * w, h :- Tile width and height
  * level :- Zoom level
  * This function does the actual extraction of the region. The x and y 
coordinate pertains to zoom level 0. The image region is extracted and 
then written in tiff format.
  * The Libtiff library is used for the writing of the images.

6.```ndpi_partition__no_overlap(int mag, int w, int h, int overlap_percent, std::string image path)```
  * mag :- Magnification level
  * w, h :- Tile width and height
  * image_path :- Image path
  * This function is provided for users using the latest stable version of openslide. It makes a system call to the ndpisplit binaries that will tile the ndpi image at a given magnification with tiles of size w and h. It is highly recommended that if ndpi images need processing, the master branch from the openslide git be used.

7.```ndpi_partition_no_overlap(int mag, int w, int h, int overlap_percent, std::string image path)```
  * mag :- Magnification level
  * w, h :- Tile width and height
  * overlap_percent :- Percentage of overlap between tiles
  * image_path :- Image path
  * Adds overlap between tiles to ndpi_partition_no_overlap

####Driver.cpp

1. Driver.cpp is simple driver program that consumes the tilerLibrary and 
demonstrates its features. 

2.  driver simply searches through a directory that is provided by the user 
and creates a vector of files.

3. Its then provides examples on how to use the functions provided in the 
library.

####Challenges
1. X - Y coordinate conversion
  * The openslide_read_region function takes in x,y coordinates relative 
to zoom level 0.
  * This means that while tiling the image at a zoom level other than zero 
the x,y coordinates need to be converted to be relative to the user 
define magnification.
  * This can be done by using a combination of openslide functions. 
Openslide provides functionality to extract the width and the height of 
the image at a given zoom level. In addition to this Openslide allows 
you to also obtain a downsampling factor for a given zoom level. The 
downsampling factor can be used to off set the x, y coordinates 
appropriately.

2. Writing the images to TIFF
  * A problem I ran into while writing the images was that determining 
the order of the channels and determining how many channels were 
present.
  * I was initially attempting to write to ppm but was unable to do so 
successfully. It turns out that this is because Openslide's read region 
function provides 4 channels including an empty alpha channel. In 
addition the this the order of the channels do not follow the 
traditional RGB.
  * I ended up leveraging Libtiff's tiffio class that allows us to write out 
TIFF from a character buffer of pixels. This simplified matters because 
it allowed me to provide the number of samples per pixel and the bit 
size of each sample. You could also set the photometric property of 
the Tiff "out" object too RGB which automatically figured out the 
order of the channels coming in from the SVS region. 
  * A small issue was that I was still under the impression that the SVS 
region had only 3 regions which results in the TIFFs being in gray 
scale. After further inquiry I realized that the TIFF "out" object could 
be configured to skip over the empty alpha channel.

3. Reading NDPI images
  * NDPI is a propriety format with an internal structure that includes both a broken tiff and a broken jpeg architecture. The sdk to process these images requires obtaining a license and a non-disclosure agreement. Trying to work without this require the reverse engineering of the format, which many have tried and failed at [within are department as well]. The only two libraries I could find that support ndpi is the master branch of openslide and NDPITools. I have implemented all functionality using openslide and partial tiling functionality using NDPITools

####Runs

I ran the code in driver.cpp and to test the library. Both the overlapping and non 
overlapping tilers were run on a single SVS, VMS, NDPI, TIF and MRXS files and I visually inspected the TIFFs to 
ensure that the entire image was being covered in the tiles. In addition to this I ran 
the search directory function on /data/images on my research machine that has a 
network of directories full of SVS, tiff and dzi images. The function was able to 
successfully identify the SVS files and iteration through the vector giving openslide_t 
handles for each one of them. I verified that the handles were valid by extracted 
metadata from the images using the handle. Due to time constraints I was unable to 
run the tiler on more than 2 - 3 images at a time.

Additionally I created a directory structure that had VMS, SVS, NDPI, TIF and MRXS files present. The directory walk down was able to identify each one of these files and tile them.

####Data

1.Tiles I have saved from test runs
  * The tiffs from tiling without overlap at zoom level 1 and 2 of an SVS
  * The tiff from tilling with overlap at zoom level 1 for the same SVS.
  * The tiffs from tiling without overlap at zoom level 2 for a vms.
  * The tiffs from tiling without overlap at zoom level 2 for a ndpi.
  * The tiffs from tiling without overlap at zoom level 2 for a tif.
  * The tiffs from tiling without overlap at zoom level 2 for a mrxs.
