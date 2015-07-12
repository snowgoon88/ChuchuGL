Chuchu Rocket again...

12/07/2015 : Up to 2 players can put arrows to manipulate Chuchu that keep coming. The Chuchu can enter one Rocket, on a mini-world.
Start Screen, Controler Screen, Game Screen

COMPILE
=======

Create a directory 'build' and run cmake in it
```
> mkdir -p build
> cd build
> cmake ..
```
The most advanced "game" version is test/test-011-engine.cpp
```
> make test-011-engine.cpp
> test/test-011-engine
```

DEPENDANCIES
============

* OpenGL with GLEXT: for visualisation
* GLFW3 : for graphical window management
* GLM : for OpenGL mathematics
* SOIL : for loading PNG images as OpenGL textures
* PortAudio : for playing sounds
* vorbis : pour lire les .ogg
* boost::program_options : pour lire certaines options