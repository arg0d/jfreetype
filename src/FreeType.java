package com.cig.jfreetype;

import java.lang.reflect.Field;
import com.cig.jfreetype.Bitmap;

public class FreeType {

	static {
		
		System.setProperty("java.library.path", System.getProperty("java.class.path"));

		// Hack, this will make a refresh of paths
		try {
			final Field sysPathsField = ClassLoader.class.getDeclaredField("sys_paths");
	    	sysPathsField.setAccessible(true);
	    	sysPathsField.set(null, null);
    	} catch (Exception e) {
    		e.printStackTrace();
    	}

		System.loadLibrary("lib");
	}
	
	public native Bitmap render(String str, float scale);

	public static void main(String[] args) {

		FreeType freeType = new FreeType();

		Bitmap bitmap = freeType.render("Random text", 0.25f);

		String str = "";
		for (int y = 0; y < bitmap.height; y++) {
			for (int x = 0; x < bitmap.width; x++) {
				byte value = bitmap.pixels[x + y * bitmap.width];

				char print = value == 0 ? '-' : ' ';
				str += print;
			}	
			str += "\n";
		}

		System.out.println(str);
	}
}