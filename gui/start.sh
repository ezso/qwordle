rm -f libwordle.so
# ln -s ../bin/libwordle.so libwordle.so
cp -f ../bin/libwordle.so libwordle.so
cp -rf ../data ./

LD_LIBRARY_PATH=.
export LD_LIBRARY_PATH
# to use a dark theme
GTK_THEME=Adwaita:dark
export GTK_THEME
# ./gui data/5.txt true
./gui 
