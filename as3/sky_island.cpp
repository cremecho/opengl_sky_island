
#define GLEW_STATIC
#define FREEGLUT_STATIC

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector"
using namespace std;

//..............       global variable       ..............//
#define checkImageWidth 48 
#define checkImageHeight 48

int window_width = 800, window_height = 800;
int sx = 250, sy = 400, sz = 0;		//diffuse light (sun) position 
//animation and input interaction vatiables
int flip = 0;
int idea = 0;		
int time_interval = 10;		//timer interval
int camera_time = 0;
int weather = 0;
int snow_time = 0;
bool flag = FALSE, flag2 = FALSE;
bool flipFlag = FALSE;
// image texture parameters
vector<GLubyte*>p;
GLint imagewidth[6]; 
GLint imageheight[6];
GLint pixellength[6];
GLuint texture[6];		//number of image texture 

GLfloat xMin = -40.0, yMin = -60.0, xMax = 40.0, yMax = 60.0, dnear = 150.0, dfar = 2500.0;		//clipping window and near-far plane distance 
GLfloat lookX = 0.0, lookY = 600.0, lookZ = 1500.0;		//camera position1
GLfloat deltaX = 0, deltaY = -900 / 100, deltaZ = -1000 / 100;	//distance from camera position1 to position2
GLfloat xref = 0.0, yref = 50.0, zref = 0.0; // look-at point
GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0; // view-up vector
GLfloat spot_x, spot_z;	//searchlight light direction
double angle;

//..............       functions       ..............//

//..............       general methods       ..............//
void keyboard_input(unsigned char key, int x, int y) {		//keyboard interaction, can also use menu
	if (key == '1') {
		weather = 0;
	}
	if (key == '2') {
		weather = 1;
	}
	if (key == '3') {
		weather = 2;
	}

	if (key == 'c' && camera_time == 0) {
		flag = TRUE;
	}
	else if (key == 'c' && camera_time == 100) {
		flag2 = TRUE;
	}

	if (key == 'r') {
		flipFlag = TRUE;
	}

	if (key == 27) {
		exit(0);
	}
}


void reshapeWindow(GLint intNewWidth, GLint intNewHeight) {		//reshape windows
	glViewport(0, 0, intNewWidth, intNewHeight);
}


void init() {	//initial setting
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(xMin, xMax, yMin, yMax, dnear, dfar);

}


void ReadImage(const char path[256], GLint& imagewidth, GLint& imageheight, GLint& pixellength){	//read an image and store in pixels
	GLubyte* pixeldata;
	FILE* pfile;
	fopen_s(&pfile, path, "rb");
	if (pfile == 0) exit(0);
	fseek(pfile, 0x0012, SEEK_SET);
	fread(&imagewidth, sizeof(imagewidth), 1, pfile);
	fread(&imageheight, sizeof(imageheight), 1, pfile);
	imageheight = imageheight;
	pixellength = imagewidth * 3;
	while (pixellength % 4 != 0)pixellength++;
	pixellength *= imageheight;
	pixeldata = (GLubyte*)malloc(pixellength);
	if (pixeldata == 0) exit(0);
	fseek(pfile, 54, SEEK_SET);
	fread(pixeldata, pixellength, 1, pfile);
	p.push_back(pixeldata);
	fclose(pfile);
}


void bindPicture() {	//bind pictures with texture
	//skybox picture from https://garrysmods.org/download/12004/skybox-minecraft
	ReadImage("right.bmp", imagewidth[0], imageheight[0], pixellength[0]);	
	ReadImage("up.bmp", imagewidth[1], imageheight[1], pixellength[1]);
	ReadImage("down.bmp", imagewidth[2], imageheight[2], pixellength[2]);
	ReadImage("leftbackfront.bmp", imagewidth[3], imageheight[3], pixellength[3]);
	//soil picture from http://www.3dczk.com/map1/ground/smhd/2015/1106/694.html
	ReadImage("soil.bmp", imagewidth[4], imageheight[4], pixellength[4]);
	//from game AzurLane
	ReadImage("chairr.bmp", imagewidth[5], imageheight[5], pixellength[5]);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
	glGenTextures(5, &texture[0]); // number of texture names to be generated and array of texture names
}


void animation() {		//idle function used for animation
	idea -= 1;
	if (idea < -380) {
		idea = 400;
	}
	glutPostRedisplay();
}


void OnTimer(int Value) {	//timer function

	if (flag) {		//camera point change smoothly
		lookY -= deltaY;
		lookZ += deltaZ;
		camera_time += 1;
		if (camera_time == 100) {
			flag = FALSE;

		}
	}

	if (flag2) {	//camera point change smoothly
		lookY += deltaY;
		lookZ -= deltaZ;
		camera_time -= 1;
		if (camera_time == 0)
			flag2 = FALSE;
	}

	if (weather == 2) {		//snow drop
		snow_time += 5;
		if (snow_time > 300)
			snow_time = 0;
	}

	angle++;				// searchlight rotation
	spot_x = cos(angle * 3.14 / 180);
	spot_z = sin(angle * 3.14 / 180);

	if (flipFlag) {
		flip += 10;
		if (flip == 360) {
			flipFlag = FALSE;
			flip = 0;
		}
	}
	glutTimerFunc(time_interval, OnTimer, 1);
}


void menu(int item) //mouse and keyboard can control the display altogether
{
	keyboard_input((unsigned char)item, 0, 0);
}

//..............       buildings       ..............//
void plane(void) {		//part of island
	glPushMatrix();
	glColor3f(0.8, 0.8, 0.8);
	glTranslatef(0.0, 0.0, 0.0);
	glScalef(1, 0.1, 1);
	glutSolidCube(700);
	glPopMatrix();
}


void cone(void) {		//part of island
	glPushMatrix();
	glColor3f(0, 0, 1);
	glRotatef(80, 1, 0, 0);
	glTranslatef(0.0, 0.0, 0.0);
	glutSolidCone(100, 400, 200, 200);
	glPopMatrix();
}


void island(void(*a)(void), void(*b)()){	//draw island
	glPushAttrib(GL_ALL_ATTRIB_BITS);		//'or' between cone and plane

	glEnable(GL_DEPTH_TEST);
	a(); b();
	glPopAttrib();
}


void tree() {		//draw the tree
	//tree1
	glPushMatrix();
	glTranslatef(-200.0, 0.0, 300);
	glPushMatrix();
	glTranslatef(0.0, 100.0, 0.0);
	glScalef(60.0, 65.0, 60.0);
	glColor3f(0.0, 0.7, 0.1);
	glutSolidIcosahedron();
	glColor3f(0.0, 0.0, 0.0);
	glutWireIcosahedron();
	glPopMatrix();
	glPopMatrix();
	//tree1 frame
	glPushMatrix();
	glTranslatef(-200.0, 0.0, 300);
	glPushMatrix();
	glTranslatef(0.0, 25.0, 0.0);
	glScalef(15, 50.0, 15);
	glColor3f(0.7, 0.3, 0.0);
	glutSolidCube(1);
	glColor3f(0.0, 0.0, 0.0);
	glutWireCube(1);
	glPopMatrix();
	glPopMatrix();
	//tree2 
	glPushMatrix();
	glTranslatef(-200.0, 0.0, -300);
	glPushMatrix();
	glTranslatef(0.0, 100.0, 0.0);
	glScalef(60.0, 65.0, 60.0);
	glColor3f(0.0, 0.7, 0.1);
	glutSolidIcosahedron();
	glColor3f(0.0, 0.0, 0.0);
	glutWireIcosahedron();
	glPopMatrix();
	glPopMatrix();
	//tree2 frame
	glPushMatrix();
	glTranslatef(-200.0, 0.0, -300);
	glPushMatrix();
	glTranslatef(0.0, 25.0, 0.0);
	glScalef(15, 50.0, 15);
	glColor3f(0.7, 0.3, 0.0);
	glutSolidCube(1);
	glColor3f(0.0, 0.0, 0.0);
	glutWireCube(1);
	glPopMatrix();
	glPopMatrix();
}


void house() {		//draw the house
	//house and house frame
	glPushMatrix();
	glTranslatef(200.0, 50.0, 0);
	glColor3f(1, 0.9, 0.6);
	glutSolidCube(100);
	glColor3f(0.0, 0.0, 0.0);
	glutWireCube(100);
	glPushMatrix();
	glTranslatef(0, 50, 0);
	glScalef(1.9, 1.9, 1.9);
	glRotatef(90, -1, 0, 0);
	glColor3f(0.8, 0.4, 0);
	//roof
	glBegin(GL_POLYGON);
	glVertex3f(-30, -30, 0);
	glVertex3f(-30, 30, 0);
	glVertex3f(30, 30, 0);
	glVertex3f(30, -30, 0);
	glEnd();
	
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-30, -30, 0);
	glVertex3f(-30, 30, 0);
	glVertex3f(0, 0, 50);
	glVertex3f(30, 30, 0);
	glVertex3f(30, -30, 0);
	glVertex3f(0, 0, 50);
	glVertex3f(-30, -30, 0);
	glEnd();
	//roof frame
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(-30, -30, 0);
	glVertex3f(-30, 30, 0);
	glVertex3f(30, 30, 0);
	glVertex3f(30, -30, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-30, -30, 0);
	glVertex3f(-30, 30, 0);
	glVertex3f(0, 0, 50);
	glVertex3f(30, 30, 0);
	glVertex3f(30, -30, 0);
	glVertex3f(0, 0, 50);
	glVertex3f(-30, -30, 0);
	glEnd();

	glPopMatrix();
	glPopMatrix();
}

void car() {	//draw a car
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(0, idea, -60);		//animation
	glRotatef(0, 1, 0, 0);
	glRotatef(270, 0, 1, 0);
	glRotatef(90, 0, 0, 1);
	glRotatef(flip, 0, 1, 0);		// flip

	//upper part
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 0.95);
	glVertex3f(-40, 0, 30);
	glVertex3f(-20, 20, 30);
	glVertex3f(40, 20, 30);
	glVertex3f(50, 0, 30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 1);
	glVertex3f(-40, 0, -30);
	glVertex3f(-20, 20, -30);
	glVertex3f(40, 20, -30);
	glVertex3f(50, 0, -30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 1, 1);
	glVertex3f(-20, 20, 30);
	glVertex3f(-20, 20, -30);
	glVertex3f(40, 20, -30);
	glVertex3f(40, 20, 30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.5, 0.8, 0.8);
	glVertex3f(-20, 20, 30);
	glVertex3f(-40, 0, 30);
	glVertex3f(-40, 0, -30);
	glVertex3f(-20, 20, -30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0.5, 0.5);
	glVertex3f(40, 20, 30);
	glVertex3f(40, 20, -30);
	glVertex3f(50, 0, -30);
	glVertex3f(50, 0, 30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 1);
	glVertex3f(-60, -30, 30);
	glVertex3f(-60, -30, -30);
	glVertex3f(60, -30, -30);
	glVertex3f(60, -30, 30);
	glEnd();
	//wheels
	glColor3f(1, 0, 0);
	glTranslated(-40, -30, 30);
	glutSolidTorus(4, 10, 5, 100);
	glTranslated(0, 0, -60);
	glutSolidTorus(4, 10, 5, 100);
	glTranslated(90, 0, 0);
	glutSolidTorus(4, 10, 5, 100);
	glTranslated(0, 0, 60);
	glutSolidTorus(4, 10, 5, 100);
	//down part
	glPushMatrix();
	glTranslatef(-45, 20, -30);
	glBegin(GL_POLYGON);
	glColor3f(0.15, 0.15, 0.85);
	glVertex3f(-80, 0, 30);
	glVertex3f(80, 0, 30);
	glColor3f(1, 0.5, 0.8);
	glVertex3f(80, -30, 30);
	glVertex3f(-80, -30, 30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.3, 0.2, 0.5);
	glVertex3f(80, 0, -30);
	glVertex3f(80, 0, 30);
	glVertex3f(80, -30, 30);
	glVertex3f(80, -30, -30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.35, 0.15, 0.35);
	glVertex3f(-80, 0, -30);
	glVertex3f(-80, 0, 30);
	glVertex3f(-80, -30, 30);
	glVertex3f(-80, -30, -30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.15, 0.15, 0.85);
	glVertex3f(-80, 0, -30);
	glVertex3f(80, 0, -30);
	glColor3f(1, 0.5, 0.8);
	glVertex3f(80, -30, -30);
	glVertex3f(-80, -30, -30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 0.95);
	glVertex3f(-80, 0, 30);
	glVertex3f(-80, 0, -30);
	glVertex3f(80, 0, -30);
	glVertex3f(80, 0, 30);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.85, 0.55, 0.25);
	glVertex3f(-80, -30, 30);
	glVertex3f(-80, -30, -30);
	glVertex3f(80, -30, -30);
	glVertex3f(80, -30, 30);
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}


void chair() {	//draw a chair
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth[5], imageheight[5], 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[5]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	
	glTranslatef(250, 40, 270);
	if (camera_time != 0) {
		glRotatef(90, -1, 0, 0);
	}
	glScalef(2, 2, 2);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-20.0, -20.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-20.0, 20.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, 20.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, -20.0, 0.0);
	glPopMatrix();
	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

//..............       Background       ..............//
void checkGround() {		//draw a check ground
	GLubyte checkImage[checkImageWidth][checkImageHeight][3];

	int i, j, c;
	for (i = 0; i < checkImageWidth; i++) {
		for (j = 0; j < checkImageHeight; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x10) == 0))) * 255;		//reduce ckeck number in X direction
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, checkImageWidth, checkImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, &checkImage[0][0][0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);

	//draw the ground
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 13, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);glVertex3f(-75.0, -370.0, 0.0);
	glTexCoord2f(0.0, 1.0);glVertex3f(-75.0, 330.0, 0.0);
	glTexCoord2f(1.0, 1.0);glVertex3f(73.0, 330.0, 0.0);
	glTexCoord2f(1.0, 0.0);glVertex3f(73.0, -370.0, 0.0);
	glPopMatrix();
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


void otherGround() {	//tree ground and house ground
	//tree ground
	glPushMatrix();
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(-290, 20, 0);
	glColor3f(0, 1, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-60.0, -370.0, 0.0);
	glVertex3f(-60.0, 330.0, 0.0);
	glVertex3f(214.5, 330.0, 0.0);
	glVertex3f(214.5, -370.0, 0.0);
	glEnd();
	glPopMatrix();

	//house ground using texture
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth[4], imageheight[4], 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[4]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glRotatef(90, 1, 0, 0);
	glTranslatef(135, 20, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-60.0, -370.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-60.0, 330.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(214.5, 330.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(214.5, -370.0, 0.0);
	glPopMatrix();
	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

}


void lighten() {
	//light data
	GLfloat ambient_Light[] = { 0.3,0.3,0.3,1 };
	GLfloat diffuse_Light[] = { 255 / 255, 245 / 255, 204 / 255, 0.9 };
	GLfloat specular_Light[] = { 1,0.9,0.9,1 };
	//light postition data
	GLfloat position[] = { sx, sy, sz, 0.0 };
	GLfloat spot_light_direction[] = { -200, 50, 0 };
	GLfloat spot_direction[] = { spot_x, 0,  spot_z };
	//material data
	GLfloat diffuse_Material[4] = { 255 / 255, 245 / 255, 204 / 255, 0.9 };
	GLfloat ambient_Material[4] = { 1,1,1,1 };
	GLfloat specular_Material[4] = { 1,0.9,0.9,1 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_Light);		//light1 ambient
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse_Light);		//light2 diffuse (sun)
	glLightfv(GL_LIGHT2, GL_POSITION, position);			
	glLightfv(GL_LIGHT3, GL_SPECULAR, specular_Light);		//light3 spot light (beacon)
	glLightfv(GL_LIGHT3, GL_POSITION, spot_light_direction);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 50.0);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 2.0);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_Material);	//materials
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_Material);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_Material);
	glMaterialf(GL_FRONT, GL_SHININESS, 1);
	glColorMaterial(GL_FRONT, GL_AMBIENT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	if (weather == 0)
		glEnable(GL_LIGHT2);
	else
		glDisable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//draw beacon
	glPushMatrix();
	glTranslatef(spot_light_direction[0], spot_light_direction[1] - 30, spot_light_direction[2]);
	glRotatef(270, 1, 0, 0);
	glColor3f(100, 100, 100);
	glutSolidCylinder(20,70,50,50);
	//level2
	int n = 100;
	glPushMatrix();
	glTranslatef(0, 0, 70.5);
	glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(30 * cos(2 * 3.14 / n * i), 30 * sin(2 * 3.14 / n * i));	
	}
	glEnd();
	glColor3f(100, 100, 100);
	glutSolidCylinder(15, 30, 50, 50);
	glPopMatrix();
	//level3
	glPushMatrix();
	glTranslatef(0, 0, 100);
	glColor3f(1, 0, 0);
	glutSolidCone(20, 20, 50, 50);
	glPopMatrix();
	//searchlight
	glPushMatrix();
	glTranslatef(0, 0, 90);
	glRotatef(90, 1, 0, 0);
	glRotatef(angle, 0, -1, 0);		//rotate animation
	glPushMatrix();
	glTranslatef(15, 0, 0);
	glColor3f(1, 1, 0);
	glutSolidSphere(5, 10, 10);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}


void skybox_singleside() {		//draw one side of skybox
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -100.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-100.0, 100.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(100.0, 100.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(100.0, -100.0, 0.0);
	glEnd();
}


void skybox(){		//draw the skybox
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, texture[i]); // target to which texture is bound and name of a texture, from 0 to 3
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth[i], imageheight[i], 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	//front side
	glPushMatrix();
	glScalef(5, 5, 5);
	glTranslatef(0, 150, 0);
	glRotatef(90, 1, 0, 0);
	if (weather == 0)
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	else
		glBindTexture(GL_TEXTURE_2D, texture[3]);
	skybox_singleside();
	glPopMatrix();
	//up side
	glPushMatrix();
	glScalef(6, 6, 6);
	glTranslatef(0, 60, 100 + camera_time);
	glRotatef(20, -1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	skybox_singleside();
	glPopMatrix();
	//down side
	glPushMatrix();
	glScalef(5, 5, 5);
	glTranslatef(0, 30, -98);
	glScalef(1.5, 1.5, 1.5);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	skybox_singleside();
	glPopMatrix();
	//left side
	glPushMatrix();
	glScalef(7, 5, 5);
	glTranslatef(-100 * 5 / 7, 50, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	skybox_singleside();
	glPopMatrix();
	//right side
	glPushMatrix();
	glScalef(7, 5, 5);
	glTranslatef(100 * 5 / 7, 50, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	skybox_singleside();
	glPopMatrix();
	//end
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}


void snow() {	//drow snow
	if (weather == 2) {
		glPushMatrix();
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					glPushMatrix();
					int x = 300 * i - snow_time;	//snow drop animation
					if (x < -40) {
						glPopMatrix();
						break;
					}
					glTranslatef(-300 + 360 * k, x, 300 - 300 * j);

					glColor4f(1, 1, 1, 0.7);
					glutSolidSphere(10, 10, 10);
					glTranslatef(100, 0, 0);
					glutSolidSphere(10, 10, 10);
					glTranslatef(-30, 70, 0);
					glutSolidSphere(10, 10, 10);
					glTranslatef(80, 50, -40);
					glutSolidSphere(10, 10, 10);
					glTranslatef(50, -150, -50);
					glutSolidSphere(10, 10, 10);
					glTranslatef(50, 0, 0);
					glutSolidSphere(10, 10, 10);
					glTranslatef(0, 60, 0);
					glutSolidSphere(10, 10, 10);
					glTranslatef(50, -100, -40);
					glutSolidSphere(10, 10, 10);
					glPopMatrix();
				}
			}

		}
		glPopMatrix();
	}
}

//.................  main display function .................//
void display_object() {		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(lookX, lookY, lookZ, xref, yref, zref, Vx, Vy, Vz);	//look at point, two values
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			//enable to change alpha
	glEnable(GL_BLEND);

	glPushMatrix();
	
	island(cone, plane);
	glPushMatrix();
	glTranslatef(0, 35.5, 0);
	otherGround();
	tree();
	house();
	chair();
	checkGround();
	car();
	lighten();
	skybox();
	snow();
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Sky Island");
	init();
	bindPicture();
	glutDisplayFunc(display_object);
	glutReshapeFunc(reshapeWindow);
	glutKeyboardFunc(keyboard_input);
	glutTimerFunc(time_interval, OnTimer, 1);
	glutIdleFunc(animation);
	//Menu
	int wea;
	wea = glutCreateMenu(menu);
	glutAddMenuEntry("Sunny", '1');
	glutAddMenuEntry("Cloudy", '2');
	glutAddMenuEntry("Snowy", '3');
	glutCreateMenu(menu);
	glutAddMenuEntry("Sky Island Operations Demo", '\0');
	glutAddMenuEntry(" ", '\0');
	glutAddSubMenu("Change Weather", wea);
	glutAddMenuEntry("Change Camera (c)", 'c');
	glutAddMenuEntry("Don't you want a super flip?! (r)", 'r');
	glutAddMenuEntry(" ", '\0');
	glutAddMenuEntry("Quit (Esc)", '\033');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}
