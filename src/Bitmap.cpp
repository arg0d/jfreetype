#include "Manifest.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class Bitmap {
public:
	Bitmap();
	Bitmap(int width, int height);
	~Bitmap();

	void Intialize(int width, int height);

	void Draw(FT_Bitmap &bitmap, int xp, int yp);

	const char* GetPixels() { return pixels; }
	int GetWidth() { return width; }
	int GetHeight() { return height; }

private:

	char * pixels;
	int width;
	int height;

};

Bitmap::Bitmap():
 pixels(NULL)
{
}

Bitmap::Bitmap(int width, int height):
 pixels(NULL)
{
	Intialize(width, height);
}

Bitmap::~Bitmap()
{
	delete pixels;
}

void Bitmap::Intialize(int width, int height)
{
	if (pixels != NULL)
	{
		delete pixels;
	}

	this->width = width;
	this->height = height;
	pixels = new char[width * height * 4];

	for (int i = 0; i < width * height * 4; i++) {
		pixels[i] = 0;
	}
}

void Bitmap::Draw(FT_Bitmap &bitmap, int xp, int yp)
{
	for (int y = 0; y < bitmap.rows; y++)
	{
		for (int x = 0; x < bitmap.width; x++)
		{
			int xx = xp + x;
			int yy = yp + y;

			if (xx < 0 || xx >= this->width || yy < 0 || yy >= this->height) {
				// TODO optimizations, breaking whole loop instead of skipping current pixel
				// out of bounds
				continue;
			}

			int index = (xx + yy * this->width) * 4;
			this->pixels[index + 0] = 255;
			this->pixels[index + 1] = 255;
			this->pixels[index + 2] = 255;
			this->pixels[index + 3] = bitmap.buffer[x + y * bitmap.width];
		}	
	}
}