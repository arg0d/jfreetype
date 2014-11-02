all: compile
	
compile:
	g++ -Werror -Isrc -Ilibs/jni/include -Ilibs/freetype/include -shared -o bin/jfreetype.dll src/FreeType.cpp -Llibs/freetype/objs -lfreetype
	g++ -Werror -Isrc -Ilibs/jni/include -Ilibs/freetype/include -shared -o bin/libjfreetype.so src/FreeType.cpp -Llibs/freetype/objs -lfreetype
	ant compile
	java -Djava.library.path=bin -classpath bin com/cig/jfreetype/JFreeType

run:
	java -classpath bin com/cig/jfreetype/JFreeType

jar:
	ant build-jar

clean:
	ant clean