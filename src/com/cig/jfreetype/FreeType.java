package com.cig.jfreetype;

import java.lang.reflect.Field;
import com.cig.jfreetype.Bitmap;

public class FreeType {

	static {

		System.loadLibrary("lib");
	}
	
	public static final int RENDER_MODE_MONO = 1;
	public static final int RENDER_MODE_NORMAL = 2;

	public static native final Bitmap render(String font, String str, float scale, int renderMode);

	public static void main(String[] args) {

		FreeType freeType = new FreeType();

		Bitmap bitmap = freeType.render("C:/Windows/Fonts/Tahoma.ttf", "Chris g", 20f, RENDER_MODE_MONO);

		String str = "";
		for (int y = 0; y < bitmap.height; y++) {
			for (int x = 0; x < bitmap.width; x++) {
				byte value = bitmap.pixels.get(x + y * bitmap.width);

				char print = value == 0 ? '@' : ' ';
				str += print;
			}	
			str += "\n";
		}

		System.out.println(str);
	}
}