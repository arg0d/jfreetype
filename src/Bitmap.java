package com.cig.jfreetype;

import java.nio.ByteBuffer;

public class Bitmap {

	public Bitmap() {
	}

	public int width;
	public int height;
	/** The buffer must be disposed, or it will cause memory leaks!*/
	public ByteBuffer pixels;
	public int format;
}