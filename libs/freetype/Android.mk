LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
SRC_PATH := $(LOCAL_PATH)/src
LOCAL_MODULE    := freetype
LOCAL_CFLAGS := -DFT2_BUILD_LIBRARY
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := \
	$(SRC_PATH)/autofit/autofit.c \
	$(SRC_PATH)/base/ftadvanc.c \
	$(SRC_PATH)/base/ftbbox.c \
	$(SRC_PATH)/base/ftbitmap.c \
	$(SRC_PATH)/base/ftcalc.c \
	$(SRC_PATH)/base/ftcid.c \
	$(SRC_PATH)/base/ftdbgmem.c \
	$(SRC_PATH)/base/ftdebug.c \
	$(SRC_PATH)/base/ftfstype.c \
	$(SRC_PATH)/base/ftgasp.c \
	$(SRC_PATH)/base/ftgloadr.c \
	$(SRC_PATH)/base/ftglyph.c \
	$(SRC_PATH)/base/ftgxval.c \
	$(SRC_PATH)/base/ftinit.c \
	$(SRC_PATH)/base/ftlcdfil.c \
	$(SRC_PATH)/base/ftmm.c \
	$(SRC_PATH)/base/ftobjs.c \
	$(SRC_PATH)/base/ftotval.c \
	$(SRC_PATH)/base/ftoutln.c \
	$(SRC_PATH)/base/ftpatent.c \
	$(SRC_PATH)/base/ftpfr.c \
	$(SRC_PATH)/base/ftrfork.c \
	$(SRC_PATH)/base/ftsnames.c \
	$(SRC_PATH)/base/ftstream.c \
	$(SRC_PATH)/base/ftstroke.c \
	$(SRC_PATH)/base/ftsynth.c \
	$(SRC_PATH)/base/ftsystem.c \
	$(SRC_PATH)/base/fttrigon.c \
	$(SRC_PATH)/base/fttype1.c \
	$(SRC_PATH)/base/ftutil.c \
	$(SRC_PATH)/base/ftwinfnt.c \
	$(SRC_PATH)/base/ftxf86.c \
	$(SRC_PATH)/bdf/bdf.c \
	$(SRC_PATH)/bzip2/ftbzip2.c \
	$(SRC_PATH)/cache/ftcache.c \
	$(SRC_PATH)/cff/cff.c \
	$(SRC_PATH)/cid/type1cid.c \
	$(SRC_PATH)/gzip/ftgzip.c \
	$(SRC_PATH)/lzw/ftlzw.c \
	$(SRC_PATH)/pcf/pcf.c \
	$(SRC_PATH)/pfr/pfr.c \
	$(SRC_PATH)/psaux/psaux.c \
	$(SRC_PATH)/pshinter/pshinter.c \
	$(SRC_PATH)/psnames/psmodule.c \
	$(SRC_PATH)/raster/raster.c \
	$(SRC_PATH)/sfnt/sfnt.c \
	$(SRC_PATH)/smooth/smooth.c \
	$(SRC_PATH)/truetype/truetype.c \
	$(SRC_PATH)/type1/type1.c \
	$(SRC_PATH)/type42/type42.c \
	$(SRC_PATH)/winfonts/winfnt.c


include $(BUILD_STATIC_LIBRARY)