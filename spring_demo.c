#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "spring_demo.h"

#define PI 3.1415926535897f
enum {MODO_NORMAL, MODO_ARRASTRE};

/* Parametrizacion de la helice */

static const int nro_de_aros=100;
static const int vertices_por_aro=50;
static GLfloat paso_helice=0.0f;
static const GLfloat radio_espiral=0.5f;
static const GLfloat radio_alambre=0.25f;

static GLfloat paso_natural=0.3f;
static GLfloat paso_inicial=0.0f;
static GLfloat paso_max=0.4f;
static GLfloat amortiguamiento=0.05f;
static GLfloat prop_estiramiento=0.0f;

/* Variables referidas al manejo del mouse */

static int x_inic, x_final;
static int y_inic, y_final;
static int ancho_ventana;
static int distancia_mouse;
static int modo_animacion;

/* Variables globales referidas a la construccion del resorte */

static int nro_de_indices; 
static GLfloat *vertices;
static GLfloat *normales;
static GLuint  *indices;
static GLfloat deltau, deltav;
static int dibujar_normales = -1;
static int dibujar_solido=1;

/* Variables globales referidas al manejo de la camara */

static GLfloat x_camara=0.0f;
static GLfloat y_camara=0.0f;
static GLfloat z_camara=0.0f;
static GLfloat radio_camara=2.0f;
static const GLfloat delta_radio = 0.25f;
static GLfloat rotacion_horizontal = 0.0f;
static GLfloat rotacion_vertical = 0.0f;
static const GLfloat delta_rotacion=0.1f;

/* Variables globales referidas a la animacion */

static GLfloat tiempo = 0.0f;
static GLfloat delta_tiempo = 0.1f;

/* ------------------------------------------- */

void pruebaProdVect()
{
  GLfloat v1[3] = {0.0f, 1.0f, 0.0f};
  GLfloat v2[3] = {0.0f, 0.0f, 1.0f};
  GLfloat v3[3] = {0.0f, 0.0f,0.0f};
  GLfloat rta[3];

  productoVectorial(v3, v1, v2, rta);
}

void dibujarVector(GLfloat *v, GLfloat *posicion)
{
  glPointSize(2.5f);

  glBegin(GL_LINES);	
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(posicion[0], posicion[1], posicion[2]);	
  glColor3f(0.0f, 1.0f, 1.0f);
  glVertex3f(posicion[0] + v[0], posicion[1] + v[1], posicion[2] + v[2]);		
  glEnd();	
}


void dibujarNormales()
{
  int i;
  int nro_de_vertices;
  GLfloat *v, *n;

  /* Esta cuenta se puede sacar afuera para optimizar */
  nro_de_vertices = nro_de_aros*vertices_por_aro;

  v = vertices;
  n = normales;

  for(i=0; i < nro_de_vertices; i++) {
    dibujarVector(n,v);
    v += 3;
    n += 3;
  }
}


void initlights(void)
{
  GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat position[] = {10.0, 10.0, 0.0, 1.0};
  GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 0.0, 1.0};
  GLfloat mat_shininess[] = {20.0};

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
}

void init(void)
{

  nro_de_indices = 4*vertices_por_aro*(nro_de_aros-1);

#ifdef _DEBUG
  printf("\nINIT:\n");
  printf("-----\n");
  printf("nro_de_indices: %i.\n", nro_de_indices);
#endif

  /* Agregar verificacion de errores a la alocacion de memoria */
  vertices = malloc(sizeof(GLfloat)*3*nro_de_aros*vertices_por_aro);
  normales = malloc(sizeof(GLfloat)*3*nro_de_aros*vertices_por_aro);

#ifdef _DEBUG
  printf("Reservando espacio para %i coordenadas de tipo GLfloat.\n"
      ,3*nro_de_aros*vertices_por_aro);
  printf("Tenemos un total de %i vertices.\n", vertices_por_aro*nro_de_aros);
#endif

  indices =  malloc(sizeof(GLint)*nro_de_indices);

  deltau = (6.0f * PI) / ((GLfloat) nro_de_aros);
  deltav = (2.0f * PI) / ((GLfloat) vertices_por_aro);

  printf("DeltaU: %f\nDeltaV: %f\n", deltau, deltav);
  calcularIndices();

  glEnableClientState(GL_VERTEX_ARRAY);
  calcularVertices();
  glVertexPointer(3, GL_FLOAT, 0, vertices);

  glEnableClientState(GL_NORMAL_ARRAY);
  calcularNormales();

  /* verificarNormales(); */

  glNormalPointer(GL_FLOAT, 0, normales);
  initlights();
  glEnable(GL_DEPTH_TEST);

  modo_animacion = MODO_NORMAL;
}

void cleanup()
{
#ifdef _DEBUG
  printf("\nLIMPIEZA:\n");
  printf("--------\n");
  printf("Liberando vertices...\n");
  printf("Liberando indices...\n");
#endif

  free(vertices);
  free(normales);
  free(indices);
}

void setearCoordenadasCamara()
{
  x_camara = radio_camara*sin(90.0 - rotacion_vertical)*sin(rotacion_horizontal);
  y_camara = radio_camara*cos(90.0 - rotacion_vertical);
  z_camara = radio_camara*sin(90-rotacion_vertical)*cos(rotacion_horizontal);	
}

void miKeyboardFunc(unsigned char key, int x, int y)
{
  switch (key) {
    case 'n':
    case 'N':
      dibujar_normales = -dibujar_normales;
      break;
    case 's':
    case 'S':
      dibujar_solido = -dibujar_solido;
      break;
    case '8':
      rotacion_vertical += delta_rotacion;
      if(rotacion_vertical>90.0f)
        rotacion_vertical = 90.0f;
      break;
    case '6':
      rotacion_horizontal += delta_rotacion;
      rotacion_horizontal = fmod(rotacion_horizontal,360.0);
      break;
    case '2':
      rotacion_vertical -= delta_rotacion;
      if(rotacion_vertical < -90.0f)
        rotacion_vertical = -90.0f;
      break;
    case '4':
      rotacion_horizontal -= delta_rotacion;
      rotacion_horizontal = fmod(rotacion_horizontal,360.0);
      break;
    case '5':
      rotacion_vertical=0.0f;
      rotacion_horizontal=0.0f;
      radio_camara = 1.0f;
      break;
    case '+':
      radio_camara += delta_radio;
      break;
    case '-':
      radio_camara -= delta_radio;
      /*if(radio_camara < 1.0f)
        radio_camara = 1.0f;*/
      break;
  }

}

void calcularIndices()
{
  int i,j,k;
  int offset;

#ifdef _DEBUG
  printf("\nINDICES:\n");
  printf("--------\n");
  printf("Calculando indices...\n");
#endif

  k=0;

  for(j=0; j < nro_de_aros - 1; j++)
  {

    offset = j*vertices_por_aro;

    for(i=0; i < vertices_por_aro-1; i++)
    {
      indices[k++] = i + offset;
      indices[k++] = i + 1 + offset;
      indices[k++] = i + vertices_por_aro + 1 + offset;
      indices[k++] = i + vertices_por_aro + offset;
    }

    indices[k++] = vertices_por_aro - 1 + offset;
    indices[k++] = offset;
    indices[k++] = vertices_por_aro + offset;
    indices[k++] = vertices_por_aro*2 - 1 + offset;

  }

#ifdef _DEBUG
  printf("Hemos calculado %i indices segun el valor de k.\n", k);
#endif

  return;
}

void calcularVertices()
{
  GLfloat u, v;
  int i,j,k;

  k=0;
  u=0; /* Con U barremos aros */
  v=0; /* Con V barremos vertices en un aro */

  for(i=0; i < nro_de_aros; i++)
  {
    for(j=0; j < vertices_por_aro; j++)
    {
      vertices[k++] = (GLfloat)((radio_espiral + radio_alambre*cos(v))*cos(u)); /* X */
      vertices[k++] = (GLfloat)((radio_espiral + radio_alambre*cos(v))*sin(u)); /* Y */
      vertices[k++] = (GLfloat) (radio_alambre*sin(v) + paso_helice*u); /* Z */

      v+=deltav;
    }

    u += deltau;
  }
}

/* 
 ** Calcula las normales, al final las invierto porque las calcule en el 
 ** orden opuesto. 
 */

void calcularNormales()
{
  GLfloat *v;
  GLfloat normal[3], normal_unit[3];
  int i,j, k, offset;

  k=0;
  v = vertices;

  for(i=0;i < nro_de_aros-1; i++) {

    offset = i*vertices_por_aro;

    for(j=0; j < vertices_por_aro-1; j++) {
      productoVectorial(&v[3*(j+offset)],&v[3*(j+offset+1)],
          &v[3*(j+offset+vertices_por_aro)],normal);

      normalizarVector(normal, normal_unit);

      normales[k++] = -normal_unit[0];
      normales[k++] = -normal_unit[1];
      normales[k++] = -normal_unit[2];
    }

    productoVectorial(&v[3*(offset+vertices_por_aro-1)], &v[3*offset],
        &v[3*(offset+2*vertices_por_aro-1)], normal);

    normalizarVector(normal, normal_unit);
    normales[k++] = -normal_unit[0];
    normales[k++] = -normal_unit[1];
    normales[k++] = -normal_unit[2];
  }

  offset = (nro_de_aros-1)*vertices_por_aro;

  for(j=0; j < vertices_por_aro-1; j++) {
    productoVectorial(&v[3*(offset+j)], &v[3*(offset+j-vertices_por_aro)],
        &v[3*(offset+j+1)], normal);

    normalizarVector(normal, normal_unit);
    normales[k++] = -normal_unit[0];
    normales[k++] = -normal_unit[1];
    normales[k++] = -normal_unit[2];
  }

  productoVectorial(&v[3*(offset + vertices_por_aro-1)],
      &v[3*(offset-1)], &v[3*offset], normal);

  normalizarVector(normal, normal_unit);

  normaVector(normal_unit[0],normal_unit[1],normal_unit[2]);
  normaVector(normal[0],normal[1],normal[2]);

  normales[k++] = -normal_unit[0];
  normales[k++] = -normal_unit[1];
  normales[k++] = -normal_unit[2];

  return;
}

GLfloat normaVector(GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat norma;

  norma = (GLfloat) sqrt(pow(x,2) + pow(x,2) + pow(z,2));
  return norma;
}

void verificarNormales(void)
{
  GLfloat norma;
  GLfloat cota_norma;
  GLfloat *n;
  int i, nro_de_vertices;

  cota_norma=1.1f;
  nro_de_vertices = vertices_por_aro*nro_de_aros;
  n = normales;

  for(i=0; i<nro_de_vertices; i++) {
    norma = normaVector(n[0], n[1], n[2]);
    if(norma > cota_norma) {
      printf("Norma mayor a 1 en vertice %i. ", i);
      printf("Mide: %f.\n", norma);
    }
    n += 3;
  }

  return;
}


void normalizarVector(GLfloat *v, GLfloat *rta)
{
  int i;
  GLfloat norma;

  norma = (GLfloat) sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));

  for(i=0; i<3; i++) {
    rta[i] = v[i]/norma;
  }

  norma = (GLfloat) sqrt(pow(rta[0],2) + pow(rta[1],2) + pow(rta[2],2));

  return;

}

void productoVectorial(GLfloat *v1, GLfloat *v2, GLfloat *v3, GLfloat *respuesta)
{
  GLfloat a[3];
  GLfloat b[3];
  int i;

  for(i=0; i<3; i++) {
    a[i] = v2[i] - v1[i];
    b[i] = v3[i] - v1[i];
  }

  respuesta[0] = a[1]*b[2] - a[2]*b[1];
  respuesta[1] = a[2]*b[0] - a[0]*b[2];
  respuesta[2] = a[0]*b[1] - a[1]*b[0];
}


/* Aca dentro ponemos todo el codigo referido a la animacion */

void idle(void)
{ 
  if(modo_animacion == MODO_NORMAL) {
    tiempo += delta_tiempo;
    paso_helice = (GLfloat) 
      (paso_natural + paso_inicial*exp(-tiempo*amortiguamiento)*sin(tiempo));
  }
  else if(modo_animacion == MODO_ARRASTRE) {
    distancia_mouse = (int) sqrt( pow((x_inic-x_final),2) 
        + pow((y_inic-y_final),2));

    prop_estiramiento = distancia_mouse / ((GLfloat) ancho_ventana/2);

    if(prop_estiramiento > 1.0f)
      prop_estiramiento = 1.0f;

    paso_inicial = paso_max*prop_estiramiento;		
  }
  calcularVertices();
  calcularNormales();
  glutPostRedisplay();
}

void display(void)
{		
  glClearColor( 0.1f, 0.1f, 0.4f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		

  glPushMatrix();
  setearCoordenadasCamara();
  gluLookAt(x_camara, y_camara, z_camara, 
      0.0, 0.0, 0.0, 
      0.0, 1.0, 0.0);

  glColor3f(0.0f, 1.0f, 0.0f);
  glTranslatef(0.0f, 5.0f, 0.0f);
  glRotatef( 90, 1.0f, 0.0f, 0.0f );

  if(dibujar_solido > 0)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	/* GL_LINE o GL_FILL */
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	/* GL_LINE o GL_FILL */

  glDrawElements(GL_QUADS, nro_de_indices, GL_UNSIGNED_INT, indices);

  if(dibujar_normales > 0)
    dibujarNormales();
  glPopMatrix();
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  ancho_ventana = w;

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= (h * 2))
    glOrtho (-6.0, 6.0, -3.0*((GLfloat)h*2)/(GLfloat)w, 
        3.0*((GLfloat)h*2)/(GLfloat)w, -10.0, 10.0);
  else
    glOrtho (-6.0*(GLfloat)w/((GLfloat)h*2), 
        6.0*(GLfloat)w/((GLfloat)h*2), -3.0, 3.0, -10.0, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void miMotionFunc(int x, int y)
{
  x_final = x;
  y_final = y;	
  return;
}

void miMouseFunc(int button, int state, int x, int y)
{
  if(state == GLUT_DOWN) {
    modo_animacion = MODO_ARRASTRE;
    x_inic = x;
    y_inic = y;
    x_final = x;
    y_final = y;
  }
  else if(state==GLUT_UP) {
    modo_animacion = MODO_NORMAL;
    tiempo = 0.0f;
  }

  return;
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 27:
      cleanup();
      exit(0);
    default:
      miKeyboardFunc(key,x,y);
      break;
  }
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (600, 450);
  glutCreateWindow("Spring demo");
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutKeyboardFunc (keyboard);
  glutMouseFunc(miMouseFunc);
  glutMotionFunc(miMotionFunc);
  glutMainLoop();    
}
