cd `pwd`/code
make distclean install
mv proj ../
cd ..
strip --strip-all proj