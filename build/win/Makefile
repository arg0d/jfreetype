# Set this to true for running the development version of the build script.
# If set to true, will try to compile Java side version and print some stuff.
# (This is purely for development of JFreeType)
DEVELOPMENT = TRUE

root_dir = ../../

# For better visiblity when printing messages
FILTER = ****JFreeType****:

all: compile
	
compile:
#	Build Freetype
	mingw32-make -C $(root_dir)libs/freetype

#	Build JFreeType
	g++ -Wall -Wl,--kill-at -static-libgcc -static-libstdc++ -I$(root_dir)src -I$(root_dir)libs/jni/include -I$(root_dir)libs/freetype/include -shared -o bin/jfreetype.dll $(root_dir)src/FreeType.cpp $(root_dir)libs/freetype/objs/freetype.a

#	Compile java code
	ant compile

ifeq ($(DEVELOPMENT), TRUE)
#	Run JFreeType development version code
	java -Djava.library.path=bin -classpath bin com/cig/jfreetype/JFreeType
endif

jar:
	@echo $(FILTER) Compiling Java source and building jar
	ant compile
	ant build-jar
	@echo $(FILTER) Jar was successfully built!

clean:
#   Cleaning of FreeType is handled in build.xml
	ant clean
