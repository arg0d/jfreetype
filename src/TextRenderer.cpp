#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Manifest.h"
#include "Bitmap.cpp"

class TextRenderer {

public:
	/* Singleton */
	static TextRenderer *instance;
	
	Bitmap* Render(const std::string &font, const std::string &text, float size, int renderMode);

private:
	
	class TextMetrics {
	public:
		TextMetrics(int width = 0, int height = 0, int base_line = 0);

		int width, height, base_line;
	};

	static FT_Render_Mode GetFTRenderMode(int renderMode);

	Bitmap* RenderString(FT_Face face, const std::string& str, FT_Render_Mode renderModeFT);
	
	TextMetrics Meassure(FT_Face face, const std::string &string);

	FT_Library library;

	typedef std::map<std::string, FT_Face> TFaces;
	TFaces m_Faces;

};

TextRenderer* TextRenderer::instance = new TextRenderer();

TextRenderer::TextMetrics::TextMetrics(int width, int height, int base_line)
{
	this->width = width;
	this->height = height;
	this->base_line = base_line;
}

Bitmap* TextRenderer::Render(const std::string& font, const std::string& text, float size, int renderMode)
{
	FT_Face face;
	int error = 0;

	if (library == NULL) {
		error = FT_Init_FreeType( &library );
		if ( error )
		{
			std::cout << "Could no intialize library" << std::endl;
			return NULL;
		}
	}
	
	// if (m_Face == NULL) {
	// 	error = FT_New_Face( library, font.c_str(), 0, &face );
	// 	if (error) {
	// 		std::cout << "Could not load face!" << std::endl;
	// 		return NULL;
	// 	}
	// 	m_Face = face;
	// } else {
	// 	face = m_Face;
	// }
	face = m_Faces[font];
	if (face == NULL) {
		error = FT_New_Face( library, font.c_str(), 0, &face );

		if (error) {
			std::cout << "Could not load face! " << font << std::endl;
			return NULL;
		}

		m_Faces[font] = face;
	}
	
	error = FT_Set_Char_Size(
            face,    /* handle to face object           */
            0,       /* char_width in 1/64th of points  */
            size * 64,   /* char_height in 1/64th of points */
            72,     /* horizontal device resolution    */
            72 );   /* vertical device resolution      */
	if (error) {
		std::cout << "Could not set char size!" << std::endl;
		return NULL;
	}
	
	return RenderString(face, text, GetFTRenderMode(renderMode));

}

Bitmap* TextRenderer::RenderString(FT_Face face, const std::string& str, FT_Render_Mode renderModeFT)
{
	int error = 0;

	TextMetrics metrics = Meassure(face, str);
	Bitmap *bitmap = new Bitmap(metrics.width, metrics.height, BITMAP_TYPE_GRAY);

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
	                           	renderModeFT ); /* render mode */
		if (error) {
			std::cout << "Could not render glyph!" << std::endl;
			continue;
		}

		int h = face->glyph->metrics.height >> 6;
		int descender = h - (h + face->glyph->metrics.horiBearingY >> 6);
		
		bitmap->Draw(face->glyph->bitmap, x_pen, metrics.base_line - h + descender);

		x_pen += face->glyph->advance.x >> 6;

	}

	return bitmap;
}

TextRenderer::TextMetrics TextRenderer::Meassure(FT_Face face, const std::string &string)
{
	if (face == NULL) {
		return TextMetrics();
	}

	int width = 0;
	int height = 0;
	int base_line = 0;

	for (int i = 0; i < string.length(); i++) {
		long glyph_index = FT_Get_Char_Index(face, (int) string.at(i));

		if (glyph_index == 0) {
			return TextMetrics();
		}

		int error = FT_Load_Glyph(
		            face,          /* handle to face object */
		            glyph_index,   /* glyph index           */
		            0 );  /* load flags, see below */
		if (error) {
			std::cout << "Could not load glyph!" << std::endl;
			return TextMetrics();
		}

		width += face->glyph->advance.x >> 6;

		int h = face->glyph->metrics.height >> 6;
		int descender = h - (h + face->glyph->metrics.horiBearingY >> 6);

		base_line = std::max(base_line, h - descender);
		height = std::max(height, h + descender);
	}

	return TextMetrics(width, height, base_line);
	
}

FT_Render_Mode TextRenderer::GetFTRenderMode(int renderMode)
{
	switch (renderMode) {

		case 1:
			return FT_RENDER_MODE_MONO;	

		case 2:
			return FT_RENDER_MODE_NORMAL;

		default:
			return FT_RENDER_MODE_MONO;
	}	
}
