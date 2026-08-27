
mkdir -p ./build/dist/cfg
mkdir -p ./build/dist/dat
mkdir -p ./build/dist/doc
mkdir -p ./build/dist/gfx
mkdir -p ./build/dist/sav
mkdir -p ./build/dist/sav/characters
mkdir -p ./build/dist/sav/states
mkdir -p ./build/dist/sfx
mkdir -p ./build/dist/vfx

cp ./cfg/* ./build/dist/cfg/
cp ./doc/* ./build/dist/doc/
cp ./sfx/* ./build/dist/sfx/
cp -r ./sav/* ../build/dist/sav/
cp ./vfx/* ./build/dist/vfx/
cp ./dat/* ./build/dist/dat/
cp ./gfx/* ./build/dist/gfx/

