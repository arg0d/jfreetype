package com.cig.jfreetype;

import java.lang.reflect.Field;
import com.cig.jfreetype.Bitmap;

public class JFreeType {

	static {

		System.loadLibrary("jfreetype");
	}
	
	public static final int RENDER_MODE_MONO = 1;
	public static final int RENDER_MODE_NORMAL = 2;

	public static native final Bitmap render(String font, String text, float size);
	public static native final Vector2 meassure(String font, String str);

	public static native final Bitmap renderWrapped(String font, String text, float scale, int width, int height, int renderMode);

	public static void main(String[] args) {

		JFreeType freeType = new JFreeType();

		String font = "C:/Windows/Fonts/Arial.ttf";
		String text = "launch g";

		Bitmap bitmap = freeType.render(font, text, 30f);
		printBitmap(bitmap);

		System.out.println(bitmap.width+"  "+bitmap.height);		
	}

	private static void printBitmap(Bitmap bitmap) {
		String str = "";
		for (int y = 0; y < bitmap.height; y++) {
			for (int x = 0; x < bitmap.width; x++) {
				byte value = bitmap.pixels.get( (x + y * bitmap.width) * 4 + 3 );

				char print = value == 0 ? '@' : ' ';
				str += print;
			}	
			str += "\n";
		}

		System.out.println(str);

	}
}