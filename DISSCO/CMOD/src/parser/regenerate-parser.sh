#!/bin/sh
flex Parser.l
bison --verbose --debug -d -o Parser.cpp Parser.y
mv Parser.hpp Parser.h
rm Parser.output
#gcc -g -c lex.yy.c -o lex.yy.o
#g++ -g lex.yy.o Parser.cpp -o Parser -Wno-deprecated
echo "*****************************************************************************"
echo "Note:"
echo "Change the two instances of msgid in Parser.cpp to (char*)msgid near line 387"
echo ""
echo "Change only the msgid at the end of the two lines, i.e.:"
echo "define YY_(msgid) dgettext (\"bison-runtime\", (char*)msgid)        ...and..."
echo "define YY_(msgid) (char*)msgid"
echo ""
echo "This will remove two warning messages about deprecated use of char*"
echo "*****************************************************************************"

