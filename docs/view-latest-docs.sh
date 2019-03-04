export PATH=$PATH:.
rm -rf html
doxygen doxyfile
firefox html/annotated.html

