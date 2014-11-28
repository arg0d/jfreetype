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

std::string TextRenderer::Render(Bitmap &bitmap, const std::string &font, const std::string &text, int size)
{
	int error = 0;

	FT_Face face = GetFace(font);

	error = FT_Set_Char_Size(face, size * 64, size * 64, 72, 72);
	if (error) {
		return Format("Could not set FreeType char size. Freetype errorCode=%d", error);
	}

	TextMetrics metrics;
	GlyphCache cache(face);
	Measure(text, &metrics, &cache);
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
	GlyphCache cache(face);
	Measure(text, &metrics, &cache);
	size.x = metrics.width;
	size.y = metrics.height;	

	return size;
}

Bitmap* TextRenderer::RenderWrapped(const std::string &font, const std::string &text, int size, Vector2 bounds, int lineSpacing, int alignment)
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

	error = FT_Set_Char_Size(face, 0, size * 64, 72, 72);
	if (error) {
		std::cout << "Could not set char size!" << std::endl;
		return NULL;
	}

	GlyphCache cache(face);

	std::vector<std::string> lines;
	WrappedTextMetrics wrappedTextMetrics = WrapLines(lines, bounds, text, lineSpacing, &cache);

	Bitmap* bitmap = new Bitmap(wrappedTextMetrics.width, wrappedTextMetrics.height);

	int y = 0;
	for (unsigned int i = 0; i < lines.size(); i++)
	{	
		std::string str = lines.at(i);
		TextMetrics metrics;
		Measure(str, &metrics, &cache);

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

		RenderString(*bitmap, face, str, Vector2(horizontalOffset, y), metrics.ascender);
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
				bitmap.Draw(glyph.ft_bitmapGlyph->bitmap, pen_x + position.GetX(), ascender - glyph.height + glyph.descender + position.GetY());
				pen_x += glyph.width;
			}
		}
	}

	if (bDeleteCache)
	{
		delete cache;
	}
}

void TextRenderer::Measure(const std::string &string, TextMetrics *metrics, GlyphCache* cache)
{
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

	metrics->width = width;
	metrics->height = height;
	metrics->ascender = ascender;
}

TextRenderer::WrappedTextMetrics TextRenderer::WrapLines(std::vector<std::string> &output, Vector2 bounds, const std::string &text, int lineSpacing, GlyphCache* cache)
{
	TextMetrics spaceMetrics;
	Measure(" ", &spaceMetrics, cache);
	const int spaceWidth = spaceMetrics.width;
	int maxLineHeight = 0;

	std::vector<std::string> words;
	String_Split(words, text, ' ');

	std::string currentLine;
	int width = 0; // loop variable
	int height = 0; // loop variable
	for (unsigned int i = 0; i < words.size(); i++)
	{
		std::string word = words.at(i);
		TextMetrics metrics;
		Measure(word, &metrics, cache);

		bool addSpace;
		addSpace = i == 0 ? false : true; // Don't add space on the first word

		if (width + metrics.width > bounds.x)
		{
			output.push_back(currentLine);

			currentLine = "";
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
		maxLineHeight = std::max(maxLineHeight, metrics.height);
	}

	output.push_back(currentLine);
	maxLineHeight += lineSpacing;

	WrappedTextMetrics wrappedTextMetrics;
	wrappedTextMetrics.width = bounds.x;
	wrappedTextMetrics.height = maxLineHeight * output.size();
	wrappedTextMetrics.line_height = maxLineHeight;
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