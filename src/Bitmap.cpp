#include "Manifest.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define BITMAP_TYPE_UNSUPPORTED 1
#define BITMAP_TYPE_GRAY 2 // 8-bit single color component color format

class Bitmap {
public:
	Bitmap(int width, int height, int type);
	~Bitmap();

	int width;
	int height;
	char * pixels;
	int type;

	void Draw(FT_Bitmap &bitmap, int xp, int yp);

	int GetMaxY() { return yMax; }

	static int TypeFromFTBitmapType(char ft_type);

private:
	// Function to draw buffer of 8-bit colors of width and height to x and y of this bitmap.
	void Draw8Bit(char * buffer, int width, int height, int xp, int yp);

	int yMax;
};


Bitmap::Bitmap(int width, int height, int type)
:yMax(0)
{
	if (type == 0) {
		Log("Cannot create bitmap with type of 0!");
	}

	this->width = width;
	this->height = height;
	this->type = type;
	pixels = new char[width * height];

	for (int i = 0; i < width * height; i++) {
		*(pixels + i) = 0;
	}
}

Bitmap::~Bitmap()
{
	delete pixels;
}

void Bitmap::Draw(FT_Bitmap &bitmap, int xp, int yp)
{
	yMax = std::max(yMax, bitmap.rows + yp);
	if (yMax >= this->height) {
		yMax = this->height;
	}

	if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
		// Mono bitmaps are packed in some interesting way, to unpack it, this tutorial was used.
		// http://dbader.org/blog/monochrome-font-rendering-with-freetype-and-python

		// first, unpack into 8-bit color gray color
		char buffer[bitmap.width * bitmap.rows];

		for ( int y = 0; y < bitmap.rows; y++ ) {
			for ( int byte_index = 0; byte_index < bitmap.pitch; byte_index++ ) {
				
				char byte_value = *( bitmap.buffer + (byte_index + y * bitmap.pitch) );
				
				int num_bits_done = byte_index * 8;
				
				int rowstart = y * bitmap.width + byte_index * 8;
				
				for ( int bit_index = 0; bit_index < std::min(8, bitmap.width - num_bits_done); bit_index++ ) {
					char bit = byte_value & (1 << (7 - bit_index));

					*(buffer + rowstart + bit_index) = bit == 0 ? 0 : 1;
				}
				
			}
		}

		// draw just like BITMAP_TYPE_GRAY
		Draw8Bit(buffer, bitmap.width, bitmap.rows, xp, yp);		

	} else if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY) {
		// single 8-bit color
		Draw8Bit((char*) bitmap.buffer, bitmap.width, bitmap.rows, xp, yp);

	} else {
		Log("No type for bitmap!", "");
	}

}

void Bitmap::Draw8Bit(char * buffer, int width, int height, int xp, int yp)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			
			int xx = xp + x;
			int yy = yp + y;

			if (xx < 0 || xx >= this->width || yy < 0 || yy >= this->height) {
				// TODO optimizations, breaking whole loop instead of skipping current pixel
				// out of bounds
				continue;
			}

			*(this->pixels + xx + yy * this->width) = *(buffer + x + y * width);

		}
	}
}

int Bitmap::TypeFromFTBitmapType(char ft_type)
{
	if (ft_type == FT_PIXEL_MODE_NONE) {
		return 0;

	} else if (ft_type == FT_PIXEL_MODE_GRAY) {
		return BITMAP_TYPE_GRAY;

	}

	return BITMAP_TYPE_UNSUPPORTED;
}