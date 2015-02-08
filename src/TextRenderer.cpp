#pragma once

#include "TextRenderer.h"

#include "memory"

FT_Face TextRenderer::GetFace(const std::string &font)
{
	FT_Face face = m_Faces[font];
	if (face == NULL) {
		int error = FT_New_Face( library, font.c_str(), 0, &face );

		if (error) {
			Log("Could not load face! %s", font.c_str());
			return NULL;
		}

		m_Faces[font] = face;
	}
	return face;
}

TextRenderer* TextRenderer::instance = NULL;

TextRenderer::TextRenderer()
{
}

TextRenderer::~TextRenderer()
{
}

bool TextRenderer::Initialize()
{
	int error = FT_Init_FreeType(&library);
	if (error != 0)
	{
		Log("Could not initialize FreeType library. FreeType errorCode=%d", error);
		return false;
	}
	return true;
}

bool TextRenderer::Cleanup()
{
	int error = FT_Done_FreeType(library);
	if (error != 0)
	{
		Log("Could not destroy FreeType library. FreeType errorCode=%d", error);
		return false;
	}
	return true;
}

std::string TextRenderer::Render(Bitmap &bitmap, const std::string &font, const std::string &text, int size, const Vector2 &bounds, int *descender)
{
	int error = 0;

	FT_Face face = GetFace(font);

	TextMetrics metrics;
	GlyphCache cache(face);

	while (true)
	{
		if (size == 0)
		{
			// Since size is 0, just quit, because font is not visible anymore
			bitmap.Initialize(0, 0);
			Log("Text: <%s> does not fit!", text.c_str());
			return "";
		}

		error = FT_Set_Char_Size(face, size * 64, size * 64, 72, 72);
		if (error) {
			return Format("Could not set FreeType char size. Freetype errorCode=%d", error);
		}

		// Cache needs to be cleared when size of the font changes,
		// so that glyphs update their metrics.
		cache.Clear();

		Measure(text, &metrics, cache);

		if (metrics.width <= bounds.x && metrics.height <= bounds.y)
		{
			// Text fits within the bounds
			break;
		}
		else
		{
			// Text doesn't fit within the bounds, reduce size
			size--;
		}
	}

	bitmap.Initialize(metrics.width, metrics.height);
	RenderString(bitmap, text, Vector2(), metrics.ascender, cache);

	if (descender != NULL)
	{
		(*descender) = metrics.descender;
	}
	return "";
}

Vector2 TextRenderer::Measure(const std::string &font, const std::string &text)
{
	Vector2 size;
	FT_Face face;

	face = GetFace(font);

	TextMetrics metrics;
	GlyphCache cache(face);
	Measure(text, &metrics, cache);
	size.x = metrics.width;
	size.y = metrics.height;	

	return size;
}

Bitmap* TextRenderer::RenderWrapped(const std::string &font, const std::string &text, int size, Vector2 bounds, int lineSpacing, int alignment)
{
	std::vector<std::string> words;
	String_Split(words, text, ' ');

	int error = 0;
	FT_Face face = GetFace(font);

	GlyphCache cache(face);
	std::vector<std::string> lines;
	WrappedTextMetrics wrappedTextMetrics;

	while (true)
	{	
		if (size == 0)
		{
			return new Bitmap(0, 0);
		}

		error = FT_Set_Char_Size(face, 0, size * 64, 72, 72);
		if (error) {
			Log("TextRenderer::RenderWrapped() - cannot set char size! Returning null");
			return NULL;
		}

		// Must reset glyph cache so that glyph metrics reset for new font size
		cache.Clear();
		lines.clear();
		wrappedTextMetrics = WrapLines(lines, bounds, words, lineSpacing, cache);

		if (wrappedTextMetrics.width <= bounds.x && wrappedTextMetrics.height <= bounds.y)
		{
			break;
		}
		else
		{
			size--;
		}
	}

	Bitmap* bitmap = new Bitmap(wrappedTextMetrics.width, wrappedTextMetrics.height);

	int y = 0;
	for (unsigned int i = 0; i < lines.size(); i++)
	{	
		std::string str = lines.at(i);
		TextMetrics metrics;
		Measure(str, &metrics, cache);

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

		RenderString(*bitmap, str, Vector2(horizontalOffset, y), wrappedTextMetrics.lineAscender, cache);
		y += wrappedTextMetrics.line_height;
	}

	return bitmap;
}

void TextRenderer::RenderString(Bitmap &bitmap, const std::string &text, const Vector2 &position, int ascender, GlyphCache &cache)
{
	int error = 0;

	int pen_x = 0;
	for (unsigned int i = 0; i < text.length(); i++)
	{
		Glyph glyph;
		std::string strError = cache.GetGlyph((int) text.at(i), &glyph);
		if (strError.empty())
		{
			error = cache.GlyphToBitmap(glyph);
			if (error == 0)
			{
				bitmap.Draw(glyph.ft_bitmapGlyph->bitmap, pen_x + position.GetX() + glyph.horiBearingX, ascender - glyph.height + glyph.descender + position.GetY());
				pen_x += glyph.width;
			}
		}
	}
}

void TextRenderer::Measure(const std::string &string, TextMetrics *metrics, GlyphCache &cache)
{
	int width = 0;
	int ascender = 0;
	int descender = 0;

	for (unsigned int i = 0; i < string.length(); i++) {
		Glyph glyph;
		std::string strError = cache.GetGlyph((int) string.at(i), &glyph);
		if (strError.empty())
		{
			width += glyph.width;
			ascender = std::max(ascender, glyph.ascender);
			descender = std::max(descender, glyph.descender);
		}
	}

	metrics->width = width;
	metrics->height = ascender + descender;
	metrics->ascender = ascender;
	metrics->descender = descender;
}

TextRenderer::WrappedTextMetrics TextRenderer::WrapLines(std::vector<std::string> &output, Vector2 bounds, const std::vector<std::string> &words, int lineSpacing, GlyphCache &cache)
{
	TextMetrics spaceMetrics;
	Measure(" ", &spaceMetrics, cache);
	const int spaceWidth = spaceMetrics.width;
	int maxLineWidth = 0;
	int maxLineHeight = 0;
	int maxLineAscender = 0;

	std::string currentLine;
	int width = 0; // loop variable
	int ascender = 0; // loop variable
	int descender = 0; // loop variable
	for (unsigned int i = 0; i < words.size(); i++)
	{
		std::string word = words.at(i);
		TextMetrics metrics;
		Measure(word, &metrics, cache);

		bool addSpace;
		addSpace = i == 0 ? false : true; // Don't add space on the first word

		if (width + metrics.width > bounds.x)
		{
// ************************ line has ended. Edit the code below when editing here
			output.push_back(currentLine);
			maxLineWidth = std::max(maxLineWidth, width);
			maxLineHeight = std::max(maxLineHeight, ascender + descender);
			maxLineAscender = std::max(maxLineAscender, ascender);
// *************************************************************************************

			currentLine = "";
			width = 0;
			ascender = 0;
			descender = 0;

			addSpace = false;
		}

		if (addSpace)
		{
			currentLine += " ";	
			width += spaceWidth;
		}
		currentLine += word;
		width += metrics.width;
		ascender = std::max(ascender, metrics.ascender);
		descender = std::max(descender, metrics.descender);
	}

// ************************ the line ended code must be the same as above
	output.push_back(currentLine);
	maxLineWidth = std::max(maxLineWidth, width);
	maxLineHeight = std::max(maxLineHeight, ascender + descender);
	maxLineAscender = std::max(maxLineAscender, ascender);
// *************************************************************************

	WrappedTextMetrics wrappedTextMetrics;
	wrappedTextMetrics.width = maxLineWidth;
	wrappedTextMetrics.height = maxLineHeight * output.size() + lineSpacing;
	wrappedTextMetrics.line_height = maxLineHeight;
	wrappedTextMetrics.lineAscender = maxLineAscender;
	return wrappedTextMetrics;
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
	Clear();
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
		glyph.horiBearingX = m_Face->glyph->metrics.horiBearingX >> 6;
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

void GlyphCache::Clear()
{
	TGlyphCache::iterator it = m_Cache.begin();
	for (; it != m_Cache.end(); ++it)
	{
		FT_Done_Glyph(it->second.ft_glyph);
		if (it->second.ft_bitmapGlyph != NULL)
		{
			FT_Done_Glyph(reinterpret_cast<FT_Glyph>(it->second.ft_bitmapGlyph));
		}
	}
	m_Cache.clear();
}