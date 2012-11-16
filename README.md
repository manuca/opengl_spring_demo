Intro
=====

Some old experiments with OpenGL I found on my archives.  This is very old
stuff I wanted to share instead of throwing it away, maybe it will be useful to
someone. 

I remember I developed this on Linux and compiled it also on Visual Studio 6
under Win32. Today I own a Mac and managed to tweak the code just so that it
compiles and runs.  

Below you'll find instructions to compile under OSX and Linux, I don't remember
how it was compiled under Windows google for it (search for "compile with glut
under visual studio" or something).

Unfortunately the code has spanish comments and names.

![Screen capture](https://raw.github.com/manuca/opengl_spring_demo/master/spring.png)

Building
========

OSX
---

```
gcc -framework OpenGL -framework GLUT spring_demo.c -o spring_demo
```

Linux
-----

```
gcc spring_demo.c -lGLU -lglut -o spring_demo
```

Win32
-----

Pending instructions

Using
=====

Drag with the mouse on the spring and the spring will oscilate with some
simulated exponential dumping.

Keys
----

```
'N' Toggle visibility of normal vectors
'S' Toggles visibility of wires
'Num pad (or numbers)' produces some rotations ('5' resets view)
```
