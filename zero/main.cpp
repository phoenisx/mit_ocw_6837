#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
using namespace std;

const int MAX_BUFFER_SIZE = 1024;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes
enum COLOR {
    RED, PURPLE, INDIGO, TEAL, END
};
COLOR selected_color = RED;

float CAMERA_POS_DELTA[] = { 0.0, 0.0 };
float CAMERA_POS[] = { 1.0, 1.0, 5.0 };


// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // Change Color on 'c
        selected_color = static_cast<COLOR>(rand() % END);
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        CAMERA_POS_DELTA[1] = 0.1f;
		break;
    case GLUT_KEY_DOWN:
        CAMERA_POS_DELTA[1] = -0.1f;
		break;
    case GLUT_KEY_LEFT:
        CAMERA_POS_DELTA[0] = -.1f;
		break;
    case GLUT_KEY_RIGHT:
        CAMERA_POS_DELTA[0] = .1f;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

void drawModel() {
    glBegin(GL_TRIANGLES);
    for (auto face : vecf) {
        glNormal3d(vecn[face[2] - 1][0], vecn[face[2] - 1][1], vecn[face[2] - 1][2]);
        glVertex3d(vecv[face[0] - 1][0], vecv[face[0] - 1][1], vecv[face[0] - 1][2]);
        glNormal3d(vecn[face[5] - 1][0], vecn[face[5] - 1][1], vecn[face[5] - 1][2]);
        glVertex3d(vecv[face[3] - 1][0], vecv[face[3] - 1][1], vecv[face[3] - 1][2]);
        glNormal3d(vecn[face[8] - 1][0], vecn[face[8] - 1][1], vecn[face[8] - 1][2]);
        glVertex3d(vecv[face[6] - 1][0], vecv[face[6] - 1][1], vecv[face[6] - 1][2]);
    }
    glEnd();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    cout << ">>>> Draw Scene";

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.93725, 0.3255, 0.3137, 1.0}, // Red
                                 {0.6706, 0.27843, 0.73725, 1.0}, // Purple
                                 {0.36078, 0.4196, 0.75294, 1.0}, // Indigo
                                 {0.1490, 0.65098, 0.60392, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[selected_color]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
    CAMERA_POS[0] = CAMERA_POS[0] + CAMERA_POS_DELTA[0];
    CAMERA_POS[1] = CAMERA_POS[1] + CAMERA_POS_DELTA[1];
	GLfloat Lt0pos[] = {CAMERA_POS[0], CAMERA_POS[1], CAMERA_POS[2], 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

    drawModel();
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
    char buffer[MAX_BUFFER_SIZE];
    while (cin.getline(buffer, MAX_BUFFER_SIZE).good()) {
        stringstream ss(buffer);
        string label;
        ss >> label;

        if (label == "v") {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            vecv.push_back(vec);
        }
        else if (label == "vn") {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            vecn.push_back(vec);
        }
        else if (label == "f") {
            vector<unsigned> face;
            string face_values[3];

            ss >> face_values[0] >> face_values[1] >> face_values[2];
            
            for (auto value : face_values) {
                char* _v = &value[0];
                char* split = strtok(_v, "/");
                while (split != NULL)
                {
                    face.push_back(stoi(split));
                    split = strtok(NULL, "/");
                }
            }
            vecf.push_back(face);
        }
    }
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
