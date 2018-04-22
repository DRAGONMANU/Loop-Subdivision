#include <GL/glut.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include"Mesh.h"

vector<Mesh> myMesh;    //vector of indexed triangle data structure
int currentMesh=0;   	//current index of Mesh

#define kRotationStep 10.0f
#define kTranslationStep 0.5f
#define BUFFER_LENGTH 64 

// Lights & Materials

GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat position[] = {0.0, 0.0, 2.0, 1.0};
GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};

// Parameters

GLfloat camRotX, camRotY, camPosX, camPosY, camPosZ;

GLint viewport[4];
GLdouble modelview[16];
GLdouble projection[16];

void initLights(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void SetCamera(void)
{
	glTranslatef(0, 0, camPosZ);
	glRotatef(camRotX, 1, 0, 0);
	glRotatef(camRotY, 0, 1, 0);
}

void SetupRC()
{
	
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	initLights();

	// Place Camera
	camRotX = 350.0f;
	camRotY = 680.0f;
	camPosX = 0.0f;
	camPosY = 0.0f;
	camPosZ = -10.5f;
}

void displayMesh()
{
	Mesh mesh;
	mesh=myMesh[currentMesh];

	for(unsigned int i=0; i<mesh.faces.size(); ++i)
	{
		
		glBegin(GL_TRIANGLES);
		
		glNormal3f(mesh.faces[i].faceNormal.x,mesh.faces[i].faceNormal.y,mesh.faces[i].faceNormal.z);
		for(unsigned int j=0; j<mesh.faces[i].ver_id.size();++j)
		{
			glVertex3f(mesh.vertices[mesh.faces[i].ver_id[j]].point.x,mesh.vertices[mesh.faces[i].ver_id[j]].point.y,mesh.vertices[mesh.faces[i].ver_id[j]].point.z);
		}
		glEnd();
	}

}

void updateMesh()
{
	Mesh newMesh;
	if(currentMesh==(int)myMesh.size())
	{
		newMesh=getLoopSub(myMesh[currentMesh-1]);
		setVertexNormal(newMesh);
		newMesh.vertices = getLoopVertices(newMesh);
		myMesh.push_back(newMesh);

	}
}
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();	
	{

		SetCamera();

		glRotatef(85.0, 1.0, 1.0, 1.0);

		displayMesh();
		
		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );		// Retrieve The Modelview Matrix
		glGetDoublev( GL_PROJECTION_MATRIX, projection );	// Retrieve The Projection Matrix
		glGetIntegerv( GL_VIEWPORT, viewport );				// Retrieves The Viewport Values (X, Y, Width, Height)

	}
	glPopMatrix();

	glFlush();
}


void SpecialKeys(unsigned char key, int x, int y)
{
	if(key == 'w') 
	{
		camRotX += kRotationStep;
	}
	if(key == 's') 
	{
		camRotX -= kRotationStep;
	}
	if(key == 'a') 
	{
		camRotY += kRotationStep;
	}
	if(key == 'd') 
	{
		camRotY -= kRotationStep;
	}
	if(key =='q')
	{
		if(currentMesh>=1)
		{
			currentMesh--;
			cout<<"SUBDIVISION level = "<<currentMesh<<endl;
		}
	}
	if(key =='e')
	{
		currentMesh++;
		cout<<"SUBDIVISION level = "<<currentMesh<<endl;
	}
	if(key == 'z') {
		camPosZ += kTranslationStep;
	}
	if(key == 'x') {
		camPosZ -= kTranslationStep;
	}
	if(key== 27) //Escape key
		exit(0);

	updateMesh();

	glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the clipping volume
	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


int main(int argc, char **argv)
{

	Mesh mesh1;
	mesh1.loadFile(argv[1]);
	myMesh.push_back(mesh1);
	setVertexNormal(myMesh[0]);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("LOOP SUBDIVISION");
	SetupRC();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(SpecialKeys);
	glutMainLoop();

	return 0;
}
