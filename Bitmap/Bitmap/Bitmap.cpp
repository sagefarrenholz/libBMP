#include "pch.h"
#include "Bitmap.h"

#include <string>
#include <time.h>
#include <random>

int main()
{
	//Prints debug information
	constexpr bool debug = true;

	//Image height and width
	int32_t width, height;
	
	/*
	
	bitdepth refers how many bits are used for one color in a pixel:
	at bitdepths of 8 or lower, bits in each pixel are used to reference colors in our pallete
	at bitdepths above 8, bits refer directly to the hex color codes
		
		here are some examples
		
			bitdepth 1:
			effectively a monochromatic image
			each bit refers to one pixel, a byte holds information for 8 pixels
			each bit is 1 or 0 so it references the first or second color in our color palette

			bitdepth 4:
			we are still forced to use indexed colors
			our bit depth is 4 so we need 4 bits for one pixel, a byte will hold 2 pixels worth of information
			we should use a palette of 16 colors because we have 4 bits of data for each pixel to reference our palette

			bitdepth 24:
			color indexing is now optional
			at a bitdepth of 24 we require 3 bytes or 24 bits to represent 1 pixel
			so effectively we must triple our width to get the acutal amount of bytes needed to represent our full image

	*/

	uint32_t bitdepth = 24, colorcount = 0, resolution = 0;

	bool padding = false;

	std::string filename = "mypicture.bmp";

	/*std::cout << "Enter target file" << std::endl;
	std::cin >> filename;*/

	std::ofstream mypicture(filename, std::ios::binary);

	std::cout << "Enter width" << std::endl;
	std::cin >> width;

	if (!width)
		width = 64;

	std::cout << "Enter height" << std::endl;
	std::cin >> height;

	if (!height)
		height = 64;



	/*
	This pads the width so each row is a multiple of 4 bytes.
	First it checks if the actual byte width is a multiple of 4.
	If not it increases the actual byte width to the next multiple of 4.
	*/

	//The actual byte width with padding
	long arraywidth = 0;
	double widthpadding = 0;

	if (std::fmod(width / (8.0 / bitdepth), 4) == 0) 
		arraywidth = width / (8.0 / bitdepth); 	
	else {
		int temp = width / (8.0 / bitdepth);
		do {
			temp++;
		} while (temp % 4 != 0);
		padding = true;
		arraywidth = temp;
		widthpadding = temp - (width / (8.0 / bitdepth));
	}

	//Pixel data vector
	std::vector<uint8_t> pixeldata(height*arraywidth);

	/*
	Color Pallete:
	Each color is ordered accordingly, each color must be preceeded by one reserved 0x00 byte
	*/
	uint32_t palette[2] = { 0x0042f44e, 0x00f71d5e };

	//std::srand(time(NULL));

	bool generating = false;
	int coun = 0;

	//Increment based on bitdepth
	int inc = 1;
	switch (bitdepth) {
		case 24: 
			inc = 3;
			break;
		case 16:
			inc = 2;
			break;
	}

	/*
	
	Here I am creating a rainbow using small increments
	I start the loop count at 3 and start the color at red 0xff0000
	
	*/
	//double ratio = std::fmod((double)(0x30000) / (height), 0xff);
	//double ratio = rat;
	unsigned ratio = 2;
	pixeldata[0] = 0xff;
	pixeldata[1] = 0x00;
	pixeldata[2] = 0x00;

	/*

	There are six color states listed below for our rainbow:
	0: red to yellow
	1: yellow to green
	2: green to aqua
	3: aqua to blue
	4: blue to purple
	5: purple to red

	*/
	int colorstate = 0;
	//How much we increment each color by
	unsigned redinc = 0, greeninc = 0, blueinc = 0;
	coun = 3;

	while (coun < (height * arraywidth)) {

		if (!generating) {
			generating = true;
			std::cout << "\nGenerating..." << std::endl;
		}

 		switch (colorstate) {
			//red to yellow
			case 0:
				//only increment if we dont overflow
				if (pixeldata[coun - 2] + greeninc < 0xff && pixeldata[coun - 2]) 
					greeninc = ratio;
				else if (pixeldata[coun - 2] + greeninc != 0xff) 
					greeninc = 0xff - pixeldata[coun - 2];
				else {
					colorstate = 1;
					greeninc = 0;
				}
				break;
			//yellow to green
			case 1:
				//only increment if we dont undeflow
				if (pixeldata[coun - 3] - redinc > 0x00) 
					redinc = ratio;
				else if (pixeldata[coun - 3] - redinc != 0x00)
					redinc = pixeldata[coun - 3];
				else {
					colorstate = 2;
					redinc = 0;
				}
				break;
			//green to aqua
			case 2:
				if (pixeldata[coun - 1] + blueinc < 0xff) 
					blueinc = ratio;
				else if (pixeldata[coun - 1] + blueinc != 0xff)
					blueinc = 0xff - pixeldata[coun - 1];
				else {
					colorstate = 3;
					blueinc = 0;
				}
				break;
			//aqua to blue
			case 3:

				if (pixeldata[coun - 2] - greeninc > 0x00) 
					greeninc = ratio;
				else if (pixeldata[coun - 2] - greeninc != 0x00)
					greeninc = pixeldata[coun - 2];
				else {
					colorstate = 4;
					greeninc = 0;
				}
				break;
			//blue to purple
			case 4:

				if (pixeldata[coun - 3] + redinc < 0xff)  
					redinc = ratio;
				else if (pixeldata[coun - 3] + redinc != 0xff)
					redinc = 0xff - pixeldata[coun - 3];
				else {
					colorstate = 5;
					redinc = 0;
				}
				break;
			//purple to red
			case 5:

				if (pixeldata[coun - 1] - blueinc > 0x00) 
					blueinc = ratio;
				else if (pixeldata[coun - 1] - blueinc != 0x00)
					blueinc = pixeldata[coun - 1];
				else {
					colorstate = 0;
					blueinc = 0;
				}
				break;
		}


		if (padding) {
			if (!isPadding(height, arraywidth, coun, bitdepth, widthpadding)) {
				//This is not padding so begin the processs of filling in our hex color information	
				if(colorstate == 1)
					pixeldata[coun] = pixeldata[coun - 3] - redinc;
				else
					pixeldata[coun] = pixeldata[coun - 3] + redinc;
				//If we have a bit depth of 8 or below we must use a color pallete
				if (bitdepth <= 8) {
					pixeldata[coun-3] = 170;
				}
			} else {
				//Continue to next byte and do nothing with this padded byte (0x00)
				coun++;
				continue;
			}
			if ((!isPadding(height, arraywidth, coun + 1, bitdepth, widthpadding)) && (coun + 1 < height * arraywidth) && (bitdepth == 16 || bitdepth == 24)) {
				//We need this byte for 16 bit and 24 color information
				if(colorstate == 3)
					pixeldata[coun + 1] = pixeldata[coun - 2] - greeninc;
				else
					pixeldata[coun + 1] = pixeldata[coun - 2] + greeninc;
			}
			if ((!isPadding(height, arraywidth, coun + 2, bitdepth, widthpadding)) && (coun + 2 < height * arraywidth) && (bitdepth == 24)) {
				//This third byte is only for 24 bit color information
				if(colorstate == 5)
					pixeldata[coun+2] = pixeldata[coun - 1] - blueinc;
				else 
					pixeldata[coun+2] = pixeldata[coun - 1] + blueinc;
			}
		} else {
			if (colorstate == 1)
				pixeldata[coun] = pixeldata[coun - 3] - redinc;
			else
				pixeldata[coun] = pixeldata[coun - 3] + redinc;
			//If we have a bit depth of 8 or below we must use a color pallete
			if (bitdepth <= 8) {
				if (bitdepth <= 8) {
					pixeldata[coun] = 170;
				}
			}

			if ((coun + 1 < height * arraywidth) && (bitdepth == 16 || bitdepth == 24)) {
				//We need this byte for 16 bit and 24 color information
				if (colorstate == 3)
					pixeldata[coun + 1] = pixeldata[coun - 2] - greeninc;
				else
					pixeldata[coun+1] = pixeldata[coun - 2 ] + greeninc;
			}
			if ((coun + 2 < height * arraywidth) && (bitdepth == 24)) {
				//This third byte is only for 24 bit color information
				if (colorstate == 5)
					pixeldata[coun + 2] = pixeldata[coun - 1] - blueinc;
				else
					pixeldata[coun + 2] = pixeldata[coun - 1] + blueinc;
			}
		}

		//increment the counter
		coun += inc;
		//ratio = (unsigned) (ratio + rat) % 0xff;

	}

	generating = false;
	std::cout << "Done." << std::endl;

	BMP_HEADER h =
		{
			{'B','M'},
			sizeof(BMP_HEADER) + colorcount * 4U + pixeldata.size(),
			0,
			sizeof(BMP_HEADER) + colorcount * 4U,
			40,
			width,
			height,
			1,
			bitdepth,
			0,
			0,
			resolution,
			resolution,
			colorcount,
			0
		};

	//Print data about image
	if (debug) {
		std::cout << "\n-------------------- Debug --------------------" << std::dec << std::endl;
		std::cout << "Amount of byte elements: " << pixeldata.size() << std::endl;
		std::cout << "Bitdepth: " << bitdepth << std::endl;
		std::cout << "Width in bytes: " << arraywidth << std::endl;
		std::cout << "Padding bytes: " << widthpadding << std::endl;
		std::cout << "Padding: " << (padding ? "Yes" : "No") << std::endl;
		std::cout << "Color palette: " << (colorcount > 0 ? "Yes" : "No") << std::endl;
	}
	mypicture.write(reinterpret_cast<char*>(&h), sizeof(h));

	if(colorcount > 0)
		mypicture.write(reinterpret_cast<char*>(&palette), colorcount * 4U);

	std::reverse(pixeldata.begin(), pixeldata.end());

	for (auto it = pixeldata.begin(); it != pixeldata.end(); ++it) 
		mypicture.write(reinterpret_cast<char*>(&(*it)), 1);

	mypicture.close();
	return 0;
}

