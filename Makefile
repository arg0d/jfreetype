all: begin compile execute
	
begin:
	

compile:
	g++ -Werror -Isrc -Ilibs/jni/include -Ilibs/freetype/include -shared -o bin/lib.dll src/FreeType.cpp -Llibs/freetype/objs -lfreetype
	ant compile
	
execute:
	java -classpath bin com/cig/jfreetype/FreeType