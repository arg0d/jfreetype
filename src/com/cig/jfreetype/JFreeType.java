package com.cig.jfreetype;

public final class JFreeType {

	private static native boolean createImpl();
	private static native boolean destroyImpl();

	private static native Bitmap render(String font, String text, int size, int width, int height);
	private static native TextMetrics measure(String font, String str);
	private static native Bitmap renderWrapped(String font, String text, int size, int width, int height, int lineSpacing);
	private static native String getLogImpl();
	private static native void setInstantLoggingEnabledImpl(boolean instantLoggingEnabled);

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
			String architecture = System.getProperty("sun.arch.data.model");
			System.loadLibrary("jfreetype" + architecture);
		}

		if (createImpl()) {
			isCreated = true;
			return true;
		} else 
			return false;
	}

	public boolean destroy() {
		validateState();

		if (destroyImpl()) {
			isCreated = false;
			return true;
		} else
			return false;
	}	

	public Bitmap render(JFreeTypeOptions options) {
		return render(options.font, options.text, options.size, options.width, options.height);
	}

	/**
	 * In theory should measure the string specified.
	 * <p>
	 * THIS METHOD MIGHT NOT BE WORKING PROPERLY.
	 * IT IS NOT TESTED TO FULL EXTENT YET.
	 * IT WILL ONLY BE FULLY TESTED AFTER ALL OTHER FEATURES ARE DONE.
	 * TRY TO USE OTHER METHODS TO COMPENSATE FOR THIS AS THAT, THAT 
	 * SHOULD BE POSSIBLE.
	 * <p>
	 * @param options
	 * @return
	 */
	public TextMetrics measure(JFreeTypeOptions options) {
		validateState();
		return measure(options.font, options.text);
	}

	public Bitmap renderWrapped(JFreeTypeOptions options) {
		validateState();
		return renderWrapped(options.font, options.text, options.size, options.width, options.height, options.lineSpacing);
	}

	public String getLog() {
		validateState();
		return getLogImpl();
	}
	
	/**
	 * If set to true will print out log message of JFreeType instantly without
	 * caching them for the retrieval via getLog() method.
	 * <p>
	 * All the logs start with JFreeType tag when instant logging.
	 * <p>
	 * @param instantLoggingEnabled true to enable instant logging.
	 */
	public void setInstantLoggingEnabled(boolean instantLoggingEnabled) {
		validateState();
		setInstantLoggingEnabledImpl(instantLoggingEnabled);
	}

	private void validateState() {
		if (!isCreated) {
			throw new IllegalStateException("JFreeType has not been initialized yet! Call JFreeType.initialize method");
		}
	}
	
	public static void main(String[] args) {
		JFreeType freetype = JFreeType.createInstance();

		System.loadLibrary("jfreetype");
		freetype.initialize(false);
		freetype.setInstantLoggingEnabled(true);

		JFreeTypeOptions options = new JFreeTypeOptions();
		options.font = "C:/Windows/Fonts/Arial.ttf";
		options.text = "W q p";
		options.size = 15;
		options.width = 30;
		options.height = 10000;
		options.lineSpacing = 0;
		Bitmap bitmap = freetype.renderWrapped(options);
		printBitmap(bitmap);

		String log = null;
		while ((log = freetype.getLog()) != null) {
			System.out.println("JFreeType log: " + log);
		}

		freetype.destroy();
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
		/**
		 * Path to font file on the file system 
		 */
		public String font;
		
		/**
		 * String of text to be rendered
		 */
		public String text;
		
		/**
		 * Initial size of the font (might be shrunk if text
		 * is too large after rendering)
		 */
		public int size;
		
		/**
		 * Maximum rendered text width
		 */
		public int width;
		
		/**
		 * Maximum rendered text height
		 */
		public int height;
		
		/**
		 * Space between lines in pixels
		 */
		public int lineSpacing;
	}

}