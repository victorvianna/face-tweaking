#rm -r bin
mkdir bin
mkdir bin/engine
cd bin/engine
HEADER=../../lib/all_headers.h
echo "#ifndef __ALL_HEADERS__" > $HEADER
echo "#define __ALL_HEADERS__" >> $HEADER
find ../../lib/features -name '*.h' | while read line; do
	echo "#include \"$line\"" >> $HEADER
done
echo "#endif" >> $HEADER
cmake -DCMAKE_BUILD_TYPE=Release ../../src/engine
make
cd ../..
mkdir bin/GUI
cd bin/GUI
javac -d . ../../src/GUI/main.java