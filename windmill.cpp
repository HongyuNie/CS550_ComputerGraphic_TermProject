
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glslprogram.h"


// This is HongyuNie final project 'WindMill'

// title of these windows:

const char *WINDOWTITLE = { "WindMill-- Hongyu Nie" };
const char* GLUTTITLE = { "User Interface Window" };


// what the glui package defines as ture and false:

const int GLUITRUE = { true };
const int GLUIFALSE = { false };


// the escape key:

#define  ESCAPE    0x1b


// animation cycle

#define MS_PER_CYCLE  10000


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// multiplications factors for input interaction:
// (these are known from previous experence)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


//minimum allowable scale factor:

const float MINSCALE = { 0.05f};


//active mouse buttons( or them together):

const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };


// which projection:

enum Projection
{
	ORTHO,
	PERSP
};


// which button

enum ButtonVals
{
	RESET,
	QUIT
};


//window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };


// line width for the axes£º

const GLfloat AXES_WIDTH = { 3. };


// the color numbers
// this order must match the radio button order 

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char* ColorNames[] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definition
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{1.,0.,0.},   //red
	{1.,1.,0.},   //yellow
	{0.,1.,0.},   //green
	{0.,1.,1.},   //cyan
	{0.,0.,1.},   //blue
	{1.,0.,1.},   //magenta
	{1.,1.,1.},   // white
	{0.,0.,0.},   //black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0,.0,.0,1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


//non-constant global variables:

int     ActiveButton;        //current button that is down
GLuint  AxesList;            //list to hold the axes
GLuint  SkeletonList;
GLuint  BodyList1;            //list of wildmill body
GLuint  BodyList2;
GLuint  BodyList3;
GLuint  BodyList4;
GLuint  BodyList5;
GLuint  BladeList;           //list of blades
GLuint  GearList;            //list of gears
GLuint  WaterWheelList;      //list of waterwheel
GLuint  AxisList1;   
GLuint  AxisList2;
GLuint  AxisList3;
GLuint  GearList1;
GLuint  GearList2;
GLuint  GearList3;
GLuint  GearList4;
int		BodyOn;				 //!=0 means to draw the body
int     AxesOn;              //!=0 means to draw the axes
int     MainWindow;          //object display list
float   Scale;               //scaling factor
int     WhichColor;          //index into Colors[]
int     WhichProjection;     //ORTHO or PERSP
int     Xmouse, Ymouse;      //mouse values
float   Xrot, Yrot;          //rotation angles in degrees
float   BladeAngle;          //blade rotation angles in degrees
GLSLProgram *Pattern;        //declare the GLSL Program 
float   Time;                //animation time
float   ColorR, ColorG, ColorB;  
float   SColorR, SColorG, SColorB;
float   Ka, Kd, Ks;
float   Shininess;
float   S0, T0;
float   Size;
int     FragmentOn;           // != 0 means to turn on the fragment animation
int     VertexOn;             // != 0 means to turn on the vertex animation



// function prototypes:

void   Animate( );
void   Display( );
void   DoBodyMenu(int);
void   DoAxesMenu( int );
void   DoMainMenu( int );
void   DoProjectMenu( int );
void   InitGraphics( );
void   InitLists( );
void   InitMenus( );
void   Keyboard( unsigned char, int, int );
void   MouseButton( int, int, int, int );
void   MouseMotion( int, int );
void   Reset( );
void   Axes( float );




//main program:

int
main( int argc, char *argv [ ])
{
	glutInit(&argc, argv);

	InitGraphics( );

	InitLists( );

	//init all the global variables used by Display();
	//this will also post a redisplay

	Reset( );

	InitMenus( );

	//draw the scene once and wait for some interaction:
	//(this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	return 0;
}


void
Animate() {

	int ms = glutGet(GLUT_ELAPSED_TIME);       // milliseconds
	ms %= MS_PER_CYCLE;
	Time = (float)ms / (float)MS_PER_CYCLE;    // [ 0., 1. )

	BladeAngle = Time * 360;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
Display()
{
	// set which window we want to do the graphics into:

	glutSetWindow(MainWindow);


	// erase the background

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// specify shading to be flat:

	glShadeModel(GL_FLAT);


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;     // minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);


	// place the object into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// set the eye position, look-at position, and up-vector:

	gluLookAt(1., 4., 8., 0., 4., 0., 0., 1., 0.);


	// rotate the scene

	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);


	// uniformly scale the scene

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);


	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[WhichColor][0]);
		glCallList(AxesList);
	}

	glEnable(GL_NORMALIZE);


	Ka = 0.5;
	Kd = 0.7;
	Ks = 0.5;
	SColorR = 1.;
	SColorG = 1.;
	SColorB = 1.;
	Shininess = 10;

	if (BodyOn)
	{
		ColorR = 1.; ColorG = 0.; ColorB = 0.;
		Pattern->Use();
		Pattern->SetUniformVariable("uKa", Ka);
		Pattern->SetUniformVariable("uKd", Kd);
		Pattern->SetUniformVariable("uKs", Ks);
		Pattern->SetUniformVariable("uColor", ColorR, ColorG, ColorB);
		Pattern->SetUniformVariable("uSpecularColor", SColorR, SColorG, SColorB);
		Pattern->SetUniformVariable("uShininess", Shininess);
		glCallList(BodyList1);
		Pattern->Use(0);

		ColorR = 1.; ColorG = 0.; ColorB = 0.;
		Pattern->Use();
		Pattern->SetUniformVariable("uKa", Ka);
		Pattern->SetUniformVariable("uKd", Kd);
		Pattern->SetUniformVariable("uKs", Ks);
		Pattern->SetUniformVariable("uColor", ColorR, ColorG, ColorB);
		Pattern->SetUniformVariable("uSpecularColor", SColorR, SColorG, SColorB);
		Pattern->SetUniformVariable("uShininess", Shininess);
		glCallList(BodyList2);
		Pattern->Use(0);

		ColorR = 1.; ColorG = 0.6; ColorB = 0.;
		Pattern->Use();
		Pattern->SetUniformVariable("uKa", Ka);
		Pattern->SetUniformVariable("uKd", Kd);
		Pattern->SetUniformVariable("uKs", Ks);
		Pattern->SetUniformVariable("uColor", ColorR, ColorG, ColorB);
		Pattern->SetUniformVariable("uSpecularColor", SColorR, SColorG, SColorB);
		Pattern->SetUniformVariable("uShininess", Shininess);
		glCallList(BodyList3);
		Pattern->Use(0);

		ColorR = 1.; ColorG = 1.; ColorB = 1.;
		Pattern->Use();
		Pattern->SetUniformVariable("uKa", Ka);
		Pattern->SetUniformVariable("uKd", Kd);
		Pattern->SetUniformVariable("uKs", Ks);
		Pattern->SetUniformVariable("uColor", ColorR, ColorG, ColorB);
		Pattern->SetUniformVariable("uSpecularColor", SColorR, SColorG, SColorB);
		Pattern->SetUniformVariable("uShininess", Shininess);
		glCallList(BodyList4);
		Pattern->Use(0);

		ColorR = 0.7; ColorG = 0.4; ColorB = 0.1;
		Pattern->Use();
		Pattern->SetUniformVariable("uKa", Ka);
		Pattern->SetUniformVariable("uKd", Kd);
		Pattern->SetUniformVariable("uKs", Ks);
		Pattern->SetUniformVariable("uColor", ColorR, ColorG, ColorB);
		Pattern->SetUniformVariable("uSpecularColor", SColorR, SColorG, SColorB);
		Pattern->SetUniformVariable("uShininess", Shininess);
		glCallList(BodyList5);
		Pattern->Use(0);
	}


	ColorR = 0.7; ColorG = 0.4; ColorB = 0.1;
	Pattern->Use();
	Pattern->SetUniformVariable("uKa", Ka);
	Pattern->SetUniformVariable("uKd", Kd);
	Pattern->SetUniformVariable("uKs", Ks);
	Pattern->SetUniformVariable("uColor", ColorR, ColorG, ColorB);
	Pattern->SetUniformVariable("uSpecularColor", SColorR, SColorG, SColorB);
	Pattern->SetUniformVariable("uShininess", Shininess);

	glPushMatrix();
	glTranslatef(0., 5.5, 2.);
	glRotatef(BladeAngle, 0., 0., 1.);
	glCallList(BladeList);
	glPopMatrix();

	glPushMatrix();
	glCallList(SkeletonList);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0., 0., -3.);
	glRotatef(BladeAngle, 0., 0., 1.);
	glCallList(WaterWheelList);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0., 5.5, 0.);
	glRotatef(BladeAngle, 0., 0., 1.);
	glCallList(AxisList1);
	glPopMatrix();


	glPushMatrix();
	glRotatef(BladeAngle, 0., 1., 0.);
	glCallList(AxisList2);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0., 0., -3);
	glRotatef(BladeAngle, 0., 0., 1.);
	glCallList(AxisList3);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0., 5.5, 0.5);
	glRotatef(BladeAngle, 0., 0., 1.);
	glCallList(GearList1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0., 5., 0.);
	glRotatef(BladeAngle, 0., 1., 0.);
	glCallList(GearList2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0., 0.3, 0.);
	glRotatef(BladeAngle, 0., 1., 0.);
	glCallList(GearList3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0., 0., -0.3);
	glRotatef(BladeAngle, 0., 0., 1.);
	glCallList(GearList4);
	glPopMatrix();

	Pattern->Use(0);

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	glFlush();
}


void
DoBodyMenu(int id)
{
	BodyOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay();
}

void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoMainMenu( int id ) 
{
	switch (id)
	{
		case RESET:
			Reset();
			break;

		case QUIT:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
InitMenus()
{
	glutSetWindow( MainWindow );

	int bodymenu = glutCreateMenu(DoBodyMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry( "Off", 0 );
	glutAddMenuEntry( "On", 1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic", ORTHO );
	glutAddMenuEntry( "Perspective", PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu( "Body",        bodymenu );
	glutAddSubMenu( "Axes",        axesmenu );
	glutAddSubMenu( "Projection" , projmenu );
	glutAddMenuEntry( "Reset",       RESET );
	glutAddMenuEntry( "Quit",        QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );

}


void
InitGraphics( )
{
	//request the display modes:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );


	//set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title£º

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// set the callback functions:

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( NULL );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( NULL );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// do the GLEW set up (initializing the GLEW)
	// init glew (a window must be open to do this)

#ifdef WIN32
	GLenum err = glewInit( );
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status:Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// do this *after* opening the window and initialing glew:

	Pattern = new GLSLProgram( );
	bool valid = Pattern->Create( "lighting.vert",  "lighting.frag" );
	if( ! valid )
	{
		fprintf( stderr, "Shader cannot be created!\n" );
		DoMainMenu( QUIT );
	}
	else
	{
		fprintf( stderr, "Shader created.\n" );
	}
	Pattern->SetVerbose( false );
	
}

void
InitLists() 
{
	glutSetWindow( MainWindow );

	// generate the windmill's main body shape 
	
	BodyList1 = glGenLists(1);
	glNewList(BodyList1, GL_COMPILE);
	glColor3f(1., 0., 0.);
	glBegin(GL_QUADS);       // the bottom hexahedron
	float a = 0., a1 = M_PI / 6;
	float r = M_PI / 3;
	for ( int i = 0; i <= 5; i++ ) 
	{ 
		glNormal3f( cos(a1), 0., sin(a1) );
		glVertex3f( 2 * cos(a), 0., 2 * sin(a) );
		glVertex3f( 2 * cos(a), 2., 2 * sin(a) );
		glVertex3f( 2 * cos(a + r), 2., 2 * sin(a + r) );
		glVertex3f( 2 * cos(a + r), 0., 2 * sin(a + r) );
		a = a + r;
		a1 = a1 + r;
	}
	glEnd();
	glEndList();
	BodyList2 = glGenLists(1);
	glNewList(BodyList2, GL_COMPILE);
	glColor3f(0., 1., 0.);
	glBegin(GL_QUAD_STRIP);   // the plane between the bottom and the middle hexahedron 
	a = 0.;
	for (int i = 0; i <= 6; i++)
	{
		glNormal3f(0., 1., 0);
		glVertex3f(2. * cos(a), 2., 2. * sin(a));
		glVertex3f(1.8 * cos(a), 2., 1.8 * sin(a));
		a = a + r;
	}
	glEnd();
	glEndList();
	BodyList3 = glGenLists(1);
	glNewList(BodyList3, GL_COMPILE);
	glColor3f(0., 0., 1.);
	glBegin(GL_QUADS);       // the middle hexahedron
	a = 0., a1 = M_PI / 6;
	for (int i = 0; i <= 5; i++)
	{
		glNormal3f(1.8 * cos(a1), sqrt(0.8 * cos(M_PI/6)) / 3, 1.8 * sin(a1));
		glVertex3f(1.8 * cos(a), 2., 1.8 * sin(a));
		glVertex3f(1. * cos(a), 5., 1. * sin(a));
		glVertex3f(1. * cos(a + r), 5., 1. * sin(a + r));
		glVertex3f(1.8 * cos(a + r), 2., 1.8 * sin(a + r));
		a = a + r;
		a1 = a1 + r;
	}
	glEnd();
	glEndList();
	BodyList4 = glGenLists(1);
	glNewList(BodyList4, GL_COMPILE);
	glColor3f(0., 0., 0.5);
	glBegin(GL_QUADS);    // the top hexahedron
	a = 0., a1 = M_PI / 6.;
	for (int i = 0; i <= 5; i++)
	{

		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(1. * cos(a), 5., 1. * sin(a));
		glVertex3f(1. * cos(a), 6., 1. * sin(a));
		glVertex3f(1. * cos(a + r), 6., 1. * sin(a + r));
		glVertex3f(1. * cos(a + r), 5., 1. * sin(a + r));
		a = a + r;
		a1 = a1 + r;
	}
	glEnd();
	glEndList();
	BodyList5 = glGenLists(1);
	glNewList(BodyList5, GL_COMPILE);
	glColor3f(0., 0.5, 0.5);
	glBegin(GL_TRIANGLES);   // the top pyramid 
	a = 0., a1 = M_PI/6;
	for (int i = 0; i <= 6; i++)
	{
		glNormal3f(1. * cos(a1), sqrt(cos(M_PI / 6)), 1. * sin(a1));
		glVertex3f(0., 7., 0.);
		glVertex3f(1. * cos(a), 6., 1. * sin(a));
		glVertex3f(1. * cos(a + r ), 6., 1. * sin(a +r));
		a = a + r;
		a1 = a1 + r;
	}
	glEnd();
	glEndList();

	
	// draw the windmill's four grid blades
	// generate one then tarnslate and rotate to get others


	BladeList = glGenLists(1);
	glNewList(BladeList, GL_COMPILE);
	//draw the skeleton of the first blade
	glPushMatrix();
	glColor3f(1., 1., 1.);
	glRotated(-90.,  1., 0., 0.);
	glPushMatrix();
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	float r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(0.05 * cos(a1), 0.05 * sin(a1), 0.);
		glVertex3f(0.05 * cos(a) , 0.05 * sin(a) , 0.);
		glVertex3f(0.05 * cos(a) , 0.05 * sin(a) , 4.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 4.);
		glVertex3f(0.05 * cos(a + r1 ), 0.05 * sin(a + r1), 0.);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	// draw the mesh of the first blade 
	float x = 0., z = 0.;
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++) 
		{
			glBegin(GL_QUAD_STRIP);
			glNormal3f(0., -1., 0.);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 4.);
			glVertex3f(0.14 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 3.5);
			glVertex3f(0.14 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 3.5);
			glVertex3f(0.06 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glEnd();
			x = x + 0.1;
		}
		x = 0.;
		z = z - 0.5;
	
	}
	glPopMatrix();
	// draw the second grid blade 
	glPushMatrix();
	glRotatef(90., 0., 1., 0.);             // rotate the first finished grid blade to generate the second one
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(0.05 * cos(a1), 0.05 * sin(a1), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 4.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 4.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 0.);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();

	x = 0., z = 0.;
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			glBegin(GL_QUAD_STRIP);
			glNormal3f(0., -1., 0.);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 4.);
			glVertex3f(0.14 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 3.5);
			glVertex3f(0.14 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 3.5);
			glVertex3f(0.06 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glEnd();
			x = x + 0.1;
		}
		x = 0.;
		z = z - 0.5;

	}
	glPopMatrix();
	// draw the third grid blade
	glPushMatrix();                                
	glRotatef(180., 0., 1., 0.);       
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(0.05 * cos(a1), 0.05 * sin(a1), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 4.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 4.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 0.);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();

	x = 0., z = 0.;
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			glBegin(GL_QUAD_STRIP);
			glNormal3f(0., -1., 0.);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 4.);
			glVertex3f(0.14 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 3.5);
			glVertex3f(0.14 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 3.5);
			glVertex3f(0.06 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glEnd();
			x = x + 0.1;
		}
		x = 0.;
		z = z - 0.5;

	}
	glPopMatrix();
	// draw the fourth gird blade
	glPushMatrix();                               
	glRotatef(270., 0., 1., 0.);
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(0.05 * cos(a1), 0.05 * sin(a1), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 4.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 4.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 0.);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();

	x = 0., z = 0.;
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			glBegin(GL_QUAD_STRIP);
			glNormal3f(0., -1., 0.);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 4.);
			glVertex3f(0.14 + x, 0., z + 3.99);
			glVertex3f(0.15 + x, 0., z + 3.5);
			glVertex3f(0.14 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 3.5);
			glVertex3f(0.06 + x, 0., z + 3.51);
			glVertex3f(0.05 + x, 0., z + 4.);
			glVertex3f(0.06 + x, 0., z + 3.99);
			glEnd();
			x = x + 0.1;
		}
		x = 0.;
		z = z - 0.5;

	}
	glPopMatrix();
	glPopMatrix();
	glEndList();


	// draw the axis which connected gears, blade and water mill
	

	// draw the axis connected blade and main axis
	AxisList1 = glGenLists(1);
	glNewList(AxisList1, GL_COMPILE);
	glColor3f(0.5, 0.5, 1.);
	glPushMatrix();
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(cos(a1), sin(a1), 0.);
		glVertex3f(0.1 * cos(a), 0.1 * sin(a), 0.5);
		glVertex3f(0.1 * cos(a), 0.1 * sin(a), 2.);
		glVertex3f(0.1 * cos(a + r1), 0.1 * sin(a + r1), 2.);
		glVertex3f(0.1 * cos(a + r1), 0.1 * sin(a + r1), 0.5);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	glPopMatrix();
	glEndList();
	
	// draw the main axis
	AxisList2 = glGenLists(1);
	glNewList(AxisList2, GL_COMPILE);
	glColor3f(0.5, 0.5, 1.);
	glPushMatrix();
	glBegin(GL_QUADS);
	a = 0.,a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(0.1 * cos(a), 0.5, 0.1 * sin(a));
		glVertex3f(0.1 * cos(a), 5., 0.1 * sin(a));
		glVertex3f(0.1 * cos(a + r1), 5., 0.1 * sin(a + r1));
		glVertex3f(0.1 * cos(a + r1), 0.5, 0.1 * sin(a+ r1));
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	glPopMatrix();
	glEndList();

	// draw the axis connect the fourth gear and the water wheel
	AxisList3 = glGenLists(1);
	glNewList(AxisList3, GL_COMPILE);
	glColor3f(0.5, 0.5, 1.);
	glPushMatrix();
	glRotatef(90., 1., 0., 0.);
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(0.1 * cos(a), 0., 0.1 * sin(a));
		glVertex3f(0.1 * cos(a), 2.5, 0.1 * sin(a));
		glVertex3f(0.1 * cos(a + r1), 2.5, 0.1 * sin(a + r1));
		glVertex3f(0.1 * cos(a + r1), 0., 0.1 * sin(a + r1 ));
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	glPopMatrix();
	glEndList();


	// draw the four gears


	//draw the first gear on the connected axis 
	GearList1 = glGenLists(1);               
	glNewList(GearList1, GL_COMPILE);
	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();
	glRotatef(-90., 1., 0., 0.);
	// one aspect of the gear 
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0,  0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0,  0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0,  0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// another aspect of the gear
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// connect two aspects of one gear
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 20;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(sin(a), 0., -cos(a));
		glVertex3f(0.3 * cos(a), 0.,  0.3 * sin(a));
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0., 0.5 * sin(a));
		
		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(0.5 * cos(a), 0.,  0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0., 0.5 * sin(a + r1));

		glNormal3f(-sin(a), 0., cos(a));
		glVertex3f(0.5 * cos(a + r1), 0.,  0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0., 0.3 * sin(a + r1));

		glNormal3f(cos(a1 + r1), 0., sin(a1 + r1));
		glVertex3f(0.3 * cos(a + r1), 0.,  0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0.2, 0.3 * sin(a + 2 * r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0., 0.3 * sin(a + 2 * r1));

		a = a + 2 * r1;
		a1 = a1 + 2 * r1;
	}

	glEnd();
	glPopMatrix();
	glEndList();
	
	//draw the second gear on the main axis
	GearList2 = glGenLists(1);
	glNewList(GearList2, GL_COMPILE);
	glColor3f(0.5, 0.1, 0.3);
	glPushMatrix();
	// one aspect of the second gear 
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0, 0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// another aspect of the gear
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// connect two aspects of one gear
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 20;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(sin(a), 0., -cos(a));
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0., 0.5 * sin(a));

		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(0.5 * cos(a), 0., 0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0., 0.5 * sin(a + r1));

		glNormal3f(-sin(a), 0., cos(a));
		glVertex3f(0.5 * cos(a + r1), 0., 0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0., 0.3 * sin(a + r1));

		glNormal3f(cos(a1 + r1), 0., sin(a1 + r1));
		glVertex3f(0.3 * cos(a + r1), 0., 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0.2, 0.3 * sin(a + 2 * r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0., 0.3 * sin(a + 2 * r1));

		a = a + 2 * r1;
		a1 = a1 + 2 * r1;
	}

	glEnd();
	glPopMatrix();
	glEndList();

	//draw the third gear on the main axis 
	GearList3 = glGenLists(1);
	glNewList(GearList3, GL_COMPILE);
	glColor3f(0.8, 0.1, 0.3);
	glPushMatrix();
	// one aspect of the third gear 
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0, 0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// another aspect of the gear
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// connect two aspects of one gear
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 20;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(sin(a), 0., -cos(a));
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0., 0.5 * sin(a));

		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(0.5 * cos(a), 0., 0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0., 0.5 * sin(a + r1));

		glNormal3f(-sin(a), 0., cos(a));
		glVertex3f(0.5 * cos(a + r1), 0., 0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0., 0.3 * sin(a + r1));

		glNormal3f(cos(a1 + r1), 0., sin(a1 + r1));
		glVertex3f(0.3 * cos(a + r1), 0., 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0.2, 0.3 * sin(a + 2 * r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0., 0.3 * sin(a + 2 * r1));

		a = a + 2 * r1;
		a1 = a1 + 2 * r1;
	}

	glEnd();
	glPopMatrix();
	glEndList();

	//draw the fourth gear near the ground  
	GearList4 = glGenLists(1);
	glNewList(GearList4, GL_COMPILE);
	glColor3f(0.9, 0.3, 0.5);
	glPushMatrix();
	glRotatef(-90., 1., 0., 0.);
	// one aspect of the fourth gear 
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0, 0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., -1., 0.);
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// another aspect of the gear
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		a = a + 2 * r1;
	}
	glEnd();
	glBegin(GL_POLYGON);
	a = 0.;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(0., 1., 0.);
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		a = a + r1;
	}
	glEnd();
	// connect two aspects of one gear
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 20;
	r1 = M_PI / 10;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f(sin(a), 0., -cos(a));
		glVertex3f(0.3 * cos(a), 0., 0.3 * sin(a));
		glVertex3f(0.3 * cos(a), 0.2, 0.3 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0., 0.5 * sin(a));

		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(0.5 * cos(a), 0., 0.5 * sin(a));
		glVertex3f(0.5 * cos(a), 0.2, 0.5 * sin(a));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0., 0.5 * sin(a + r1));

		glNormal3f(-sin(a), 0., cos(a));
		glVertex3f(0.5 * cos(a + r1), 0., 0.5 * sin(a + r1));
		glVertex3f(0.5 * cos(a + r1), 0.2, 0.5 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0., 0.3 * sin(a + r1));

		glNormal3f(cos(a1 + r1), 0., sin(a1 + r1));
		glVertex3f(0.3 * cos(a + r1), 0., 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + r1), 0.2, 0.3 * sin(a + r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0.2, 0.3 * sin(a + 2 * r1));
		glVertex3f(0.3 * cos(a + 2 * r1), 0., 0.3 * sin(a + 2 * r1));

		a = a + 2 * r1;
		a1 = a1 + 2 * r1;
	}

	glEnd();
	glPopMatrix();
	glEndList();


	// draw the water wheel 

	
	WaterWheelList = glGenLists(1);;
	glNewList(WaterWheelList, GL_COMPILE);
	glPushMatrix();
	glColor3f(0.1, 0.7, 0.6);
	glRotatef(-90., 1., 0., 0.);
	
	//draw the squares 
	glBegin(GL_QUADS);
	a = 0.;
	r1 = M_PI / 5;
	for (int i = 0; i <= 9; i++)
	{
		glNormal3f( sin(a), 0., -cos(a));
		glVertex3f(1. * cos(a), 0., 1. * sin(a));
		glVertex3f(1.5 * cos(a), 0., 1.5 * sin(a));
		glVertex3f(1.5 * cos(a), -0.5, 1.5 * sin(a));
		glVertex3f(1. * cos(a), -0.5, 1. * sin(a));
		glNormal3f(-sin(a), 0., cos(a));
		glVertex3f(1. * cos(a + 0.001), 0., 1. * sin(a + 0.001));
		glVertex3f(1.5 * cos(a + 0.001), 0., 1.5 * sin(a + 0.001));
		glVertex3f(1.5 * cos(a + 0.001), -0.5, 1.5 * sin(a + 0.001));
		glVertex3f(1. * cos(a + 0.001), -0.5, 1. * sin(a + 0.001));
		a = a + r1;
	}
	glEnd();
	//draw the round loop
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 20;
	r1 = M_PI / 10;
	for (int i = 0; i <= 19; i++)
	{
		glNormal3f(cos(a1), 0., sin(a1));
		glVertex3f(1. * cos(a),  0.,  1. * sin(a));
		glVertex3f(1. * cos(a), -0.5, 1. * sin(a));
		glVertex3f(1. * cos(a + r1), -0.5, 1. * sin(a + r1));
		glVertex3f(1. * cos(a + r1), 0., 1. * sin(a + r1));
		glNormal3f(-cos(a1), 0., -sin(a1));
		glVertex3f(0.999 * cos(a), 0., 0.999 * sin(a));
		glVertex3f(0.999 * cos(a), -0.5, 0.999 * sin(a));
		glVertex3f(0.999 * cos(a + r1), -0.5, 0.999 * sin(a + r1));
		glVertex3f(0.999 * cos(a + r1), 0., 0.999 * sin(a + r1));
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	// draw the four middle sticks
	glPushMatrix();
	glBegin(GL_QUADS);
	a = 0.,a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
		{
			glNormal3f(cos(a1), sin(a1), 0.);
			glVertex3f(0.05 * cos(a), 0.05 * sin(a), 0.);
			glVertex3f(0.05 * cos(a), 0.05 * sin(a), 1.);
			glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 1.);
			glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 0.);
			a = a + r1;
			a1 = a1 + r1;
		}
	glEnd();
	glRotatef(90.,  0., 1., 0.);
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(cos(a1), sin(a1), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 1.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 1.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 0.);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	glRotatef(90., 0., 1., 0.);
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(cos(a1), sin(a1), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 1.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 1.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 0.);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	glRotatef(90., 0., 1., 0.);
	glBegin(GL_QUADS);
	a = 0., a1 = M_PI / 4;
	r1 = M_PI / 2;
	for (int i = 0; i <= 3; i++)
	{
		glNormal3f(cos(a1), sin(a1), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 0.);
		glVertex3f(0.05 * cos(a), 0.05 * sin(a), 1.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 1.);
		glVertex3f(0.05 * cos(a + r1), 0.05 * sin(a + r1), 0.);
		a = a + r1;
		a1 = a1 + r1;
	}
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glEndList();


	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(2.5);
	glLineWidth(1.);
	glEndList();

}


void
Keyboard(unsigned char c, int x, int y)
{
	
	switch (c)
	{
	case 'o':
	case 'O':
		WhichProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		WhichProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu( QUIT );	// will not return here
		break;				

	case 'b':
		VertexOn = 1;
		FragmentOn = 1;
		break;

	case 'f':
		VertexOn = 0;
		FragmentOn = 0;
		break;

	case 'F':
		VertexOn = 0;
		FragmentOn = 1;
		break;

	case 'V':
		VertexOn = 1;
		FragmentOn = 0;
		break;

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}


	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


void
MouseMotion(int x, int y)
{

	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}


	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
Reset()
{
	ActiveButton = 0;
	BodyOn = 1;
	AxesOn = 0;
	Scale = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


static float xx[] = {
		0.f, 1.f, 0.f, 1.f
};

static float xy[] = {
		-.5f, .5f, .5f, -.5f
};

static int xorder[] = {
		1, 2, -3, 4
};

static float yx[] = {
		0.f, 0.f, -.5f, .5f
};

static float yy[] = {
		0.f, .6f, 1.f, 1.f
};

static int yorder[] = {
		1, 2, 3, -2, 4
};

static float zx[] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
};

static float zy[] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
};

static int zorder[] = {
		1, 2, 3, 4, -5, 6
};

const float LENFRAC = 0.10f;

const float BASEFRAC = 1.10f;

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}


// not great programming style, but if you are uncomfortable with compiling
// multiple files in visual studio, this works too...

#include "glslprogram.cpp"


