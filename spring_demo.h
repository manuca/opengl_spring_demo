#ifdef __APPLE__
#include <OpenGL/gl.h> 
#include <OpenGL/glu.h> 
#include "AGL/agl.h" 
#include <GLUT/glut.h> 
#else
#include <GL/glut.h>
#endif

#ifndef _openglmain_h
#define _openglmain_h
void init(void);
void cleanup(void);
void idle(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void calcularIndices();
void calcularVertices();
void miKeyboardFunc(unsigned char key, int x, int y);
void calcularNormales();
void productoVectorial(GLfloat *v1, GLfloat *v2, GLfloat *v3, GLfloat *respuesta);
void normalizarVector(GLfloat *v, GLfloat *rta);
void dibujarVector(GLfloat *v, GLfloat *posicion);
void dibujarNormales(void);
GLfloat normaVector(GLfloat x, GLfloat y, GLfloat z);
void verificarNormales(void);
void miMouseFunc(int button, int state, int x, int y);
void miMotionFunc(int x, int y);
#endif
