package com.cig.jfreetype;

import java.lang.reflect.Field;
import com.cig.jfreetype.Bitmap;

public class JFreeType {

	static {

		System.loadLibrary("jfreetype");
	}
	
	public static final int RENDER_MODE_MONO = 1;
	public static final int RENDER_MODE_NORMAL = 2;

	public static native final Bitmap render(String font, String str, float scale, int renderMode);

	public static void main(String[] args) {

		JFreeType freeType = new JFreeType();

		String text = "Testing";

		printBitmap(freeType.render("C:/Windows/Fonts/Tahoma.ttf", text, 20f, RENDER_MODE_MONO));
		printBitmap(freeType.render("C:/Windows/Fonts/Arial.ttf", text, 20f, RENDER_MODE_MONO));
		printBitmap(freeType.render("C:/Windows/Fonts/Tahoma.ttf", text, 20f, RENDER_MODE_MONO));
		printBitmap(freeType.render("C:/Windows/Fonts/Arial.ttf", text, 20f, RENDER_MODE_MONO));
		
	}

	private static void printBitmap(Bitmap bitmap) {
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