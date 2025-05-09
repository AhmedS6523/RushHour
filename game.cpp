//============================================================================
// Name        : RushHour.cpp
// Author      : Dr. Sibt Ul Hussain (Refactored)
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game with improved OOP design
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



// Buildings class (Base class for Map)
class Buildings {
protected:
    // 32 x 24 grid
    int buildings[32][24];

public:
    Buildings() {
        // Initialize buildings
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 24; j++) {
                buildings[i][j] = 0;
            }
        }
    }

    bool isBuilding(int X, int Y) {
        if (X >= 0 && X < 32 && Y >= 0 && Y < 24) {
            return buildings[X][Y] == 1;
        }
        return false;
    }

    void BuildingGen() {
        

        const int maxB = 300;
        int buildingCount = 0;

        for (int i = 2; i < 30 && buildingCount < maxB; ++i) {
            for (int j = 2; j < 21 && buildingCount < maxB; ++j) {
                int neighbors = 0;
                if (i > 0 && buildings[i - 1][j])
                    neighbors++;
                if (j > 0 && buildings[i][j - 1])
                    neighbors++;
                if (i > 0 && j > 0 && buildings[i - 1][j - 1])
                    neighbors++;

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
                    bool up = buildings[i][j + 1] == 1;
                    bool down = buildings[i][j - 1] == 1;
                    bool left = buildings[i - 1][j] == 1;

                    if (right && up && down) {
                        buildings[i][j - 1] = 0;
                    } else if (right && up && left) {
                        buildings[i][j + 1] = 0;
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

class FuelStation {
private:
    int gridX, gridY;
    int cellSize;
    
public:

    FuelStation() {
    cellSize = 0;
    gridX = -1;
    gridY = -1;
    }

    FuelStation(int cSize) {
        cellSize = cSize;
        gridX = -1;
        gridY = -1;
    }
    
    void setPosition(int x, int y) {
        gridX = x;
        gridY = y;
    }
    
    void draw() {
        if (gridX >= 0 && gridY >= 0) {
           
            DrawSquare(gridX * cellSize, gridY * cellSize, cellSize, colors[RED]);
        }
    }
    
    bool isAtStation(int carGridX, int carGridY) {
        return (carGridX == gridX && carGridY == gridY);
    }
    
    int getX() { return gridX; }
    int getY() { return gridY; }
};

class Map : public Buildings {
private:
    int width, height;
    int cell;
    FuelStation fuelStations[2];

public:
    Map(int w, int h, int cSize) {
        width = w;
        height = h;
        cell = cSize;
        BuildingGen();
        
        // Initialize fuel stations
        fuelStations[0] = FuelStation(cell);
        fuelStations[1] = FuelStation(cell);
        
        // Generate fuel station positions
        generateFuelStations();
    }
    
    void generateFuelStations() {
        for (int i = 0; i < 2; i++) {
            int maxAttempts = 50;
            int attempt = 0;
            bool validPosition = false;
            
            while (!validPosition && attempt < maxAttempts) {
                
                int gridX = rand() % 30 + 1;
                int gridY = rand() % 21 + 1;
                
                if (!isBuilding(gridX, gridY) && 
                    (i == 0 || (gridX != fuelStations[0].getX() || gridY != fuelStations[0].getY()))) {
                    fuelStations[i].setPosition(gridX, gridY);
                    validPosition = true;
                }
                attempt++;
            }
            
            
            if (!validPosition) {
                for (int x = 1; x < 30; x++) {
                    for (int y = 1; y < 22; y++) {
                        if (!isBuilding(x, y) && 
                            (i == 0 || (x != fuelStations[0].getX() || y != fuelStations[0].getY()))) {
                            fuelStations[i].setPosition(x, y);
                            return;
                        }
                    }
                }
            }
        }
    }

    void draw() {
        for (int i = 0; i < width * cell; i += cell) {
            for (int j = 0; j < (height - 2) * cell; j += cell) {
                DrawSquare(i, j, cell, colors[WHITE]);
            }
        }

        DrawRectangle(0, (height - 2) * cell, width * cell, 2 * cell, colors[WHITE]);
        DrawRectangle(0, 0, width * cell, cell, colors[WHITE]);
        DrawRectangle(0, 0, cell, height * cell, colors[WHITE]);
        DrawSquare((width - 1) * cell, 0, height * cell, colors[WHITE]);
        drawBuildings(cell);
        
        // Draw fuel stations
        fuelStations[0].draw();
        fuelStations[1].draw();
    }

    bool canMoveTo(int pixelX, int pixelY) {
        int X = pixelX / cell;
        int Y = pixelY / cell;
        return !isBuilding(X, Y);
    }
    
    bool isAtFuelStation(int carGridX, int carGridY) {
        return fuelStations[0].isAtStation(carGridX, carGridY) || 
               fuelStations[1].isAtStation(carGridX, carGridY);
    }

    int getCellSize() {
        return cell;
    }
    
    int getWidth() {
        return width;
    }
    
    int getHeight() {
        return height;
    }
};

// Car class
class Car {
private:
    int xPos, yPos;
    int size;
    int maxWidth, maxHeight;
    int cellSize;
    Map* gameMapPtr;
    int fuel;
    const int maxFuel = 100;
    bool hasPassenger;
    ColorNames carColor;

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
        hasPassenger = false;
        carColor = BLUE;
    }

    void draw() {
        float bodyHeight = size * 0.6;
        float bodyLength = size * 1.4;
        float bodyX = xPos - size * 0.2;
        float bodyY = yPos;

        DrawRoundRect(bodyX, bodyY, bodyLength, bodyHeight, colors[carColor]);

        float cabinWidth = size * 0.8;
        float cabinHeight = size * 0.5;
        float cabinX = xPos + size * 0.1;
        float cabinY = yPos + bodyHeight;

        DrawRectangle(cabinX, cabinY, cabinWidth, cabinHeight, colors[RED]);

        DrawCircle(bodyX + size * 0.3, bodyY - size * 0.2, size * 0.2, colors[DARK_GRAY]);
        DrawCircle(bodyX + size * 1.0, bodyY - size * 0.2, size * 0.2, colors[DARK_GRAY]);

        if (hasPassenger) {
            DrawSquare(cabinX + cabinWidth / 2 - size * 0.15, cabinY + cabinHeight, size * 0.3, colors[YELLOW]);
        }
    }

    void moveLeft() {
        if (fuel <= 0) return; // Can't move if out of fuel
        
        int newX = xPos - (size + 20);

        if (newX >= cellSize && gameMapPtr->canMoveTo(newX, yPos)) {
            xPos = newX;
            fuel = max(fuel - 1, 0);
        }
    }

    void moveRight() {
        if (fuel <= 0) return; // Can't move if out of fuel
        
        int newX = xPos + (size + 20);
        if (newX + size < maxWidth - cellSize && gameMapPtr->canMoveTo(newX, yPos)) {
            xPos = newX;
            fuel = max(fuel - 1, 0);
        }
    }

    void moveUp() {
        if (fuel <= 0) return; // Can't move if out of fuel
        
        int newY = yPos + (size + 20);
        if (newY + size < maxHeight - 2 * cellSize && gameMapPtr->canMoveTo(xPos, newY)) {
            yPos = newY;
            fuel = max(fuel - 1, 0);
        }
    }

    void moveDown() {
        if (fuel <= 0) return; // Can't move if out of fuel
        
        int newY = yPos - (size + 20);
        if (newY >= cellSize && gameMapPtr->canMoveTo(xPos, newY)) {
            yPos = newY;
            fuel = max(fuel - 1, 0);
        }
    }

    void pickupPassenger() {
        hasPassenger = true;
        carColor = YELLOW;
    }

    void dropoffPassenger() {
        hasPassenger = false;
        carColor = BLUE;
    }
    
    bool tryRefuel(int& money) {
        int gridX = xPos / cellSize;
        int gridY = yPos / cellSize;
        
        // Check if at fuel station and has enough money
        if (gameMapPtr->isAtFuelStation(gridX, gridY)) {
            if (money >= 50) {
                money -= 50;
                fuel = maxFuel;
                return true;
            }
        }
        return false;
    }

    int getX() { return xPos; }
    int getY() { return yPos; }
    int getFuel() { return fuel; }
    bool getHasPassenger() { return hasPassenger; }
    void refuel() { fuel = maxFuel; } // Keep this for compatibility
};

// NPCCar class
class NPCCar {
    private:
        int xPos, yPos;
        int size;
        int cellSize;
        Map* gameMapPtr;
        ColorNames carColor;
        int moveTimer;
        int moveDirection; // 0-3: Left, Right, Up, Down
        int maxWidth, maxHeight;  // Added to track grid boundaries
    
    public:
        NPCCar(Map* mapPtr, int cSize) {
            size = 15;
            cellSize = cSize;
            gameMapPtr = mapPtr;
            carColor = RED;
            moveTimer = 0;
            moveDirection = rand() % 4;
            // Set grid boundaries based on map size
            maxWidth = mapPtr->getWidth() * cSize;
            maxHeight = mapPtr->getHeight() * cSize;
            
            // Initialize position randomly at a valid location
            spawn();
        }
        
        void spawn() {
            int maxAttempts = 50;
            int attempt = 0;
            bool validPosition = false;
            
            while (!validPosition && attempt < maxAttempts) {
                int gridX = rand() % 30 + 1;
                int gridY = rand() % 21 + 1;
                
                xPos = gridX * cellSize;
                yPos = gridY * cellSize;
                
                if (gameMapPtr->canMoveTo(xPos, yPos)) {
                    validPosition = true;
                }
                attempt++;
            }
            
            if (!validPosition) {
                xPos = 3 * cellSize;
                yPos = 3 * cellSize;
                
                if (!gameMapPtr->canMoveTo(xPos, yPos)) {
                    for (int i = 1; i < 30; i++) {
                        for (int j = 1; j < 22; j++) {
                            if (gameMapPtr->canMoveTo(i * cellSize, j * cellSize)) {
                                xPos = i * cellSize;
                                yPos = j * cellSize;
                                return;
                            }
                        }
                    }
                }
            }
        }
        
        void draw() {
            float bodyHeight = size * 0.6;
            float bodyLength = size * 1.4;
            float bodyX = xPos - size * 0.2;
            float bodyY = yPos;
    
            DrawRoundRect(bodyX, bodyY, bodyLength, bodyHeight, colors[carColor]);
    
            float cabinWidth = size * 0.8;
            float cabinHeight = size * 0.5;
            float cabinX = xPos + size * 0.1;
            float cabinY = yPos + bodyHeight;
    
            DrawRectangle(cabinX, cabinY, cabinWidth, cabinHeight, colors[BLACK]);
    
            DrawCircle(bodyX + size * 0.3, bodyY - size * 0.2, size * 0.2, colors[DARK_GRAY]);
            DrawCircle(bodyX + size * 1.0, bodyY - size * 0.2, size * 0.2, colors[DARK_GRAY]);
        }
        
        void update() {
            // Update move timer
            moveTimer++;
            if (moveTimer >= 5) { // Move every 5 ticks
                moveTimer = 0;
                
                // Sometimes change direction
                if (rand() % 10 == 0) {
                    moveDirection = rand() % 4;
                }
                
                // Try to move in current direction
                bool moved = false;
                int originalDirection = moveDirection;
                
                do {
                    int newX = xPos;
                    int newY = yPos;
                    
                    switch (moveDirection) {
                        case 0: // Left
                            newX = xPos - cellSize;
                            break;
                        case 1: // Right
                            newX = xPos + cellSize;
                            break;
                        case 2: // Up
                            newY = yPos + cellSize;
                            break;
                        case 3: // Down
                            newY = yPos - cellSize;
                            break;
                    }
                    
                    // Check if the new position is within grid bounds and not on a building
                    if (newX >= cellSize && newX + size < maxWidth - cellSize && 
                        newY >= cellSize && newY + size < maxHeight - 2 * cellSize && 
                        gameMapPtr->canMoveTo(newX, newY)) {
                        xPos = newX;
                        yPos = newY;
                        moved = true;
                    } else {
                        // Try another direction
                        moveDirection = (moveDirection + 1) % 4;
                    }
                } while (!moved && moveDirection != originalDirection);
            }
        }
        
        bool collidesWith(int carX, int carY, int carSize) {
            // Simple rectangular collision
            int thisLeft = xPos - size * 0.2;
            int thisRight = xPos + size * 1.2;
            int thisBottom = yPos - size * 0.2;
            int thisTop = yPos + size * 1.1;
            
            int otherLeft = carX - carSize * 0.2;
            int otherRight = carX + carSize * 1.2;
            int otherBottom = carY - carSize * 0.2;
            int otherTop = carY + carSize * 1.1;
            
            return (thisRight > otherLeft && thisLeft < otherRight &&
                    thisTop > otherBottom && thisBottom < otherTop);
        }
        
        int getX() { return xPos; }
        int getY() { return yPos; }
    };

// Passenger class
class Passenger {
private:
    int xPos, yPos;
    int destX, destY;
    bool isVisible;
    bool hasDestination;
    int size;
    Map* gameMapPtr;
    int cellSize;

public:
    Passenger(Map* mapPtr, int cSize) {
        cellSize = cSize;
        gameMapPtr = mapPtr;
        size = cellSize / 3;
        isVisible = false;
        hasDestination = false;
    }

    void spawn() {
        int maxAttempts = 50;
        int attempt = 0;
        bool validPosition = false;

        while (!validPosition && attempt < maxAttempts) {
            // Generate random grid position
            int gridX = rand() % 30 + 1;
            int gridY = rand() % 21 + 1;

            // Convert to pixel coordinates - center in cell
            int x = gridX * cellSize;
            int y = gridY * cellSize;

            // Check if position is valid (not on a building)
            if (gameMapPtr->canMoveTo(x, y)) {
                xPos = x;
                yPos = y;
                validPosition = true;
                isVisible = true;
            }
            attempt++;
        }

        if (!validPosition) {
            xPos = 3 * cellSize; // 3rd cell from left
            yPos = 3 * cellSize; // 3rd cell from bottom

            // Make sure default position is valid
            if (gameMapPtr->canMoveTo(xPos, yPos)) {
                isVisible = true;
            } else {
                for (int i = 1; i < 30; i++) {
                    for (int j = 1; j < 22; j++) {
                        if (gameMapPtr->canMoveTo(i * cellSize, j * cellSize)) {
                            xPos = i * cellSize;
                            yPos = j * cellSize;
                            isVisible = true;
                            return;
                        }
                    }
                }
            }
        }
    }

    void draw() {
        if (hasDestination) {
            DrawSquare(destX, destY, cellSize, colors[PINK]);
        }

        if (isVisible) {
            float scale = 0.5;
            float s = size * scale;

            float centerX = xPos + cellSize / 2;
            float centerY = yPos + cellSize / 2;

            DrawCircle(centerX, centerY, s / 2, colors[BLACK]);
            DrawLine(centerX, centerY - s / 2, centerX, centerY - s / 2 - s, 2, colors[BLACK]);
            DrawLine(centerX, centerY - s / 2 - s * 0.5, centerX - s, centerY - s / 2 - s * 0.2, 2, colors[BLACK]);
            DrawLine(centerX, centerY - s / 2 - s * 0.5, centerX + s, centerY - s / 2 - s * 0.2, 2, colors[BLACK]);
            DrawLine(centerX, centerY - s / 2 - s, centerX - s, centerY - s / 2 - s * 2, 2, colors[BLACK]);
            DrawLine(centerX, centerY - s / 2 - s, centerX + s, centerY - s / 2 - s * 2, 2, colors[BLACK]);
        }
    }

    bool isPickedUp(int carX, int carY) {
        if (!isVisible)
            return false;

        int passengerGridX = xPos / cellSize;
        int passengerGridY = yPos / cellSize;
        int carGridX = carX / cellSize;
        int carGridY = carY / cellSize;

        return (passengerGridX == carGridX && passengerGridY == carGridY);
    }

    void pickup() {
        isVisible = false;
        generateDestination();
    }

    void generateDestination() {
        int maxAttempts = 50;
        int attempt = 0;
        bool validPosition = false;

        while (!validPosition && attempt < maxAttempts) {
            
            int gridX = rand() % 30 + 1;
            int gridY = rand() % 21 + 1;

            
            int x = gridX * cellSize;
            int y = gridY * cellSize;

            
            if (gameMapPtr->canMoveTo(x, y) && (x != xPos || y != yPos)) {
                destX = x;
                destY = y;
                validPosition = true;
                hasDestination = true;
            }
            attempt++;
        }
        if (!validPosition) {
           
            for (int i = 1; i < 30; i++) {
                for (int j = 1; j < 22; j++) {
                    int x = i * cellSize;
                    int y = j * cellSize;
                    if (gameMapPtr->canMoveTo(x, y) && (x != xPos || y != yPos)) {
                        destX = x;
                        destY = y;
                        hasDestination = true;
                        return;
                    }
                }
            }
        }
    }

    bool isAtDestination(int carX, int carY) {
        if (!hasDestination)
            return false;

        int destGridX = destX / cellSize;
        int destGridY = destY / cellSize;
        int carGridX = carX / cellSize;
        int carGridY = carY / cellSize;

        return (destGridX == carGridX && destGridY == carGridY);
    }

    void resetDestination() {
        hasDestination = false;
    }

    bool getHasDestination() { return hasDestination; }
    bool getIsVisible() { return isVisible; }
}; 


class GameManager {
private:
    Map* gameMap;
    Car* playerCar;
    static const int NUM_PASSENGERS = 2;
    Passenger* passengers[NUM_PASSENGERS];
    int activePassengerIndex;
    int score;
    int money;
    int width, height;
    bool refuelMessage;
    int refuelMessageTimer;
    string statusMessage;
    static const int NUM_NPC_CARS = 4;
    NPCCar* npcCars[NUM_NPC_CARS];
    bool gameOver;
    int moveTimerCounter; // Added to track time-based movement
    
public:
    GameManager(int windowWidth, int windowHeight) {
        width = windowWidth;
        height = windowHeight;
        score = 0;
        money = 0; 
        refuelMessage = false;
        refuelMessageTimer = 0;
        statusMessage = "";
        gameOver = false;
        moveTimerCounter = 0; // Initialize the counter
        
        // FIXED: First initialize the map
        gameMap = new Map(32, 24, 40);
        
        // FIXED: Then initialize NPCs with the valid map pointer
        for (int i = 0; i < NUM_NPC_CARS; i++) {
            npcCars[i] = new NPCCar(gameMap, 40);
        }
        
        // Initialize car and passengers
        playerCar = new Car(50, 847, 20, width, height, 40, gameMap);
        activePassengerIndex = -1; // No active passenger initially
        for (int i = 0; i < NUM_PASSENGERS; i++) {
            passengers[i] = new Passenger(gameMap, 40);
            passengers[i]->spawn();
        }
    } 
    ~GameManager() {
        
        delete gameMap;
        delete playerCar;
        for (int i = 0; i < NUM_PASSENGERS; i++) {
            delete passengers[i];
        }
        for (int i = 0; i < NUM_NPC_CARS; i++) {
            delete npcCars[i];
        }
    }
    
    void update() {
        // Check if we need to spawn a new passenger
        if (!playerCar->getHasPassenger()) {
            for (int i = 0; i < NUM_PASSENGERS; i++) {
                if (!passengers[i]->getIsVisible()) {
                    passengers[i]->spawn();
                }
            }
        }
        
        // Update timer for status message
        if (refuelMessageTimer > 0) {
            refuelMessageTimer--;
            if (refuelMessageTimer == 0) {
                statusMessage = "";
            }
        }
        
        // Updated to make NPC cars move based on time
        if (!gameOver) {
            // Always update all NPC cars every tick
            for (int i = 0; i < NUM_NPC_CARS; i++) {
                npcCars[i]->update();
                
                if (npcCars[i]->collidesWith(playerCar->getX(), playerCar->getY(), 20)) {
                    gameOver = true;
                    statusMessage = "GAME OVER! You crashed!";
                    refuelMessageTimer = 300; // Show for longer
                }
            }
        }
    }
    
    void draw() {
        // Clear screen
        glClearColor(0, 0, 0.0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw map
        gameMap->draw();
        
        // Draw passenger and car
        for (int i = 0; i < NUM_PASSENGERS; i++) {
            passengers[i]->draw();
        }
        playerCar->draw();
        for (int i = 0; i < NUM_NPC_CARS; i++) {
            npcCars[i]->draw();
        }
        
        // Draw game over message if needed
        if (gameOver) {
            DrawString(width/2 - 100, height/2, "GAME OVER!", colors[RED]);
            DrawString(width/2 - 120, height/2 - 30, "Press ESC to exit", colors[BLACK]);
        }
        
        // Draw UI elements
        drawUI();
        
        glutSwapBuffers();
    }
    
    void drawUI() {
        // Draw fuel bar
        int fuelWidth = (200 * playerCar->getFuel()) / 100;
        DrawRectangle(1000, 910, 200, 20, colors[GRAY]);
        DrawRectangle(1000, 910, fuelWidth, 20, colors[RED]);
        DrawString(1000, 930, "Fuel", colors[BLACK]);
        
        // Draw score and money
        string scoreStr = "Score=" + to_string(score);
        DrawString(40, 920, scoreStr, colors[BLACK]);
        
        string moneyStr = "Money=$" + to_string(money);
        DrawString(40, 890, moneyStr, colors[BLACK]);
        
        // Draw status message if there is one
        if (statusMessage != "") {
            DrawString(width / 2 - 100, height - 50, statusMessage, colors[BLACK]);
        }
       
    }
    
    void handleSpecialKey(int key) {
        if (gameOver) return; // Don't process inputs if game is over
        
        switch(key) {
            case GLUT_KEY_LEFT:
                playerCar->moveLeft();
                break;
            case GLUT_KEY_RIGHT:
                playerCar->moveRight();
                break;
            case GLUT_KEY_UP:
                playerCar->moveUp();
                break;
            case GLUT_KEY_DOWN:
                playerCar->moveDown();
                break;
        }
    }
    
    
    void handleKey(unsigned char key) {
        if (key == 27) { // Escape key
            exit(1);
        } else if (key == 'g' || key == 'G') {
            if (!playerCar->getHasPassenger()) {
                // Try to pick up any passenger at current location
                for (int i = 0; i < NUM_PASSENGERS; i++) {
                    if (passengers[i]->isPickedUp(playerCar->getX(), playerCar->getY())) {
                        playerCar->pickupPassenger();
                        passengers[i]->pickup();
                        activePassengerIndex = i;
                        statusMessage = "Passenger picked up!";
                        refuelMessageTimer = 30;
                        break;
                    }
                }
            } else if (playerCar->getHasPassenger() && activePassengerIndex >= 0) {
                // Try to drop off active passenger
                if (passengers[activePassengerIndex]->isAtDestination(playerCar->getX(), playerCar->getY())) {
                    playerCar->dropoffPassenger();
                    passengers[activePassengerIndex]->resetDestination();
                    activePassengerIndex = -1;
                    score += 10;
                    int payment = GetRandInRange(25, 65);
                    money += payment;
                    statusMessage = "Fare completed! +$" + to_string(payment);
                    refuelMessageTimer = 30;
                }
            }
        }
    }
    
    void handleMouseClick(int button, int state, int x, int y) {
        // Handle mouse clicks (not implemented in this example)
    }
};

// Global pointer to the game manager
GameManager* gameManagerPtr = nullptr;

// GLUT callback functions
void GameDisplay() {
    if (gameManagerPtr) {
        gameManagerPtr->draw();
    }
}

void NonPrintableKeys(int key, int x, int y) {
    if (gameManagerPtr) {
        gameManagerPtr->handleSpecialKey(key);
    }
    glutPostRedisplay();
}

void PrintableKeys(unsigned char key, int x, int y) {
    if (gameManagerPtr) {
        gameManagerPtr->handleKey(key);
    }
    glutPostRedisplay();
}

void Timer(int m) {
    if (gameManagerPtr) {
        gameManagerPtr->update();
    }
    glutTimerFunc(100, Timer, 0);
}

void MouseClicked(int button, int state, int x, int y) {
    if (gameManagerPtr) {
        gameManagerPtr->handleMouseClick(button, state, x, y);
    }
    glutPostRedisplay();
}

void SetCanvasSize(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MousePressedAndMoved(int x, int y) {
    glutPostRedisplay();
}

void MouseMoved(int x, int y) {
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {

    int width = 1280, height = 960;
    
    srand(time(NULL));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(width, height);
    glutCreateWindow("Rush Hour");
    SetCanvasSize(width, height);
    
    gameManagerPtr = new GameManager(width, height);
   
    glutDisplayFunc(GameDisplay);
    glutSpecialFunc(NonPrintableKeys);
    glutKeyboardFunc(PrintableKeys);
    glutTimerFunc(100, Timer, 0);
    glutMouseFunc(MouseClicked);
    glutPassiveMotionFunc(MouseMoved);
    glutMotionFunc(MousePressedAndMoved);
    
    // Enter main loop
    glutMainLoop();
    
    // Clean up
    delete gameManagerPtr;
    
    return 0;
}
#endif /* RushHour_CPP_ */