These are direction for how the structure file JFreeType project builds
for different platforms should look like.

Each specific platform should have:
	* Makefile which should preferably be built using only "make" command
	 	and cleaned with "make clean"

	* a file which describes what dependencies you need to build for that
		specific platform (g++, android-ndk, ant and stuff like that should
		be listed in that file)

Each specific platform should use its own directory for storing stuff as
much as possible (for example bin folder should not be in the JFreeType 
root for that specific platform)

Each platform should clean everything it has generated in "make clean".
This includes FreeType compilation files, bin files and stuff like that.
After running "make clean" you should have a clean copy of that platform
compilation files.