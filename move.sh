
mkdir -p ./build/cfg
mkdir -p ./build/dat
mkdir -p ./build/doc
mkdir -p ./build/gfx
mkdir -p ./build/sav
mkdir -p ./build/sav/characters
mkdir -p ./build/sav/states
mkdir -p ./build/sfx
mkdir -p ./build/vfx

cp ./cfg/* ./build/cfg/
cp ./doc/* ./build/doc/
cp ./sfx/* ./build/sfx/
cp -r ./sav/* ../build/sav/
cp ./vfx/* ./build/vfx/
cp ./dat/* ./build/dat/
cp ./gfx/* ./build/gfx/

