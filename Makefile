all: compile
	
compile:
	g++ -Werror -Isrc -Ilibs/jni/include -Ilibs/freetype/include -shared -o bin/lib.dll src/FreeType.cpp -Llibs/freetype/objs -lfreetype
	ant compile
	java -Djava.library.path=bin -classpath bin com/cig/jfreetype/FreeType

run:
	java -classpath bin com/cig/jfreetype/FreeType

jar:
	ant build-jar

clean:
	ant clean