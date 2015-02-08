#pragma once

#include "TextRenderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "Manifest.h"
#include "Bitmap.cpp"
#include <map>

#define TR_ALIGNMENT_LEFT 1
#define TR_ALIGNMENT_CENTER 2
#define TR_ALIGNMENT_RIGHT 3

struct Glyph
{

public:
	FT_Glyph									ft_glyph;
	FT_BitmapGlyph								ft_bitmapGlyph;
	int 										width;
	int 										height;
	int 										ascender;
	int 										descender;
	int 										horiBearingX;
};

class GlyphCache {

public:
												GlyphCache(FT_Face face);
												~GlyphCache();

	std::string									GetGlyph(int iCharCode, Glyph *glyph);
	int 										GlyphToBitmap(Glyph &glyph);
	void										Clear();

private:

	typedef std::map<int, Glyph>				TGlyphCache;
	TGlyphCache									m_Cache;
	FT_Face										m_Face;

};

struct TextMetrics {
	int 										width;
	int 										height;
	int 										ascender;
	int 										descender;
};

class TextRenderer {

public:
	/* Singleton */
	static TextRenderer *instance;

												TextRenderer();
												~TextRenderer();

	bool										Initialize();
	bool										Cleanup();
	
	std::string 								Render(Bitmap &bitmap, const std::string &font, const std::string &text, int size, const Vector2 &bounds, int *descender);
	Vector2 									Measure(const std::string &font, const std::string &text);

	Bitmap* 									RenderWrapped(const std::string &font, const std::string &text, int size, Vector2 bounds, int lineSpacing, int alignment);

private:
	
	class WrappedTextMetrics {
	public:
		int 										width;
		int 										height;
		int 										line_height;
		int 										lineAscender;
	};

	FT_Face 									GetFace(const std::string &font);
	void										RenderString(Bitmap &bitmap, const std::string &text, const Vector2 &position, int ascender, GlyphCache &cache);
	void										Measure(const std::string &string, TextMetrics *metrics, GlyphCache &cache);
	WrappedTextMetrics							WrapLines(std::vector<std::string> &output, Vector2 size, const std::vector<std::string> &words, int lineSpacing, GlyphCache &cache);

	FT_Library 									library;

	typedef std::map<std::string, FT_Face> 		TFaces;
	TFaces 										m_Faces;

};