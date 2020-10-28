#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctime>
#include <cstdlib>

#define KEY_ESC (key == 27)
#define KEY_SPACEBAR (key == 32)
#define KEY_W (key == 'w' || key == 'W')
#define KEY_S (key == 's' || key == 'S')
#define KEY_A (key == 'a' || key == 'A')
#define KEY_D (key == 'd' || key == 'D')
#define KEY_Q (key == 'q' || key == 'Q')
#define KEY_E (key == 'e' || key == 'E')
#define KEY_R (key == 'r' || key == 'R')


int WIDTH = 1000;
int HEIGHT = 800;

int windowID;
int N;
int ***grid;	//0=red | 1=green | 2=blue | 3=yellow | 100=magenta | -1=NULL

int lives = 3;
int points = 50;
int stock = 0;

double xpos; // = round((N-1)/2.0);
double ypos; // = round((N-1)/2.0);
double zpos; // = 1;

int targetXpos; // = round((N-1)/2.0);
int targetYpos; // = round((N-1)/2.0);
int targetZpos; // = 1;

double angleRotation = 0;
int targetAngleRotation = angleRotation;

const int spawnZ = 6;
int remainingFalls = 0;
int fallPointsToRemove = 0;

bool gameOver = false;
bool runTimer = false;

double currentAvatarX;
double currentAvatarY;
double currentAvatarZ;
bool avatarSpotlightON = false;
bool thirdPersonCameraON = false;

// Avatar moving parts variables //
double RightLegCurrentAngle = 0.0;
double RightLegTargetAngle = 20;

double LeftLegCurrentAngle = 0.0;
double LeftLegTargetAngle = -20;

double LeftArmCurrentAngle = 0.0;
double LeftArmTargetAngle = 20.0;

double RightArmCurrentAngle = 0.0;
double RightArmTargetAngle = -20.0;
// ============================= //



void initialize() {
	srand(time(NULL));
	
    grid = new int**[N];
    for (int i=0; i<N; i++) {
        grid[i] = new int*[N];
        for (int j=0; j<N; j++){
			grid[i][j] = new int[N];
			for(int z=0; z<N; z++){
				if (z==0){
					if (!(i==round((N-1)/2.0) && j==round((N-1)/2.0))){
						grid[i][j][z] = rand() % 4;
					}else{
						grid[i][j][z] = 100;
					}
				}else{
					grid[i][j][z] = -1;
				}
			}
		}
    }
}

void setPointsLives(int pointsAlteration, int livesAlteration){
	if (pointsAlteration != 0){
		if (points + pointsAlteration >= 0){
			points = points + pointsAlteration;
		}else{
			points = 0;
			lives--;
		}
	}
	if (livesAlteration != 0){
		lives = lives + livesAlteration;
	}
	
	if ((lives==0 && points==0) || lives<0){
		points = 0;
		lives = 0;
		
		gameOver = true;
	}
}

void getLookingCoordinates(int *x, int *y){
	switch((int)angleRotation){
		case 0:
		case 360:
			*x = targetXpos;
			*y = targetYpos+1;
			break;
		case 90:
			*x = targetXpos+1;
			*y = targetYpos;
			break;
		case 180:
			*x = targetXpos;
			*y = targetYpos-1;
			break;
		case 270:
			*x = targetXpos-1;
			*y = targetYpos;
			break;
		default:
			*x = -1;
			*y = -1;
			break;
	}
}

int findLookingClosestFreeZ_UpDown(int i, int j){
	int minZ_pos = targetZpos;
	
	for(int z=targetZpos-1; z>=0; z--){
		if (grid[i][j][z] == -1){
			minZ_pos = z;
		}else{
			break;
		}
	}
	
	return minZ_pos;
}

int findLookingClosestFreeZ_DownUp(int i, int j){
	for (int z=targetZpos; z<N; z++){
		if (grid[i][j][z] == -1){
			return z;
		}
	}
	
	return -1;
}

void doMove(int newX, int newY){
	if (grid[newX][newY][targetZpos]==-1 && grid[newX][newY][targetZpos-1]!=-1){			//Den exei kuvo mprosta moy alla mprosta moy exei kyvo apo katw moy.
		targetXpos = newX;
		targetYpos = newY;
	}
	else if (grid[targetXpos][targetYpos][targetZpos+1]==-1 && grid[newX][newY][targetZpos]!=-1 && grid[newX][newY][targetZpos+1]==-1){	//Den exei kuvo apo panw moy & exei kuvo mprosta moy & o mprosta moy kuvos den exei kuvo panw tou.
		targetXpos = newX;
		targetYpos = newY;
		targetZpos = targetZpos + 1;
		
		int addPoints = +5;
		setPointsLives(addPoints, 0);
	}
	else if (grid[newX][newY][targetZpos]==-1 && grid[newX][newY][targetZpos-1]==-1){		//Den exei kuvo mprosta moy & mprosta moy den exw kuvo apo katw moy.
		if (targetZpos-1==0 || grid[newX][newY][targetZpos-2]!=-1){							  //Yparxei kuvos omws parakatw mou (ptwsh 1 epipedo)
			targetXpos = newX;
			targetYpos = newY;
			targetZpos = targetZpos - 1;
		}else{																				//Den uparxei kuvos parakatw mou (ptwsh >1 epipedou)
			int z_destination = findLookingClosestFreeZ_UpDown(newX, newY);
			
			fallPointsToRemove -= ((targetZpos-1)-z_destination)*5;
			
			targetXpos = newX;
			targetYpos = newY;
			targetZpos = z_destination;
		}
		
		if (targetZpos==0){
			targetZpos = -spawnZ;
		}
	}
}

void powerOnAvatarSpotlight(){
	int x;
	int y;

	getLookingCoordinates(&x, &y);

	if (x!=-1 && y!=-1){
		if (targetXpos > x){
			x = -1.0;
			y = 0.0;
		}else if (targetXpos < x){
			x = 1.0;
			y = 0.0;
		}else if (targetYpos > y){
			x = 0.0;
			y = -1.0;
		}else if (targetYpos < y){
			x = 0.0;
			y = 1.0;
		}
	}else{
		glDisable(GL_LIGHT4);
		return;
	}

	GLfloat spotlight_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat spotlight_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat spotlight_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat spotlight_position[] = { currentAvatarX, currentAvatarY, currentAvatarZ, 1.0};
	GLfloat spotlight_target[] = { x , y , 0.0 };

	glLightfv(GL_LIGHT4, GL_AMBIENT, spotlight_ambient);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, spotlight_diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, spotlight_specular);
     
	glLightfv(GL_LIGHT4, GL_POSITION, spotlight_position);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, spotlight_target);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 60.0);
	//glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 0.0);

	glEnable(GL_LIGHT4);
}

void drawAvatar(){
	glPushMatrix();
		glTranslatef(currentAvatarX, currentAvatarY, currentAvatarZ);
		glRotatef(-(double)angleRotation, 0.0, 0.0, 1.0);
		// Head
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(0.0f, 0.0f, 0.39f);
			glutSolidSphere(0.09f, 20, 20);
			glColor3f(0.0f, 0.0f, 0.0f);				
			glutWireSphere(0.09f, 20, 20);
		glPopMatrix();

		// Body
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(0.0f, 0.0f, 0.08f);
			glScalef(1.0f, 0.4f, 1.4f);
			glutSolidCube(0.3f);
			glColor3f(0.0f, 0.0f, 0.0f);				
			glutWireCube(0.3f);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(currentAvatarX, currentAvatarY, currentAvatarZ+0.2);
		glRotatef(-(double)angleRotation, 0.0, 0.0, 1.0);
		glRotatef(-180, 0.0f, 1.0f, 0.0f);
		glRotatef(RightArmCurrentAngle, 1.0f, 0.0f, 0.0f);

		// Right Arm
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(0.18f, 0.0f, 0.1175f);
			glScalef(0.2f, 0.2f, 1.4f);
			glutSolidCube(0.25f);
			glColor3f(0.0f, 0.0f, 0.0f);				
			glutWireCube(0.25f);
		glPopMatrix();

		// Right Hand
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(0.18f, 0.0f, 0.32f);
			glutSolidSphere(0.035f, 20, 20);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(currentAvatarX, currentAvatarY, currentAvatarZ+0.2);
		glRotatef(-(double)angleRotation, 0.0, 0.0, 1.0);
		glRotatef(-180, 0.0f, 1.0f, 0.0f);
		glRotatef(LeftArmCurrentAngle, 1.0f, 0.0f, 0.0f);

		// Left Arm
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(-0.18f, 0.0f, 0.1175f);
			glScalef(0.2f, 0.2f, 1.4f);
			glutSolidCube(0.25f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glutWireCube(0.25f);
		glPopMatrix();

		// Left Hand
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(-0.18f, 0.0f, 0.32f);
			glutSolidSphere(0.035f, 20, 20);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(currentAvatarX, currentAvatarY, currentAvatarZ);
		glRotatef(-(double)angleRotation, 0.0, 0.0, 1.0);
		glRotatef(RightLegCurrentAngle, 1.0f, 0.0f, 0.0f);

		// Right Leg
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(0.075f, 0.0f, -0.315f);
			glScalef(0.4f, 0.35f, 1.5f);
			glutSolidCube(0.245f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glutWireCube(0.245001f);
		glPopMatrix();

		// Right Foot
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(0.075f, 0.05f, -0.48f);
			glScalef(0.4f, 0.35f, 0.15f);
			glutSolidCube(0.245f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glutWireCube(0.245001f);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(currentAvatarX, currentAvatarY, currentAvatarZ);
		glRotatef(-(double)angleRotation, 0.0, 0.0, 1.0);
		glRotatef(LeftLegCurrentAngle, 1.0f, 0.0f, 0.0f);

		// Left Leg
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(-0.075f, 0.0f, -0.315f);
			glScalef(0.4f, 0.35f, 1.5f);
			glutSolidCube(0.245f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glutWireCube(0.245001f);
		glPopMatrix();

		// Left Foot
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			glTranslatef(-0.075f, 0.05f, -0.48f);
			glScalef(0.4f, 0.35f, 0.15f);
			glutSolidCube(0.245f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glutWireCube(0.245001f);
		glPopMatrix();
	glPopMatrix();

	if (avatarSpotlightON){
		powerOnAvatarSpotlight();
	}
}

void keyboardNormal(unsigned char key, int x, int y) {
	if (KEY_ESC){
		if (gameOver == false){
			points = 0;
			lives = 0;
			
			gameOver = true;
		}else{
			glutDestroyWindow(windowID);
		}
	}
	
	if (gameOver == false){
		if (KEY_SPACEBAR){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
				int x = targetXpos;
				int y = targetYpos;
				int z = targetZpos-1;

				switch(grid[x][y][z]){
					case 3:		// 3==yellow, apothema --> 1
						grid[x][y][z] = 2;	// to kanw 2==blue, apothema --> 0
						stock++;
						break;
					case 0:		// 0==red, apothema --> 2
						grid[x][y][z] = 3;	// to kanw 3==yellow, apothema --> 1
						stock++;
						break;
					case 1:		// 1==green, apothema --> 3
						grid[x][y][z] = 0;	// to kanw 0==red, apothema --> 2
						stock++;
						break;
				}
			}
		}
		if (KEY_W){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
				if (angleRotation == targetAngleRotation){
					switch(targetAngleRotation){
						case 0:
						case 360:
							if (targetYpos+1 <= N-1){
								doMove(targetXpos, targetYpos+1);
							}
							break;
						case 90:
							if (targetXpos+1 <= N-1){
								doMove(targetXpos+1, targetYpos);
							}
							break;
						case 180:
							if (targetYpos-1 >= 0){
								doMove(targetXpos, targetYpos-1);
							}
							break;
						case 270:
							if (targetXpos-1 >= 0){
								doMove(targetXpos-1, targetYpos);
							}
							break;
					}
				}
			}
		}
		if (KEY_S){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
				if (angleRotation == targetAngleRotation){
					switch(targetAngleRotation){
						case 0:
						case 360:
							if (targetYpos-1 >= 0){
								doMove(targetXpos, targetYpos-1);
							}
							break;
						case 90:
							if (targetXpos-1 >= 0){
								doMove(targetXpos-1, targetYpos);
							}
							break;
						case 180:
							if (targetYpos+1 <= N-1){
								doMove(targetXpos, targetYpos+1);
							}
							break;
						case 270:
							if (targetXpos+1 <= N-1){
								doMove(targetXpos+1, targetYpos);
							}
							break;
					}
				}
			}
		}
		if (KEY_D){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos){
				if (angleRotation == targetAngleRotation){
					if (targetAngleRotation == 360){
						targetAngleRotation = 0;
						angleRotation = 0;
					}
					targetAngleRotation += 90;
				}
			}
		}
		if (KEY_A){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos){
				if (angleRotation == targetAngleRotation){
					targetAngleRotation -= 90;
					if (targetAngleRotation < 0){
						angleRotation = 360;
						targetAngleRotation = 360 + targetAngleRotation;
					}
				}
			}
		}
		if (KEY_Q){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
				int x;
				int y;
				int z = targetZpos;
				
				getLookingCoordinates(&x, &y);
				
				if (x<0 || x>=N || y<0 || y>=N){
					return;
				}
				
				if (grid[x][y][z] != -1){
					grid[x][y][z] = -1;
				}
			}
		}
		if (KEY_E){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
				int x;
				int y;
				int z;
				
				getLookingCoordinates(&x, &y);
				
				if (x<0 || x>=N || y<0 || y>=N){
					return;
				}
				
				bool foundCube = false;
				
				for (z=0; z<N; z++){
					if (x==round((N-1)/2.0) && y==round((N-1)/2.0) && z==0){		//To magenta den mporei na katastrafei.
						continue;
					}
					if (grid[x][y][z] != -1){
						grid[x][y][z] = -1;
						foundCube = true;
					}
				}
				
				if (foundCube){
					int removePoints = -20;
					int addLife = +1;
					
					setPointsLives(removePoints, addLife);
				}
			}
		}
		if (KEY_R){
			if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
				remainingFalls = N;
			}
		}
	}
}

void keyboardSpecial(int key, int x, int y){
	switch(key){
		case GLUT_KEY_F1:
			currentAvatarX = targetXpos;
			currentAvatarY = targetYpos;
			currentAvatarZ = targetZpos;
			
			thirdPersonCameraON = !thirdPersonCameraON;

			if (thirdPersonCameraON){
				RightLegCurrentAngle = 0.0;
				RightLegTargetAngle = 20;

				LeftLegCurrentAngle = 0.0;
				LeftLegTargetAngle = -20;

				LeftArmCurrentAngle = 0.0;
				LeftArmTargetAngle = 20.0;

				RightArmCurrentAngle = 0.0;
				RightArmTargetAngle = -20.0;
			}

			if (avatarSpotlightON){
				avatarSpotlightON = false;
				glDisable(GL_LIGHT4);
			}
			break;
		case GLUT_KEY_F2:
			if (thirdPersonCameraON){
				if (!avatarSpotlightON){
					avatarSpotlightON = true;
				}else{
					avatarSpotlightON = false;
					glDisable(GL_LIGHT4);
				}
			}
			break;
	}
}

void mouseClickHandler(int button, int state, int x, int y){
    if ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON) && stock>0){
		if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
			int x;
			int y;
			int z;
			
			getLookingCoordinates(&x, &y);
			
			if (x<0 || x>=N || y<0 || y>=N){
				return;
			}
			
			z = findLookingClosestFreeZ_DownUp(x, y);
			
			if (z != -1){
				grid[x][y][z] = rand() % 4;
				stock--;
				
				int addPoints = +5;
				setPointsLives(addPoints, 0);
			}
		}
    }
    
    if ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON)){
		if (xpos==targetXpos && ypos==targetYpos && zpos==targetZpos && remainingFalls == 0){
			int x;
			int y;
			int z = targetZpos;
			
			getLookingCoordinates(&x, &y);
			
			if (x<0 || x>=N || y<0 || y>=N){
				return;
			}
			
			if (grid[x][y][z] != -1){
				int cubesToMove = 0;
				if (x>xpos){
					for (int i=x; i<N; i++){
						if (grid[i][y][z] != -1){
							cubesToMove++;
						}else{
							break;
						}
					}
					
					int ic = x+cubesToMove;
					if (ic==N){
						ic--;
					}
					for (int i=ic; i>x; i--){
						grid[i][y][z] = grid[i-1][y][z];
					}
					grid[x][y][z] = -1;
				}
				else if (x<xpos){
					for (int i=x; i>=0; i--){
						if (grid[i][y][z] != -1){
							cubesToMove++;
						}else{
							break;
						}
					}
					
					int ic = x-cubesToMove;
					if (ic<0){
						ic=0;
					}
					for (int i=ic; i<x; i++){
						grid[i][y][z] = grid[i+1][y][z];
					}
					grid[x][y][z] = -1;
				}
				else if (y>ypos){
					for (int j=y; j<N; j++){
						if (grid[x][j][z] != -1){
							cubesToMove++;
						}else{
							break;
						}
					}
					
					int jc = y+cubesToMove;
					if (jc==N){
						jc--;
					}
					for (int j=jc; j>y; j--){
						grid[x][j][z] = grid[x][j-1][z];
					}
					grid[x][y][z] = -1;
				}
				else if (y<ypos){
					for (int j=y; j>=0; j--){
						if (grid[x][j][z] != -1){
							cubesToMove++;
						}else{
							break;
						}
					}
					
					int jc = y-cubesToMove;
					if (jc<0){
						jc=0;
					}
					for (int j=jc; j<y; j++){
						grid[x][j][z] = grid[x][j+1][z];
					}
					grid[x][y][z] = -1;
				}
			}
		}
    }
}

void renderBitmapString2D(char *string, void *font, float x, float y) {
	glRasterPos2i(x, y);		// Gia na provalw 2D text.
	//glRasterPos3f(x, y, z);	// Gia na provalw 3D text.
	char *c;
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void renderTextOnScreen(char *text, float *color_RGB, int x, int y){
	float R = color_RGB[0];
	float G = color_RGB[1];
	float B = color_RGB[2];

	glDisable(GL_LIGHTING);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			glColor3f(R, G, B);
			renderBitmapString2D(text, GLUT_BITMAP_TIMES_ROMAN_24, x, y);
			glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
}

void setLights(){
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 0.18, 0.18, 0.18, 1.0 };
	GLfloat light_specular[] = { 0.18, 0.18, 0.18, 1.0 };
	GLfloat light_corner1[] = { 0.0f, 0.0f, N, 1.0f };
	GLfloat light_corner2[] = { 0.0f, N-1, N, 1.0f };
	GLfloat light_corner3[] = { N-1, 0.0f, N, 1.0f };
	GLfloat light_corner4[] = { N-1, N-1, N,  1.0f };

	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_corner1);
	
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light_corner2);
	
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light_corner3);
	
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT3, GL_POSITION, light_corner4);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
}

void initializeScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1);
	glViewport(0, 0, (GLsizei)WIDTH, (GLsizei)HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (GLfloat)WIDTH / (GLfloat)HEIGHT, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_LIGHTING);
}

void renderScene() {
	initializeScene();
	
	if (gameOver == false){
		char statsText[100];
		float statsTextColor[] = {1.0, 1.0, 1.0};
		sprintf(statsText, "Points: %d  |  Lives: %d  |  Stock: %d", points, lives, stock);
		renderTextOnScreen(statsText, statsTextColor, 40, HEIGHT-50);
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			if (thirdPersonCameraON == false){
				glRotatef(-60, 1.0, 0.0, 0.0);
				glRotatef((double)angleRotation, 0.0, 0.0, 1.0);
				glTranslatef(-(xpos), -(ypos), -(zpos+1));
			}else{
				int x = 0;
				int y = 1;
				int z = 2;

				double cameraPosition[] = {xpos-1.5, ypos-1.5, zpos+1};
				double cameraLookingCoordinates[] = {xpos,ypos,zpos};
				double cameraLookingAngle[] = {0,0,1};
				
				gluLookAt(cameraPosition[x], cameraPosition[y], cameraPosition[z],
							cameraLookingCoordinates[x], cameraLookingCoordinates[y], cameraLookingCoordinates[z], 
								cameraLookingAngle[x], cameraLookingAngle[y], cameraLookingAngle[z]);
			}
			setLights();
		
			int i, j, z;
			for (i=0; i<N; i++){
				for (j=0; j<N; j++){
					for (z=0; z<N; z++){
						if (grid[i][j][z]!=-1){
							glPushMatrix();
							
								glTranslatef(i, j, z);
								switch (grid[i][j][z]){
									case 0:
										glColor3f(1.0f, 0.0f, 0.0f);	//red
										break;
									case 1:
										glColor3f(0.0f, 1.0f, 0.0f);	//green
										break;
									case 2:
										glColor3f(0.0f, 0.0f, 1.0f);	//blue
										break;
									case 3:
										glColor3f(1.0f, 1.0f, 0.0f);	//yellow
										break;
									case 100:
										glColor3f(1.0f, 0.0f, 1.0f);	//magenta
										break;
								}
								glutSolidCube(1.0f);
								
								glColor3f(0.0f, 0.0f, 0.0f);
								glutWireCube(1.002f);
							
							glPopMatrix();
						}
					}
				}
			}

			if (thirdPersonCameraON){
				drawAvatar();
			}

		glPopMatrix();
	}else{
		if (lives > 0 && points > 0){
			char winText[50];
			float winTextColor[] = {0.0, 1.0, 0.0};
			sprintf(winText, "YOU HAVE WON!");
			renderTextOnScreen(winText, winTextColor, WIDTH/2.0-strlen(winText)*7, HEIGHT/2.0);
			
			char statisticsText[50];
			float statisticsTextColor[] = {1.0, 1.0, 1.0};
			sprintf(statisticsText, "GAME STATISTICS:");
			renderTextOnScreen(statisticsText, statisticsTextColor, 40, HEIGHT-50);

			char statsText[100];
			float statsTextColor[] = {1.0, 1.0, 1.0};
			sprintf(statsText, "Points: %d  |  Lives: %d  |  Stock: %d", points, lives, stock);
			renderTextOnScreen(statsText, statsTextColor, WIDTH-370, HEIGHT-50);
		}else{
			char gameOverText[50];
			float gameOverTextColor[] = {1.0, 1.0, 1.0};
			sprintf(gameOverText, "GAME OVER!");
			renderTextOnScreen(gameOverText, gameOverTextColor, WIDTH/2.0-strlen(gameOverText)*7, HEIGHT/2.0);
		}
		
		runTimer = false;
	}
	
    glutSwapBuffers();
}

void changeSize(int w, int h) {
	if (h == 0){
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	glutPostRedisplay();
}

void timerFunction(int value){
	if (runTimer){
		if (thirdPersonCameraON==false){
			if (targetZpos != zpos){
				if (zpos < targetZpos-0.25){
					zpos += 0.25;
				}
				else if(zpos > targetZpos+0.25){
					zpos -= 0.25;
				}
				else{
					zpos = targetZpos;
					setPointsLives(fallPointsToRemove, 0);
					fallPointsToRemove = 0;
				}
			}
			if (targetXpos != xpos){
				if (xpos < targetXpos-0.25){
					xpos += 0.25;
				}
				else if(xpos > targetXpos+0.25){
					xpos -= 0.25;
				}
				else{
					xpos = targetXpos;
				}
			}
			if (targetYpos != ypos){
				if (ypos < targetYpos-0.25){
					ypos += 0.25;
				}
				else if(ypos > targetYpos+0.25){
					ypos -= 0.25;
				}
				else{
					ypos = targetYpos;
				}
			}
		}else{
			if (zpos >= targetZpos && (xpos!=targetXpos || ypos!=targetYpos)){
				if (xpos < (double)targetXpos-0.05){
					xpos += 0.05;
				}
				else if (xpos > (double)targetXpos+0.05){
					xpos -= 0.05;
				}
				else if (ypos < (double)targetYpos-0.05){
					ypos += 0.05;
				}
				else if (ypos > (double)targetYpos+0.05){
					ypos -= 0.05;
				}
				else{
					xpos = targetXpos;
					ypos = targetYpos;
				}
				
				if (xpos!=targetXpos || ypos!=targetYpos){
					if (RightLegCurrentAngle < RightLegTargetAngle){
						RightLegCurrentAngle += 4;
					}
					else if(RightLegCurrentAngle > RightLegTargetAngle){
						RightLegCurrentAngle -= 4;
					}
					if (RightLegCurrentAngle == RightLegTargetAngle){
						RightLegTargetAngle *= -1;
					}

					if (LeftLegCurrentAngle < LeftLegTargetAngle){
						LeftLegCurrentAngle += 4;
					}
					else if(LeftLegCurrentAngle > LeftLegTargetAngle){
						LeftLegCurrentAngle -= 4;
					}
					if (LeftLegCurrentAngle == LeftLegTargetAngle){
						LeftLegTargetAngle *= -1;
					}

					if (LeftArmCurrentAngle < LeftArmTargetAngle){
						LeftArmCurrentAngle += 4;
					}
					else if (LeftArmCurrentAngle > LeftArmTargetAngle){
						LeftArmCurrentAngle -= 4;
					}
					if(LeftArmCurrentAngle == LeftArmTargetAngle){
						LeftArmTargetAngle *= -1;
					}

					if (RightArmCurrentAngle > RightArmTargetAngle){
						RightArmCurrentAngle -= 4;
					}
					else if (RightArmCurrentAngle < RightArmTargetAngle){
						RightArmCurrentAngle += 4;
					}
					if(RightArmCurrentAngle == RightArmTargetAngle){
						RightArmTargetAngle *= -1;
					}
				}else{
					RightLegCurrentAngle = 0;
					LeftLegCurrentAngle = 0;
					LeftArmCurrentAngle = 0;
					RightArmCurrentAngle = 0;
				}

				currentAvatarX = xpos;
				currentAvatarY = ypos;
				currentAvatarZ = zpos;
			}else{
				currentAvatarX = targetXpos;
				currentAvatarY = targetYpos;

				if (targetZpos != zpos){
					if (zpos < targetZpos-0.05){
						zpos += 0.05;
					}
					else if(zpos > targetZpos+0.05){
						zpos -= 0.05;
					}
					else{
						zpos = targetZpos;
					}
				}
				if (zpos > targetZpos){
					zpos -= 0.20;
					currentAvatarZ = zpos;
				}else{
					currentAvatarZ = targetZpos;
					setPointsLives(fallPointsToRemove, 0);
					fallPointsToRemove = 0;
				}

				if (targetXpos != xpos){
					if (xpos < targetXpos-0.05){
						xpos += 0.05;
					}
					else if(xpos > targetXpos+0.05){
						xpos -= 0.05;
					}
					else{
						xpos = targetXpos;
					}
				}
				if (targetYpos != ypos){
					if (ypos < targetYpos-0.05){
						ypos += 0.05;
					}
					else if(ypos > targetYpos+0.05){
						ypos -= 0.05;
					}
					else{
						ypos = targetYpos;
					}
				}
			}
		}

		if (zpos == -spawnZ){
			for (int z=1; z<=spawnZ; z++){
				if (z>=N){
					break;
				}
				grid[(int)round((N-1)/2.0)][(int)round((N-1)/2.0)][z] = -1;
			}

			targetXpos = round((N-1)/2.0);
			targetYpos = round((N-1)/2.0);
			targetZpos = 1;

			xpos = targetXpos;
			ypos = targetYpos;
			zpos = spawnZ;
			
			int removeLife = -1;
			setPointsLives(0, removeLife);
		}
		if (targetZpos == N-1 && zpos==targetZpos){
			int addLife = +1;
			int addPoints = +100;
			setPointsLives(addPoints, addLife);
			
			gameOver = true;
		}
		
		if (targetAngleRotation != angleRotation){
			if (targetAngleRotation < angleRotation){
				angleRotation -= 10;
			}
			else if (targetAngleRotation > angleRotation){
				angleRotation += 10;
			}
		}
		
		if (remainingFalls!=0){
			for (int x=0; x<N; x++){
				for (int y=0; y<N; y++){
					for (int z=1; z<N; z++){
						if (grid[x][y][z-1]==-1 && grid[x][y][z]!=-1 && !(targetXpos==x && targetYpos==y && targetZpos==z-1)){
							grid[x][y][z-1] = grid[x][y][z];
							grid[x][y][z] = -1;
							
							if (targetXpos==x && targetYpos==y && targetZpos-1==z && grid[x][y][z]==-1){
								targetZpos--;
								zpos--;
								if (thirdPersonCameraON){
									currentAvatarZ--;
								}
							}
							
							if (z==1){
								grid[x][y][z-1] = -1;
							}
						}
					}
				}
			}

			if (targetZpos==1 && grid[targetXpos][targetYpos][targetZpos-1]==-1){
				targetZpos = -spawnZ;
			}
			remainingFalls--;
		}
		
		glutTimerFunc(50, timerFunction, 0);
		
		glutPostRedisplay();
	}
}

int main(int argc, char **argv) {
	if (argc!=2 || (argc==2 && atoi(argv[1])<3)){
		printf("Prepei na dwseis ws orisma to N, opou N>=3! --> (%s N)\n", argv[0]);
	}else{
		N = atoi(argv[1]);
		targetXpos = round((N-1)/2.0);
		targetYpos = round((N-1)/2.0);
		targetZpos = 1;

		xpos = targetXpos;
		ypos = targetYpos;
		zpos = targetZpos;
		
		glutInit(&argc, argv);
		glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize (WIDTH, HEIGHT);
		windowID = glutCreateWindow("OpenGL Minecraft");
		glutDisplayFunc(renderScene);
		//glutIdleFunc(renderScene);			// This was replaced by glutTimerFunc (glutIdleFunc runs every ~10ms) which runs every 50ms.
		glutReshapeFunc(changeSize);
		glutKeyboardFunc(keyboardNormal);
		glutSpecialFunc(keyboardSpecial);
		glutMouseFunc(mouseClickHandler);
		
		initialize();
		
		glutTimerFunc(50, timerFunction, 0);
		runTimer = true;
		
		glutMainLoop();
	}

    return EXIT_SUCCESS;
}