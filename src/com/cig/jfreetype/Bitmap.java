package com.cig.jfreetype;

import java.nio.ByteBuffer;

public class Bitmap {

	public int width;
	public int height;
	public ByteBuffer pixels;
	public int format;
	public TextMetrics textMetrics;
	
	/**
	 * Not sure if this must be defined for native code to be able to create
	 * new object with empty constructor parameters.
	 */
	public Bitmap() {
	}

}