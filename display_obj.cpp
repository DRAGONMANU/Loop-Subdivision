#include <GL/glut.h>
#include<GL/glu.h>
#include<GL/glut.h>


#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include"Mesh.h"

// #include "RgbImage.h"




int first_x=1;

GLuint texture[2];


struct Image {

    unsigned long sizeX;

    unsigned long sizeY;

    char *data;

};

typedef struct Image Image;


#define checkImageWidth 64

#define checkImageHeight 64


GLubyte checkImage[checkImageWidth][checkImageHeight][3];

void makeCheckImage(void){

    int i, j, c;

    for (i = 0; i < checkImageWidth; i++) {

        for (j = 0; j < checkImageHeight; j++) {

            c = ((((i&0x8)==0)^((j&0x8)==0)))*255;

            checkImage[i][j][0] = (GLubyte) c;

            checkImage[i][j][1] = (GLubyte) c;

            checkImage[i][j][2] = (GLubyte) c;

        }

    }

}



int ImageLoad(char *filename, Image *image) {

    FILE *file;

    unsigned long size; // size of the image in bytes.

    unsigned long i; // standard counter.

    unsigned short int planes; // number of planes in image (must be 1)

    unsigned short int bpp; // number of bits per pixel (must be 24)

    char temp; // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.

    cout<<"file name"<<filename<<endl;
    if ((file = fopen(filename, "rb"))==NULL){

        printf("File Not Found : %s\n",filename);

        return 0;

    }

    // seek through the bmp header, up to the width/height:

    fseek(file, 18, SEEK_CUR);

    // read the width

    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {

        printf("Error reading width from %s.\n", filename);

        return 0;

    }

    cout << "size_x -> "<<image->sizeX<<endl;

    //printf("Width of %s: %lu\n", filename, image->sizeX);

    // read the height

    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {

        printf("Error reading height from %s.\n", filename);

        return 0;

    }

    //printf("Height of %s: %lu\n", filename, image->sizeY);

    // calculate the size (assuming 24 bits or 3 bytes per pixel).

    size = image->sizeX * image->sizeY * 3;

    // read the planes

    if ((fread(&planes, 2, 1, file)) != 1) {

        printf("Error reading planes from %s.\n", filename);

        return 0;

    }

    if (planes != 1) {

        printf("Planes from %s is not 1: %u\n", filename, planes);

        return 0;

    }

    // read the bitsperpixel

    if ((i = fread(&bpp, 2, 1, file)) != 1) {

        printf("Error reading bpp from %s.\n", filename);

        return 0;

    }

    if (bpp != 24) {

        printf("Bpp from %s is not 24: %u\n", filename, bpp);

        return 0;

    }

    // seek past the rest of the bitmap header.

    fseek(file, 24, SEEK_CUR);

    // read the data.

    image->data = (char*) malloc(size);

	cout<<"x-> "<<image->sizeX << " Y-> "<<image->sizeY;

    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");

        return 0;

    }

    if ((i = fread(image->data, size, 1, file)) != 1) {

        printf("Error reading image data from %s.\n", filename);

        return 0;

    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)

        temp = image->data[i];

        image->data[i] = image->data[i+2];

        image->data[i+2] = temp;

    }
    fclose(file);
    // we're done.

    return 1;

}


Image * loadTexture(){

    Image *image1;

    // allocate space for texture

    image1 = (Image *) malloc(sizeof(Image));

    cout<< "IMage size ->"<<sizeof(Image)<<endl;
    if (image1 == NULL) {

        printf("Error allocating space for image");

        exit(0);

    }

    if (!ImageLoad("Cube.bmp", image1)) {
    	cout <<"not loaded"<<endl;
        exit(1);
    }

    return image1;

}


// GLuint  texture[1];         // Storage For One Texture ( NEW )
/*
* Read a texture map from a BMP bitmap file.
*/
void loadTextureFromFile(char *filename)
{   
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	// png::image<png::rgba_pixel> image("diffuse.png");
	
	// RgbImage theTexMap( filename );

	// Pixel alignment: each row is word aligned (aligned to a 4 byte boundary)
	//    Therefore, no need to call glPixelStore( GL_UNPACK_ALIGNMENT, ... );
    cout<<"Load texture before"<<endl;

    Image *image1 = loadTexture();

    cout<<"Load texture after"<<endl;

	glGenTextures(1, &texture[0]);                  // Create The Texture
	glBindTexture(GL_TEXTURE_2D, texture[0]);

// const void* buffer = image.get_pixbuf().get_row(0).get_data();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Typical Texture Generation Using Data From The Bitmap


	// glTexImage2D(GL_TEXTURE_2D, 0, 3, image1.get_width(), image.get_height(), 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );
	 glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

}

vector<Mesh> myMesh;    //vector of indexed triangle data structure
int currentMesh = 0;   	//current index of Mesh

#define kRotationStep 10.0f
#define kTranslationStep 0.5f

// Lights & Materials

GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat position[] = {0.0, 0.0, 2.0, 1.0};
GLfloat position1[] = {0.0, 2.0, 0.0, 1.0};
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

	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);

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
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	initLights();

	// Place Camera
	camRotX = 0;
	camRotY = 0;
	camPosX = 0.0f;
	camPosY = 0.0f;
	camPosZ = -10.5f;
}

void displayMesh()
{
	Mesh mesh;
	cout<<"gg"<<endl;
	mesh = myMesh[currentMesh];

	cout<<"in displayMesh"<<endl;

	char* filename = "Cube.png";



	if(first_x == 1){
		loadTextureFromFile(filename);
		first_x ++;
	}
	cout<<"sd"<<endl;
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	cout<<"DISSSSSSSSSSSSSS"<<endl;
	for(unsigned int i = 0; i < mesh.faces.size(); ++i)
	{
		cout<<"ll"<<endl;
		glBegin(GL_TRIANGLES);
		
		glNormal3f(mesh.faces[i].faceNormal.x,mesh.faces[i].faceNormal.y,mesh.faces[i].faceNormal.z);
		// for(unsigned int j=0; j<mesh.faces[i].ver_id.size();++j)
		// {
		// 	glVertex3f(mesh.vertices[mesh.faces[i].ver_id[j]].point.x,mesh.vertices[mesh.faces[i].ver_id[j]].point.y,mesh.vertices[mesh.faces[i].ver_id[j]].point.z);

		// }

		Edge edge;

		edge = mesh.edges[mesh.faces[i].edge_id];
		do
		{
			edge = mesh.edges[edge.next_id];
			glTexCoord2f((mesh.texture_mapping[edge.texture_id]).x , (mesh.texture_mapping[edge.texture_id]).y);
			glVertex3f(mesh.vertices[edge.vertex_id].point.x , mesh.vertices[edge.vertex_id].point.y , mesh.vertices[edge.vertex_id].point.z);
		}while(edge.id!=mesh.edges[mesh.faces[i].edge_id].id);

		glEnd();
	}
	// int t = currentMesh;
	// while(t>0)
	// {
	// 	mesh = myMesh[t-1];
	// 	int delta = (currentMesh - t)*1;

	// 	for(unsigned int i=0; i<mesh.faces.size(); ++i)
	// 	{
			
	// 		glBegin(GL_TRIANGLES);
		
	// 		glNormal3f(mesh.faces[i].faceNormal.x,mesh.faces[i].faceNormal.y,mesh.faces[i].faceNormal.z);
	// 		// for(unsigned int j=0; j<mesh.faces[i].ver_id.size();++j)
	// 		// {
	// 		// 	glVertex3f(mesh.vertices[mesh.faces[i].ver_id[j]].point.x,mesh.vertices[mesh.faces[i].ver_id[j]].point.y,mesh.vertices[mesh.faces[i].ver_id[j]].point.z);

	// 		// }

	// 		Edge edge;

	// 		edge = mesh.edges[mesh.faces[i].edge_id];
	// 		do
	// 		{
	// 			edge = mesh.edges[edge.next_id];
	// 			glTexCoord2f((mesh.texture_mapping[edge.texture_id]).x , (mesh.texture_mapping[edge.texture_id]).y);
	// 			glVertex3f(mesh.vertices[edge.vertex_id].point.x , mesh.vertices[edge.vertex_id].point.y , mesh.vertices[edge.vertex_id].point.z);
	// 		}while(edge.id!=mesh.edges[mesh.faces[i].edge_id].id);

	// 		glEnd();
	// 	}
	// 	t--;
	// }

}

void updateMesh()
{
	Mesh newMesh;
	if(currentMesh==(int)myMesh.size())
	{
		cout<<"In updateMesh"<<endl;

		newMesh = getLoopSub(myMesh[currentMesh-1]);
		setVertexNormal(newMesh);
		cout<<"In updateMesh"<<endl;
		// newMesh.vertices = getLoopVertices(newMesh);
		myMesh.push_back(newMesh);

	}
}
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();	
	{

		SetCamera();



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
	cout<<"main"<<endl;
	SetupRC();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(SpecialKeys);
	glutMainLoop();

	return 0;
}
