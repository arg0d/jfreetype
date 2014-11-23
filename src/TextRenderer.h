#pragma once

#include "TextRenderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Manifest.h"
#include "Bitmap.cpp"
#include <map>
#include "ftglyph.h"

#define TR_ALIGNMENT_LEFT 1
#define TR_ALIGNMENT_CENTER 2
#define TR_ALIGNMENT_RIGHT 3

class GlyphCache {

public:
												GlyphCache(FT_Face face);
												~GlyphCache();

	FT_Glyph									GetGlyph(int iCharCode);

private:

	typedef std::map<int, FT_Glyph>				TGlyphCache;
	TGlyphCache									m_Cache;
	FT_Face										m_Face;

};

class TextRenderer {

public:
	/* Singleton */
	static TextRenderer *instance;
	
	void										Render(Bitmap &bitmap, const std::string &font, const std::string &text, float size);
	Bitmap* 									Render(const std::string &font, const std::string &text, float size, int renderMode);
	Vector2 									Measure(const std::string &font, const std::string &text);

	Bitmap* 									RenderWrapped(const std::string &font, const std::string &text, float size, Vector2 bounds, int alignment, int renderMode);

private:
	
	class TextMetrics {
	public:
													TextMetrics(int width = 0, int height = 0, int base_line = 0);

		int 										width;
		int 										height;
		int 										base_line;
	};

	class WrappedTextMetrics {
	public:
		int 										width;
		int 										height;
		int 										line_height;
	};

	static FT_Render_Mode 						GetFTRenderMode(int renderMode);
	FT_Face 									GetFace(const std::string &font);
	void										RenderString(Bitmap &bitmap, FT_Face face, const std::string& str, const Vector2 &position, const TextMetrics &metrics, FT_Render_Mode renderModeFT);
	TextMetrics 								Measure(FT_Face face, const std::string &string);
	WrappedTextMetrics							WrapLines(std::vector<std::string> &output, Vector2 size, FT_Face face, const std::string &text);

	FT_Library 									library;

	typedef std::map<std::string, FT_Face> 		TFaces;
	TFaces 										m_Faces;

};