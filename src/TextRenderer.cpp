#pragma once

#include "TextRenderer.h"

TextRenderer* TextRenderer::instance = new TextRenderer();

TextRenderer::TextMetrics::TextMetrics(int width, int height, int base_line)
{
	this->width = width;
	this->height = height;
	this->base_line = base_line;
}

FT_Face TextRenderer::GetFace(const std::string &font)
{
	FT_Face face = m_Faces[font];
	if (face == NULL) {
		int error = FT_New_Face( library, font.c_str(), 0, &face );

		if (error) {
			std::cout << "Could not load face! " << font << std::endl;
			return NULL;
		}

		m_Faces[font] = face;
	}
	return face;
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
	
	face = GetFace(font);
	
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
	
	TextMetrics metrics = Measure(face, text);
	Bitmap *bitmap = new Bitmap(metrics.width, metrics.height);
	RenderString(*bitmap, face, text, Vector2(), metrics, GetFTRenderMode(renderMode));
	return bitmap;
}

void TextRenderer::Render(Bitmap &bitmap, const std::string &font, const std::string &text, float size)
{
	int error = 0;

	if (library == NULL) {
		error = FT_Init_FreeType( &library );
		if ( error )
		{
			std::cout << "Could no intialize library" << std::endl;
			return;
		}
	}

	FT_Face face = GetFace(font);

	error = FT_Set_Char_Size(
            face,    /* handle to face object           */
            0,       /* char_width in 1/64th of points  */
            size * 64,   /* char_height in 1/64th of points */
            72,     /* horizontal device resolution    */
            72 );   /* vertical device resolution      */
	if (error) {
		std::cout << "Could not set char size!" << std::endl;
		return;
	}

	TextMetrics metrics = Measure(face, text);
	bitmap.Initialize(metrics.width, metrics.height);
	RenderString(bitmap, face, text, Vector2(), metrics, FT_RENDER_MODE_NORMAL);
}

Vector2 TextRenderer::Measure(const std::string &font, const std::string &text)
{
	Vector2 size;
	FT_Face face;

	face = GetFace(font);

	TextMetrics metrics = Measure(face, text);
	size.x = metrics.width;
	size.y = metrics.height;	

	return size;
}

Bitmap* TextRenderer::RenderWrapped(const std::string &font, const std::string &text, float size, Vector2 bounds, int alignment, int renderMode)
{
	int error = 0;

	if (library == NULL) {
		error = FT_Init_FreeType( &library );
		if ( error )
		{
			std::cout << "Could no intialize library" << std::endl;
			return NULL;
		}
	}

	FT_Face face = GetFace(font);

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

	std::vector<std::string> lines;
	WrappedTextMetrics wrappedTextMetrics = WrapLines(lines, bounds, face, text);

	Bitmap* bitmap = new Bitmap(wrappedTextMetrics.width, wrappedTextMetrics.height);

	int y = 0;
	for (unsigned int i = 0; i < lines.size(); i++)
	{	
		std::string str = lines.at(i);
		TextMetrics metrics = Measure(face, str);

		int horizontalOffset = 0;

		if (alignment == TR_ALIGNMENT_LEFT)
		{
		}
		else if (alignment == TR_ALIGNMENT_CENTER)
		{
			horizontalOffset = wrappedTextMetrics.width / 2 - metrics.width / 2;
		}
		else if (alignment == TR_ALIGNMENT_RIGHT)
		{
			horizontalOffset = wrappedTextMetrics.height - metrics.width;
		}
		else
		{
			Log("Invalid wrapping alignment!");
		}

		RenderString(*bitmap, face, str, Vector2(horizontalOffset, y), metrics, GetFTRenderMode(renderMode));
		y += wrappedTextMetrics.line_height;
	}

	return bitmap;
}

void TextRenderer::RenderString(Bitmap &bitmap, FT_Face face, const std::string &str, const Vector2 &position, const TextMetrics &metrics, FT_Render_Mode renderModeFT)
{
	int error = 0;
	int x_pen = 0;

	for (unsigned int i = 0; i < str.length(); i++)
	{
		long glyph_index = FT_Get_Char_Index( face, (int) str.at(i));
	
		error = FT_Load_Glyph(
	            face,          /* handle to face object */
	            glyph_index,   /* glyph index           */
	            0 );  /* load flags, see below */
		if (error)
		{
			std::cout << "Could not load glyph! " << error << std::endl;
			continue;
		}
		
		error = FT_Render_Glyph( face->glyph,   /* glyph slot  */
	                           	renderModeFT ); /* render mode */
		if (error)
		{
			std::cout << "Could not render glyph!" << std::endl;
			continue;
		}

		int h = face->glyph->metrics.height >> 6;
		int descender = h - (face->glyph->metrics.horiBearingY >> 6);
		
		bitmap.Draw(face->glyph->bitmap, x_pen + position.GetX(), metrics.base_line - h + descender + position.GetY());

		x_pen += face->glyph->advance.x >> 6;

	}
}

TextRenderer::TextMetrics TextRenderer::Measure(FT_Face face, const std::string &string)
{
	if (face == NULL) {
		return TextMetrics();
	}

	int width = 0;
	int height = 0;
	int base_line = 0;

	for (unsigned int i = 0; i < string.length(); i++) {
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
		int descender = h - (face->glyph->metrics.horiBearingY >> 6);

		base_line = std::max(base_line, h - descender);
		height = std::max(height, h + descender);
	}

	return TextMetrics(width, height, base_line);
}

TextRenderer::WrappedTextMetrics TextRenderer::WrapLines(std::vector<std::string> &output, Vector2 bounds, FT_Face face, const std::string &text)
{
	int totalHeight = 0; // output variable
	const int spaceWidth = Measure(face, " ").width;
	int maxHeight = 0;

	std::vector<std::string> words;
	String_Split(words, text, ' ');

	std::string currentLine;
	int width = 0; // loop variable
	int height = 0; // loop variable
	for (unsigned int i = 0; i < words.size(); i++)
	{
		std::string word = words.at(i);
		TextMetrics metrics = Measure(face, word);

		bool addSpace;
		addSpace = i == 0 ? false : true; // Don't add space on the first word

		if (width + metrics.width > bounds.x)
		{
			output.push_back(currentLine);

			currentLine = "";
			totalHeight += height;
			width = 0;
			height = 0;

			addSpace = false;
		}

		if (addSpace)
		{
			currentLine += " ";	
			width += spaceWidth;
		}
		currentLine += word;
		width += metrics.width;
		height = std::max(height, metrics.height);
		maxHeight = std::max(maxHeight, metrics.height);
	}

	output.push_back(currentLine);
	totalHeight += height;

	WrappedTextMetrics wrappedTextMetrics;
	wrappedTextMetrics.width = bounds.x;
	wrappedTextMetrics.height = totalHeight;
	wrappedTextMetrics.line_height = maxHeight;
	return wrappedTextMetrics;
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




//=========================================================================================
// TextMetricsCache
//=========================================================================================

GlyphCache::GlyphCache(FT_Face face):
 m_Face(face)
{
}

GlyphCache::~GlyphCache()
{
	TGlyphCache::iterator it = m_Cache.begin();
	for (; it != m_Cache.end(); it++)
	{
		FT_Done_Glyph(it->second);
	}
}

FT_Glyph GlyphCache::GetGlyph(int iCharCode)
{
	TGlyphCache::iterator it = m_Cache.find(iCharCode);
	if (it != m_Cache.end())
	{
		return it->second;
	}

	// Add glyph to cache
	int error = 0;

	FT_UInt uiGlyphIndex = FT_Get_Char_Index(m_Face, iCharCode);
	if (uiGlyphIndex != 0)
	{
		error = FT_Load_Glyph(m_Face, uiGlyphIndex, FT_LOAD_DEFAULT);
		if (error != 0)
		{
			Log("GlyphCache couldn't load glyph with charcode<%d>", iCharCode);
			return NULL;
		}

		FT_Glyph glyph;
		error = FT_Get_Glyph(m_Face->glyph, &glyph);
		if (error != 0)
		{
			Log("GlyphCache couldn't copy glyph from glyph slot for charcode<%d>", iCharCode);
			return NULL;
		}

		// Glyph has been cached succesfully
		m_Cache[iCharCode] = glyph;
		return glyph;
	}
	else
	{
		Log("GlyphCache couldn't find glyph index for charcode<%d>", iCharCode);
		return NULL;
	}
}