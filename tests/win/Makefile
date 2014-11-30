root_dir = ../../

all: compile
	
compile:
# 	mingw32-make -C $(root_dir)libs/freetype
	g++ -Wall -Wl,--kill-at -static-libgcc -static-libstdc++ -I$(root_dir)src -I$(root_dir)libs/jni/include -I$(root_dir)libs/freetype/include -shared -o $(root_dir)bin/jfreetype.dll $(root_dir)src/FreeType.cpp $(root_dir)libs/freetype/objs/freetype.a
	ant -buildfile $(root_dir)build.xml compile
	java -Djava.library.path=$(root_dir)bin -classpath $(root_dir)bin com/cig/jfreetype/JFreeType

jar:
	ant -buildfile $(root_dir)build.xml build-jar

clean:
#   Cleaning of FreeType is handled in build.xml
	ant -buildfile $(root_dir)build.xml clean