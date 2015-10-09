#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "imageloader.h"
#include "vec3f.h"
#include "math.h"

#define PI 3.14159265

using namespace std;

class Vector3 {
        public:
                float x, y, z;
    
                Vector3 () {
                }
                Vector3 (float _x, float _y, float _z) {
                        x = _x; 
                        y = _y; 
                        z = _z; 
                }
                ~Vector3 () {
                }
};


//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;
			
			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}
			
			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}
			
			computedNormals = false;
		}
		
		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;
			
			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}
		
		int width() {
			return w;
		}
		
		int length() {
			return l;
		}
		
		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}
		
		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}
		
		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}
			
			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}
			
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);
					
					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < w - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}
					
					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}
					
					normals2[z][x] = sum;
				}
			}
			
			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];
					
					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}
					
					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}
			
			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;
			
			computedNormals = true;
		}
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}

float _angle = 45.0f;
Terrain* _terrain;

void cleanup() {
	delete _terrain;
}

class Top {
	private:
		float x;
		float y;
		float z;
		float dir;
		float power;

	public:
		Top(float X, float Y, float Z, float Direction, float Power);
		float getX();
		void setX(float X);
		float getY();
		void setY(float Y);
		float getZ();
		void setZ(float Z);
		float getSpeed();
		void setSpeed(float Speed);
		float getDirection();
		void setDirection(float Direction);
};

Top::Top(float X, float Y, float Z, float Direction, float Power): x(X), y(Y), z(Z), dir(Direction), power(Power) {
	cout<<"Top is being Created"<<endl;
}
								   
float Top::getX() {
	return x;
}

float Top::getY() {
	return y;
}

float Top::getZ() {
	return z;
}

void Top :: setX(float X) {
	x = X;
}

void Top :: setY(float Y) {
	y = Y;
}

void Top :: setZ(float Z) {
	z = Z;
}

float Top :: getSpeed() {
	return power;
}

float Top :: getDirection() {
	return dir;
}

void Top :: setSpeed(float Speed) {
	power = Speed;
}

void Top :: setDirection(float Direction) {
	dir = Direction;
}

Top top(128.0, 0.0, 255.0, 0.0, 0.0);
int CameraView=0, Shoot=0, Hits=0, Misses=0;

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanup();
			cout<<"Number of Hits: "<< Hits << " Number of Misses: " << Misses << endl;
			exit(0);

		case ' ': //Shoot
			Shoot = 1;
		break;
		case 'p': //Player View
			CameraView = 1;
		break;

		case 't': //Top View
			CameraView = 2;
		break;

		case 'o': //Overhead View
			CameraView = 3;
		break;

		case 'f': //Follow Cam
			CameraView = 4;
		break;
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void drawSpeedometer(int speed) {
	int i;
	for(i=0;i<speed;i++){
		glColor3f(i*0.05, 0.95-i*0.05, 0.0);
		glTranslatef(0.0, 0.0, -12.0);
		glutSolidCube(10);
	}
}

void drawDirectionMeter(float angle) {
	int i;
	glRotatef(angle, 0, 1, 0);
	for(i=0;i<5;i++) {
		glColor3f(1, 0, 0);
		glTranslatef(0.0, 0.0, -5.0);
		glutSolidCube(4);
	}
	glTranslatef(0.0, 0.0, -2.0);
	glRotatef(-180, 1, 0, 0);
	glutSolidCone(5, 9, 20, 20);
}

void drawHitCounter() {
	int i;
	glColor3f(0, 1, 0);
	for(i=0;i<Hits;i++){
		glTranslatef(0, 0, -12);
		glutSolidCube(10);
	}
}

void drawMissCounter() {
	int i;
	glColor3f(1, 0, 0);
	for(i=0;i<Misses;i++){
		glTranslatef(0, 0, -12);
		glutSolidCube(10);
	}
}

float x, y, z;
int flag=0, speed=1, direction=0, target=0;
float camera_rotation_angle = 0, target_x, target_z, target_y;

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Terrain
	glTranslatef(0.0f, 0.0f, -8.0f);
	if(CameraView == 3)
		glRotatef(90, 1.0, 0.0, 0.0);
	else
		glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(90, 0, 1, 0);
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	glScalef(scale, scale, scale);
	if (CameraView == 2){
		gluLookAt(top.getX(), top.getY(), top.getZ(), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	else
		glTranslatef(-(float)(_terrain->width() - 1) / 2,
				 0.0f,
				 -(float)(_terrain->length() - 1) / 2);
	
	glColor3f(1.0f, 1.0f, 1.0f);
	for(int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}

	// Bottom Part of the Top
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(top.getX(), top.getY()+9.0, top.getZ());
	glRotatef(90, 1, 0, 0);
	Vec3f N = _terrain->getNormal(top.getX(), top.getZ());     
	float rotation_angle = acos(N[1] / (sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2])));     
	glRotatef(-rotation_angle / PI * 180, -N[2] / (sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2])), 0, N[0] / (sqrt(N[0] * N[0] + N[1] * N[1] + 		    N[2] * N[2])));
	glRotatef(-_angle, 0.0, 0.0, 1.0);
	glutSolidCone(6.0f, 9.0f, 20, 20);
	glPopMatrix();

	//Middle Part of the Top
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	glTranslatef(top.getX(), top.getY()+9.0, top.getZ());
	glRotatef(90, 1, 0, 0);
	N = _terrain->getNormal(top.getX(), top.getZ());     
	rotation_angle = acos(N[1] / (sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2])));     
	glRotatef(-rotation_angle / PI * 180, -N[2] / (sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2])), 0, N[0] / (sqrt(N[0] * N[0] + N[1] * N[1] + 		    N[2] * N[2])));
	glRotatef(-_angle, 0.0, 0.0, 1.0);
	glutWireTorus(2.25f, 4.5f, 10, 10);
	glPopMatrix();

	//Top most part of the Top
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(top.getX(), top.getY()+9.0, top.getZ());
	glRotatef(-90, 1, 0, 0);
	N = _terrain->getNormal(top.getX(), top.getZ());     
	rotation_angle = acos(N[1] / (sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2])));     
	glRotatef(-rotation_angle / PI * 180, -N[2] / (sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2])), 0, N[0] / (sqrt(N[0] * N[0] + N[1] * N[1] + 		    N[2] * N[2])));
	glRotatef(_angle, 0.0, 0.0, 1.0);
	glutSolidCone(2.25, 5.0f, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(270, 0.0, 255);
	drawSpeedometer(speed);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(128, 0.0, 310);
	drawDirectionMeter(direction);
	glPopMatrix();

	
	glPushMatrix();
		glTranslatef(target_x, target_y+5.0, target_z);
	glColor3f(1.0, 0.0, 0.0);
	glutSolidSphere(5.0, 20, 20);
	glColor3f(0.0, 0.0, 0.0);
	glutSolidTorus(2.0, 4.0, 20, 20);
	glColor3f(1.0, 0.0, 0.0);
	glutSolidTorus(2.0, 6.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-25, 0.0, 255);
	drawHitCounter();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-45, 0.0, 255);
	drawMissCounter();
	glPopMatrix();
	
	glutSwapBuffers();
	camera_rotation_angle++;
}

void update(int value) {
	if(Shoot){
        	_angle = _angle + speed*2;
        	if (_angle > 360) {
                	_angle -= 360;
        	}

		if(direction >= 0) {
			top.setX(top.getX() - 0.1*speed*sin(atan(direction*PI/180)));
			top.setZ(top.getZ() - 0.1*speed*cos(atan(direction*PI/180)));
			top.setY(_terrain->getHeight(top.getX(), top.getZ()));
		}

		if(direction < 0) {
			top.setX(top.getX() + 0.1*speed*sin(atan(-direction*PI/180)));
			top.setZ(top.getZ() - 0.1*speed*cos(atan(-direction*PI/180)));
			top.setY(_terrain->getHeight(top.getX(), top.getZ()));
		}
		if(top.getX() <= 0 || top.getX() >= 255 || top.getZ() <= 0 || (top.getX() == target_x || top.getZ() == target_z))
		{
			speed=1;
			Shoot=0;
			direction = 0;
			target = 0;
			Misses++;
			top.setX(128.0);
			top.setZ(255.0);
			top.setY(_terrain->getHeight(128, 255));
		}
	}
	if(!target){
	target_x = (rand()%190)+64;
	target = 1;
	target_z = (rand()%190)+64;
	target_y = _terrain->getHeight(target_x, target_z);
	}

	if(sqrt(pow((top.getX()-target_x),2)+pow((top.getY()-target_y),2)+pow((top.getZ()-target_z),2)) <= 11 || sqrt(pow((top.getX()-target_x),2)+pow((top.getY()-target_y),2)+pow((top.getZ()-target_z),2)) - 11 < 2.0){
		Hits++;
		speed=1;
		Shoot=0;
		direction = 0;
		target = 0; 
		top.setX(128);
		top.setZ(255.0);
		top.setY(_terrain->getHeight(128, 255));
	}
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

void handleKeypress2(int key, int x, int y) {
	if(key == GLUT_KEY_UP && speed<20 && !Shoot) {
		speed++;
	}
	
	if(key == GLUT_KEY_DOWN && speed>1 && !Shoot) {
		speed--;
	}
	
	if(key == GLUT_KEY_LEFT && direction<83 && !Shoot) {
		direction += 7;
	}
	
	else if(key == GLUT_KEY_LEFT && direction>83 && !Shoot) {
		direction = 90;
	}

	if(key == GLUT_KEY_RIGHT && direction > -83 && !Shoot) {
		direction -= 7;
	}
	
	else if(key == GLUT_KEY_RIGHT && direction < -83 && !Shoot) {
		direction = -90;
	}
}

int main(int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("3D Game");
	initRendering();

	_terrain = loadTerrain("heightmap.bmp", 20);
	top.setY(_terrain->getHeight(top.getX(), top.getZ()));

	cout<<"Width: "<<_terrain->width()<<" Length: "<<_terrain->length()<<endl;

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialFunc(handleKeypress2);
	glutReshapeFunc(handleResize);
	glutFullScreen();
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









