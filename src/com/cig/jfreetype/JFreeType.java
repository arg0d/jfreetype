package com.cig.jfreetype;

import java.lang.reflect.Field;
import com.cig.jfreetype.Bitmap;
import java.lang.IllegalStateException;

public final class JFreeType {

	private static native boolean createImpl();
	private static native boolean destroyImpl();

	private static native Bitmap render(String font, String text, int size);
	private static native Vector2 measure(String font, String str);
	private static native Bitmap renderWrapped(String font, String text, int size, int width, int height, int lineSpacing);
	private static native String getLogImpl();

	private static boolean isInstanceCreated = false;
	private static boolean isInstanceValid = false;

	private boolean isCreated = false;

	public JFreeType() {
		if (!isInstanceValid) {
			throw new IllegalStateException("Must use JFreeType.createInstance() to create JFreeType instance!");
		}
	}

	public boolean initialize(boolean loadNatives) {
		if (isCreated) {
			throw new IllegalStateException("JFreeType instance is already initialized!");
		}

		if (loadNatives) {
			String architecture = System.getProperty("os.arch");
			if (architecture.equals("x32") || architecture.equals("x86")) {
				System.loadLibrary("jfreetype32");
			} else if (architecture.equals("x64")) {
				System.loadLibrary("jfreetype64");
			}
		}

		if (createImpl()) {
			isCreated = true;
			return true;
		} else 
			return false;
	}

	public boolean destroy() {
		if (!isCreated) {
			throw new IllegalStateException("JFreeType instance has not been created yet!");
		}

		if (destroyImpl()) {
			isCreated = false;
			return true;
		} else
			return false;
	}	

	public Bitmap render(JFreeTypeOptions options) {
		return render(options.font, options.text, options.size);
	}

	public Vector2 measure(JFreeTypeOptions options) {
		return measure(options.font, options.text);
	}

	public Bitmap renderWrapped(JFreeTypeOptions options) {
		return renderWrapped(options.font, options.text, options.size, options.width, options.height, options.lineSpacing);
	}

	public String getLog() {
		return getLogImpl();
	}

	public static void main(String[] args) {

		JFreeType freetype = JFreeType.createInstance();

		System.loadLibrary("jfreetype");
		freetype.initialize(false);

		JFreeTypeOptions options = new JFreeTypeOptions();
		options.font = "C:/Windows/Fonts/Arial.ttf";
		options.text = "p you you";
		options.size = 25;
		options.width = 70;
		options.height = 10;
		options.lineSpacing = 0;
		Bitmap bitmap = freetype.renderWrapped(options);
		printBitmap(bitmap);

		freetype.destroy();

		String log = null;
		while ((log = freetype.getLog()) != null) {
			System.out.println("JFreeType log: " + log);
		}

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

	/**
	* Creates instance of JFreeType. Can only call this method 1 time.
	* IllegalStateException will be thrown otherwise.
	*/
	public static JFreeType createInstance() throws IllegalStateException {
		if (!isInstanceCreated) {
			isInstanceCreated = true;
			isInstanceValid = true;
			JFreeType instance = new JFreeType();
			isInstanceValid = false;
			return instance;
		} else throw new IllegalStateException("Only 1 instance of JFreeType can be created!");
	}

	public static class JFreeTypeOptions {
		public String font;
		public String text;
		public int size;
		public int width;
		public int height;
		public int lineSpacing;
	}

}