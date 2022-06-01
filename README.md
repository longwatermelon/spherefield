# spherefield
Cubefield but with raytraced spheres

# Controls
* Left / right arrow keys: move
* R: restart
* F: toggle flash

https://user-images.githubusercontent.com/73869536/171477000-9b6c7a1c-0f61-4b36-a668-bf9fd0147ce4.mp4

# Building
Windows is not supported

Requires sdl2 and sdl2-ttf

## Linux
```
git clone https://github.com/longwatermelon/spherefield
cd spherefield
make
./a.out
```

## MacOS
```
git clone https://github.com/longwatermelon/spherefield
cd spherefield
make INCLUDE=-I$(brew --prefix)/include LIBRARIES=-L$(brew --prefix)/lib FLAGS=-Wno-error=unused-command-line-argument
./a.out
```
