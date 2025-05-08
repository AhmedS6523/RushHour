//============================================================================
// Name        : .cpp
// Author      : Dr. Sibt Ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game...
//============================================================================

#ifndef RushHour_CPP_
#define RushHour_CPP_
#include "util.h"
#include <iostream>
#include <string>
#include <cmath> 
#include <cstdlib>  
#include <ctime>    
using namespace std;



class Buildings {
	protected:

	    // 32 x 24 grid

		int buildings[32][24]; 
		
	public:
		
		bool isBuilding(int X, int Y) {
			if (X >= 0 && X < 32 && Y >= 0 && Y < 24) {
				return buildings[X][Y] == 1;
			}
			return false;
		}
		
		void BuildingGen() {
			
			for (int i = 0; i < 31; i++) {
				for (int j = 0; j < 22; j++) {
					buildings[i][j] = 0;
				}
			}
	
			const int maxB = 300;  
			int buildingCount = 0;
		
			for (int i = 2; i < 30 && buildingCount < maxB; ++i) {
				for (int j = 2; j < 21 && buildingCount < maxB; ++j) {
					
					int neighbors = 0;
					if (i > 0 && buildings[i-1][j]) neighbors++;
					if (j > 0 && buildings[i][j-1]) neighbors++;
					if (i > 0 && j > 0 && buildings[i-1][j-1]) neighbors++;
		
					
					int baseChance = rand() % 84;
					if (baseChance < 10 + 15 * neighbors) {
						buildings[i][j] = 1;  
						buildingCount++;
					}

				
				}
			}
            for (int i = 1; i < 31; ++i) {
                for (int j = 1; j < 22; ++j) {
                    // If current cell is not a building
                    if (buildings[i][j] == 0) {
                        bool right = buildings[i + 1][j] == 1;
                        bool up    = buildings[i][j + 1] == 1;
                        bool down  = buildings[i][j - 1] == 1;
                        bool left  = buildings[i-1][j] == 1;
            
                        if (right && up && down) {
                            buildings[i][j - 1] = 0; 
                        }
                        else if(right && up && left){
                            buildings[i][j+1] = 0;
                        }
                        
                    }
                    
                }
            }
		}
		

		void drawBuildings(int cSize) {
			for (int i = 0; i < 32; i++) {
				for (int j = 0; j < 24; j++) {
					if (buildings[i][j] == 1) {
						DrawSquare(i * cSize, j * cSize, cSize, colors[DARK_SLATE_GRAY]);
					}
				}
			}
		}
	};
	
	class Map : public Buildings {
        protected: 
            int width, height;    
            int cell;         
            
        public:
            Map(int w, int h, int cSize) {
                width = w;
                height = h;
                cell = cSize;
                srand(time(NULL));
        
                BuildingGen();  
            }
            
            
            void draw() {
                
                for(int i = 0; i < width * cell; i += cell) {
                    for(int j = 0; j < (height-2) * cell; j += cell) {
                        DrawSquare(i, j, cell, colors[WHITE]);
                    }
                }
        
                DrawRectangle(0, (height-2) * cell, width * cell, 2 * cell, colors[WHITE]);
                DrawRectangle(0, 0, width * cell, cell, colors[WHITE]);
                DrawRectangle(0, 0, cell, height * cell, colors[WHITE]);
                DrawSquare((width - 1) * cell, 0, height * cell, colors[WHITE]);
                drawBuildings(cell);
            }
            
            // Make sure this method is public so the Car class can use it
            bool canMoveTo(int pixelX, int pixelY) {
                int X = pixelX / cell; 
                int Y = pixelY / cell;
                return !isBuilding(X, Y);
            }
            
            int getCellSize() {
                return cell;
            }
        };
	
        class Car {
            private:
                int xPos, yPos;
                int size;
                int maxWidth, maxHeight; 
                int cellSize;  
                Map* gameMapPtr; 
                int fuel;
                const int maxFuel = 100;
                
            public:
                
                Car(int x, int y, int s, int maxW, int maxH, int cSize, Map* mapPtr) {
                    xPos = x;
                    yPos = y;
                    size = s;
                    maxWidth = maxW;
                    maxHeight = maxH;
                    cellSize = cSize;
                    gameMapPtr = mapPtr;
                    fuel = maxFuel;
                }
                
                void draw() {
                    float bodyHeight = size * 0.6;
                    float bodyLength = size * 1.4;
                    float bodyX = xPos - size * 0.2; 
                    float bodyY = yPos;
                
                    DrawRoundRect(bodyX, bodyY, bodyLength, bodyHeight, colors[RED]);
                    
                    float cabinWidth = size * 0.8;
                    float cabinHeight = size * 0.5;
                    float cabinX = xPos + size * 0.1;
                    float cabinY = yPos + bodyHeight;
                
                    DrawRectangle(cabinX, cabinY, cabinWidth, cabinHeight, colors[BLUE]);
                
                    DrawCircle(bodyX + size * 0.3, bodyY - size * 0.2, size * 0.2, colors[DARK_GRAY]); 
                    DrawCircle(bodyX + size * 1.0, bodyY - size * 0.2, size * 0.2, colors[DARK_GRAY]); 
                }
                
                void moveLeft() { 
                    int newX = xPos - (size+20);
                   

                    if (newX >= cellSize && gameMapPtr->canMoveTo(newX, yPos)) {
                        xPos = newX;
                        fuel = max(fuel - 1, 0);  // don't let it go below 0
                    }
                }
                
                void moveRight() { 
                    int newX = xPos + (size + 20);
                    if (newX + size < maxWidth - cellSize && gameMapPtr->canMoveTo(newX, yPos)) { 
                        xPos = newX;
                        fuel = max(fuel - 1, 0);  
                    }
                }
                
                void moveUp() { 
                    int newY = yPos + (size + 20);
                    if (newY + size < maxHeight - 2 * cellSize && gameMapPtr->canMoveTo(xPos, newY)) {
                        yPos = newY;
                        fuel = max(fuel - 1, 0);  
                    }
                }
                void moveDown() { 
                    int newY = yPos - (size + 20);
                    if (newY >= cellSize && gameMapPtr->canMoveTo(xPos, newY)) { 
                        yPos = newY;
                        fuel = max(fuel - 1, 0);  
                    }
                }
                
                int getX() { return xPos; }
                int getY() { return yPos; }
                int getFuel() { return fuel; }
            };

            Map* gameMapPtr;  
            Car* playerCarPtr;  

            void InitializeGame() {
        
                gameMapPtr = new Map(32, 24, 40);    
                playerCarPtr = new Car(50, 807, 20, 1280, 960, 40, gameMapPtr);
            }

// seed the random numbers generator by current time (see the documentation of srand for further help)...

/* Function sets canvas size (drawing area) in pixels...
 *  that is what dimensions (x and y) your game will have
 *  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
 * */
void SetCanvasSize(int width, int height) {
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

/*
 * Main Canvas drawing function.
 * */

void GameDisplay()/**/{
    // set the background color using function glClearColor.
    // to change the background play with the red, green and blue values below.
    // Note that r, g and b values must be in the range [0,1] where 0 means dim rid and 1 means pure red and so on.

    glClearColor(0/*Red Component*/, 0,    //148.0/255/*Green Component*/,
            0.0/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
    glClear (GL_COLOR_BUFFER_BIT); 
    
    gameMapPtr->draw();  
    playerCarPtr->draw();  

    // Calculate current fuel width
    int fuelWidth = (200 * playerCarPtr->getFuel()) / 100;


    DrawRectangle(1000, 910, 200, 20, colors[GRAY]);
    DrawRectangle(1000, 910, fuelWidth, 20, colors[RED]);
    DrawString(1000, 930, "Fuel", colors[BLACK]);
    DrawString( 40, 920, "Score=0", colors[BLACK]);
    
    glutSwapBuffers(); // do not modify this line..
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
 * is pressed from the keyboard
 *
 * You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
 *
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 *
 * */


 void NonPrintableKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
        
        playerCarPtr->moveLeft();  
    } 
    else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {
        playerCarPtr->moveRight();  
    } 
    else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
        playerCarPtr->moveUp();  
    } 
    else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
        playerCarPtr->moveDown();  
    }

    /* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
     * this function*/
    glutPostRedisplay();
}

/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
 * is pressed from the keyboard
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 * */
void PrintableKeys(unsigned char key, int x, int y) {
    if (key == 27/* Escape key ASCII*/) {
        exit(1); // exit the program when escape key is pressed.
    }

    
    glutPostRedisplay();
}

/*
 * This function is called after every 1000.0/FPS milliseconds
 * (FPS is defined on in the beginning).
 * You can use this function to animate objects and control the
 * speed of different moving objects by varying the constant FPS.
 *
 * */
void Timer(int m) {
    // implement your functionality here

    // once again we tell the library to call our Timer function after next 1000/FPS
    glutTimerFunc(100, Timer, 0);
}

/*This function is called (automatically) whenever your mouse moves witin inside the game window
 *
 * You will have to add the necessary code here for finding the direction of shooting
 *
 * This function has two arguments: x & y that tells the coordinate of current position of move mouse
 *
 * */
void MousePressedAndMoved(int x, int y) {
    cout << x << " " << y << endl;
    glutPostRedisplay();
}
void MouseMoved(int x, int y) {
    //cout << x << " " << y << endl;
    glutPostRedisplay();
}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
 *
 * You will have to add the necessary code here for shooting, etc.
 *
 * This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
 * x & y that tells the coordinate of current position of move mouse
 *
 * */
void MouseClicked(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) // dealing only with left button
    {
        cout << GLUT_DOWN << " " << GLUT_UP << endl;
    } 
    else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
    {
        cout<<"Right Button Pressed"<<endl;
    }
    glutPostRedisplay();
}

/*
 * our gateway main function
 * */
int main(int argc, char*argv[]) {
    int width = 1280, height = 960; // i have set my window size to be 800 x 600

    //Seed the random number generator at the start of the program
    srand(time(NULL));
    
    //InitRandomizer(); // seed the random number generator...
    glutInit(&argc, argv); // initialize the graphics library...
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
    glutInitWindowPosition(50, 50); // set the initial position of our window
    glutInitWindowSize(width, height); // set the size of our window
    glutCreateWindow("OOP Project"); // set the title of our game window
    SetCanvasSize(width, height); // set the number of pixels...

    InitializeGame();

    // Register your functions to the library,
    // you are telling the library names of function to call for different tasks.
    //glutDisplayFunc(display); // tell library which function to call for drawing Canvas.


    glutDisplayFunc(GameDisplay); // tell library which function to call for drawing Canvas.
    glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
    glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
    // This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
    glutTimerFunc(1000.0, Timer, 0);

    glutMouseFunc(MouseClicked);
    glutPassiveMotionFunc(MouseMoved); // Mouse
    glutMotionFunc(MousePressedAndMoved); // Mouse

    // now handle the control to library and it will call our registered functions when
    // it deems necessary...
    glutMainLoop();
    delete gameMapPtr;
    delete playerCarPtr;
    return 1;
}
#endif /* RushHour_CPP_ */