Chuchu Rocket again...

Right now : some tuto on OpenGL and some moving Chuchu, but not visualized.

COMPILE
=======

Create a directory 'build' and run cmake in it
...
> mkdir -p build
> cd build
> cmake ..
...

Then compile and run tuto-001-triangle for example
...
> make tuto-001-triangle
> opengltuto/tuto-001-triangle
...

DEPENDANCIES
============

* OpenGL with GLEXT: for visualisation
* GLFW3 : for graphical window management
* GLM : for OpenGL mathematics
* SOIL : for loading images as OpenGL textures
* PortAudio : for playing sounds
* vorbis : pour lire les .ogg
* boost::program_options : pour lire certaines option