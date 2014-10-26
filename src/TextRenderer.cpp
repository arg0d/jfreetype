#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Manifest.h"
#include "Bitmap.cpp"

using namespace std;

class TextRenderer {

public:
	/* Singleton */
	static TextRenderer *instance;
	
	Bitmap* Render(const std::string& text, float scale);
private:
	
	void PrintBitmap(Bitmap * bitmap);
	Bitmap* RenderChar(FT_Face, int charCode);
	Bitmap* RenderString(FT_Face face, const std::string& str);
	
};

TextRenderer* TextRenderer::instance = new TextRenderer();

Bitmap* TextRenderer::Render(const std::string& text, float scale)
{
	FT_Library  library;
	FT_Face face;
	int error = 0;
	
	error = FT_Init_FreeType( &library );
	if ( error )
	{
	
	}
	
	error = FT_New_Face( library, "C:/Windows/Fonts/Arial.ttf", 0, &face );
	if (error) {
		std::cout << "Could not load face!" << std::endl;
		return NULL;
	}
	
	error = FT_Set_Char_Size(
            face,    /* handle to face object           */
            0,       /* char_width in 1/64th of points  */
            16*64 * scale,   /* char_height in 1/64th of points */
            300,     /* horizontal device resolution    */
            300 );   /* vertical device resolution      */
	if (error) {
		std::cout << "Could not set char size!" << std::endl;
		return NULL;
	}
	
	
	return RenderString(face, text);

}

Bitmap* TextRenderer::RenderString(FT_Face face, const std::string& str)
{
	int error = 0;

	long glyph_index = FT_Get_Char_Index( face, (int) 'W');
	error = FT_Load_Glyph(
            face,          /* handle to face object */
            glyph_index,   /* glyph index           */
            0 );  /* load flags, see below */
	if (error) {
		std::cout << "Could not load glyph!" << std::endl;
		return NULL;
	}

	int h = face->glyph->metrics.height >> 6;
	
	Bitmap *bitmap = new Bitmap(139, h * 1.5, BITMAP_TYPE_MONO);

	int x_pen = 0;

	for (int i = 0; i < str.length(); i++) {
		long glyph_index = FT_Get_Char_Index( face, (int) str.at(i));
	
		error = FT_Load_Glyph(
	            face,          /* handle to face object */
	            glyph_index,   /* glyph index           */
	            0 );  /* load flags, see below */
		if (error) {
			std::cout << "Could not load glyph!" << std::endl;
			continue;
		}
		
		error = FT_Render_Glyph( face->glyph,   /* glyph slot  */
	                           FT_RENDER_MODE_MONO ); /* render mode */
		if (error) {
			std::cout << "Could not render glyph!" << std::endl;
			continue;
		}

		bool scalable = FT_IS_SCALABLE(face);
		Log("Height: %d", face->glyph->bitmap_top);
		bitmap->Draw(face->glyph->bitmap, x_pen, h -  face->glyph->bitmap_top);

		x_pen += face->glyph->advance.x >> 6;
	}

	PrintBitmap(bitmap);

	return bitmap;
}

void TextRenderer::PrintBitmap(Bitmap * bitmap)
{
	char * pixels = bitmap->pixels;
	
	int width = bitmap->width;
	int height = bitmap->getMaxY();
	
	//std::cout << "width: " << width << ";  height: " << height << endl;
	
	for ( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			char val = *(pixels + (x + y * width));
			
			char print = val == 0 ? ' ' : '#';
			cout << print; 
		}
		
		cout << endl; 
	}
}

Bitmap* TextRenderer::RenderChar(FT_Face face, int charCode)
{
	int error = 0;
	
	long glyph_index = FT_Get_Char_Index( face, charCode);
	
	error = FT_Load_Glyph(
            face,          /* handle to face object */
            glyph_index,   /* glyph index           */
            0 );  /* load flags, see below */
	if (error) {
		std::cout << "Could not load glyph!" << std::endl;
		return NULL;
	}
	
	error = FT_Render_Glyph( face->glyph,   /* glyph slot  */
                           FT_RENDER_MODE_MONO ); /* render mode */
	if (error) {
		std::cout << "Could not render glyph!" << std::endl;
		return NULL;
	}
	
	Bitmap *bitmap = new Bitmap(face->glyph->bitmap.width, face->glyph->bitmap.rows, Bitmap::TypeFromFTBitmapType(face->glyph->bitmap.pixel_mode));
	bitmap->Draw(face->glyph->bitmap, 0, 0);
	return bitmap;
}