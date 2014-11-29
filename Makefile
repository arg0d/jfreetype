all: compile
	
compile:
	make -C libs/freetype
	g++ -Wall -Wl,--kill-at -static-libgcc -static-libstdc++ -Isrc -Ilibs/jni/include -Ilibs/freetype/include -shared -o bin/jfreetype.dll src/FreeType.cpp libs/freetype/objs/freetype.a
	ant compile
	java -Djava.library.path=bin -classpath bin com/cig/jfreetype/JFreeType

run:
	java -classpath bin com/cig/jfreetype/JFreeType

jar:
	ant build-jar

android:
	ndk-build

clean:
#   Cleaning of FreeType is handled in build.xml
	ant clean

clean_android:
	ndk-build clean

clean_all:
	ndk-build clean
	ant clean