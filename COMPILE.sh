cd engine
HEADER=../lib/all_headers.h
echo "#ifndef __ALL_HEADERS__" > $HEADER
echo "#define __ALL_HEADERS__" >> $HEADER
find ../features -name '*.h' | while read line; do
	echo "#include \"$line\"" >> $HEADER
done
echo "#endif" >> $HEADER