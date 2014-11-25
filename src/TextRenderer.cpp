#pragma once

#include "TextRenderer.h"

#include "memory"

TextRenderer* TextRenderer::instance = new TextRenderer();

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

std::string TextRenderer::Render(Bitmap &bitmap, const std::string &font, const std::string &text, float size)
{

	int error = 0;

	if (library == NULL) {
		error = FT_Init_FreeType( &library );
		if ( error )
		{
			return Format("Could not initalize library errorCode=%d", error);
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
		return Format("Could not set char size errorCode=%d", error);
	}

	TextMetrics metrics;
	Measure(face, text, &metrics);
	bitmap.Initialize(metrics.width, metrics.height);
	RenderString(bitmap, face, text, Vector2(), metrics.ascender);

	return "";
}

Vector2 TextRenderer::Measure(const std::string &font, const std::string &text)
{
	Vector2 size;
	FT_Face face;

	face = GetFace(font);

	TextMetrics metrics;
	Measure(face, text, &metrics);
	size.x = metrics.width;
	size.y = metrics.height;	

	return size;
}

Bitmap* TextRenderer::RenderWrapped(const std::string &font, const std::string &text, float size, Vector2 bounds, int alignment)
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
		TextMetrics metrics;
		Measure(face, str, &metrics);

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

		RenderString(*bitmap, face, str, Vector2(horizontalOffset, y), metrics, FT_RENDER_MODE_NORMAL);
		y += wrappedTextMetrics.line_height;
	}

	return bitmap;
}

void TextRenderer::RenderString(Bitmap &bitmap, FT_Face face, const std::string &text, const Vector2 &position, int ascender, GlyphCache* cache)
{
	bool bDeleteCache = false;
	if (cache == NULL)
	{
		cache = new GlyphCache(face);
		bDeleteCache = true;
	}

	int error = 0;

	int pen_x = 0;
	for (unsigned int i = 0; i < text.length(); i++)
	{
		Glyph glyph;
		std::string strError = cache->GetGlyph((int) text.at(i), &glyph);
		if (strError.empty())
		{
			error = cache->GlyphToBitmap(glyph);
			if (error == 0)
			{
				bitmap.Draw(glyph.ft_bitmapGlyph->bitmap, pen_x, ascender - glyph.height + glyph.descender);
				pen_x += glyph.width;
			}
		}
	}

	if (bDeleteCache)
	{
		delete cache;
	}
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
		
		bitmap.Draw(face->glyph->bitmap, x_pen + position.GetX(), metrics.ascender - h + descender + position.GetY());

		x_pen += face->glyph->advance.x >> 6;

	}
}

std::string TextRenderer::Measure(FT_Face face, const std::string &string, TextMetrics *metrics, GlyphCache* cache)
{
	// Hack, want to delete cache if it was created.
	// If it was passed to this function, don't delete it
	bool bDeleteCachce = false;

	if (cache == NULL)
	{
		cache = new GlyphCache(face);
		bDeleteCachce = true; // Delete cache once we are done
	}

	int width = 0;
	int height = 0;
	int ascender = 0;

	for (unsigned int i = 0; i < string.length(); i++) {
		Glyph glyph;
		std::string strError = cache->GetGlyph((int) string.at(i), &glyph);
		if (strError.empty())
		{
			width += glyph.width;
			height = std::max(glyph.height + glyph.descender, height);
			ascender = std::max(glyph.ascender, ascender);
		}
	}

	if (bDeleteCachce)
	{
		// Delete cache since this function created it
		delete cache;
	}

	metrics->width = width;
	metrics->height = height;
	metrics->ascender = ascender;
	return "";
}

TextRenderer::WrappedTextMetrics TextRenderer::WrapLines(std::vector<std::string> &output, Vector2 bounds, FT_Face face, const std::string &text)
{
	int totalHeight = 0; // output variable
	TextMetrics spaceMetrics;
	Measure(face, " ", &spaceMetrics);
	const int spaceWidth = spaceMetrics.width;
	int maxHeight = 0;

	std::vector<std::string> words;
	String_Split(words, text, ' ');

	std::string currentLine;
	int width = 0; // loop variable
	int height = 0; // loop variable
	for (unsigned int i = 0; i < words.size(); i++)
	{
		std::string word = words.at(i);
		TextMetrics metrics;
		Measure(face, word, &metrics);

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
		FT_Done_Glyph(it->second.ft_glyph);
		if (it->second.ft_bitmapGlyph != NULL)
		{
			FT_Done_Glyph(reinterpret_cast<FT_Glyph>(it->second.ft_bitmapGlyph));
		}
	}
}

std::string GlyphCache::GetGlyph(int iCharCode, Glyph* pGlyph)
{
	TGlyphCache::iterator it = m_Cache.find(iCharCode);
	if (it != m_Cache.end())
	{
		(*pGlyph) = it->second; // set the output glyph
		return "";
	}

	// Add glyph to cache
	int error = 0;

	FT_UInt uiGlyphIndex = FT_Get_Char_Index(m_Face, iCharCode);
	if (uiGlyphIndex != 0)
	{
		error = FT_Load_Glyph(m_Face, uiGlyphIndex, FT_LOAD_DEFAULT);
		if (error != 0)
		{
			return Format("GlyphCache could not load glyph charcode=%d", iCharCode);
		}

		FT_Glyph ft_glyph;
		error = FT_Get_Glyph(m_Face->glyph, &ft_glyph);
		if (error != 0)
		{
			return Format("GlyphCache could not get glyph charcode=%d", iCharCode);
		}

		// Glyph has been cached succesfully
		Glyph glyph;
		glyph.ft_glyph = ft_glyph;
		glyph.ft_bitmapGlyph = NULL;
		glyph.width = m_Face->glyph->advance.x >> 6;
		glyph.height = m_Face->glyph->metrics.height >> 6;
		glyph.ascender = m_Face->glyph->metrics.horiBearingY >> 6;
		glyph.descender = glyph.height - glyph.ascender;
		m_Cache[iCharCode] = glyph;
		(*pGlyph) = glyph; // set the output glyph
		return "";
	}
	else
	{
		return Format("GlyphCache Could not get char index charcode=%d", iCharCode);
	}
}

int GlyphCache::GlyphToBitmap(Glyph &glyph)
{
	if (glyph.ft_bitmapGlyph != NULL)
	{
		return 0;	// already rendered, 0 indicates success just like freetype
	}

	FT_Glyph ft_glyph = glyph.ft_glyph;
	FT_Vector offset;
	offset.x = 0;
	offset.y = 0;

	int error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, &offset, false);
	if (error == 0)
	{
		glyph.ft_bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(ft_glyph);
		return 0; // success
	}

	return error; // failed
}