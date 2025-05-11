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
#include <fstream>

using namespace std;

int MENU_STATE = 0;
const int TAXI_MODE = 1;
const int DELIVERY_MODE = 2;
const int GAME_OVER_STATE = 3;
const int LEADERBOARD_STATE = 4;

class Menu {
    private:
        int selectedOption;
        int windowWidth;
        int windowHeight;
        float animTimer;
        
    public:
        Menu() : selectedOption(0), windowWidth(1280), windowHeight(960), animTimer(0) {}
        
        void draw(int width, int height) {
            windowWidth = width;
            windowHeight = height;
            animTimer += 0.1; // Animation timer
            
            // Draw background
            DrawRectangle(0, 0, width, height, colors[SLATE_GRAY]);
            
            // Create a city skyline effect
            for (int i = 0; i < 10; i++) {
                int buildingWidth = width / 10;
                int buildingHeight = 80 + (i % 4) * 30 + sin(animTimer + i) * 10;
                DrawRectangle(i * buildingWidth, 100, buildingWidth - 10, buildingHeight, colors[DARK_GRAY]);
                
                // Windows on buildings
                for (int w = 0; w < 3; w++) {
                    for (int h = 0; h < 5; h++) {
                        DrawSquare(i * buildingWidth + 10 + w * 20, 120 + h * 30, 10, 
                                  ((w + h + i) % 3 == 0) ? colors[YELLOW] : colors[BLACK]);
                    }
                }
            }
            
            // Road
            DrawRectangle(0, 20, width, 60, colors[BLACK]);
            
            // Road lines
            for (int i = 0; i < width; i += 80) {
                DrawRectangle(i, 45, 40, 10, colors[YELLOW]);
            }
            
            // Moving car
            int carPosX = 580;
            float carSize = 40;
            DrawRoundRect(carPosX, 30, carSize * 1.4, carSize * 0.6, colors[RED]);
            DrawRectangle(carPosX + carSize * 0.1, 30 + carSize * 0.6, carSize * 0.8, carSize * 0.5, colors[DARK_RED]);
            DrawCircle(carPosX + carSize * 0.3, 30 - carSize * 0.2, carSize * 0.2, colors[DARK_GRAY]);
            DrawCircle(carPosX + carSize * 1.0, 30 - carSize * 0.2, carSize * 0.2, colors[DARK_GRAY]);
        
            DrawString(width / 2 - 110, height - 200, "RUSH HOUR", colors[RED]);
            
            // Option box with gradient effect
            DrawRoundRect(width / 2 - 200, height - 505, 330, 250, colors[DARK_BLUE]);
            DrawRoundRect(width / 2 - 195, height - 500, 320, 240, colors[BLUE]);
            
            // Draw options with hover effect
            const int optionCount = 3; // Total number of options
            const char* options[] = {"Taxi Rush", "Delivery Rush", "Leaderboard"};
            const int yPositions[] = {height - 320, height - 370, height - 420};
            
            for (int i = 0; i < optionCount; i++) {
                if (selectedOption == i) {
                    DrawRoundRect(width / 2 - 180, yPositions[i] - 10, 285, 40, colors[YELLOW]);
                }
                DrawString(width / 2 - 160, yPositions[i], options[i], selectedOption == i ? colors[RED] : colors[WHITE]);
            }
        }
        
        void moveSelection(bool up) {
            if (up && selectedOption > 0) {
                selectedOption--;
            } else if (!up && selectedOption < 2) {
                selectedOption++;
            }
        }
        
        int getSelection() {
            return selectedOption;
        }

        void handleMouseClick(int x, int y, int height) {
            // Calculate positions relative to menu
            const int yPositions[] = {windowHeight - 320, windowHeight - 370, windowHeight - 420};
            
            for (int i = 0; i < 3; i++) {
                if (y >= yPositions[i] - 20 && y <= yPositions[i] + 20 && 
                    x >= windowWidth / 2 - 180 && x <= windowWidth / 2 + 180) {
                    selectedOption = i;
                    MENU_STATE = i + 1; // Set to corresponding mode
                    if (i == 2) MENU_STATE = LEADERBOARD_STATE; // Override for leaderboard
                    break;
                }
            }
        }
};
    
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

        // Generate initial buildings
        for (int i = 2; i < 30 && buildingCount < maxB; ++i) {
            for (int j = 2; j < 21 && buildingCount < maxB; ++j) {
                int neighbors = 0;
                if (i > 0 && buildings[i - 1][j]) neighbors++;
                if (j > 0 && buildings[i][j - 1]) neighbors++;
                if (i > 0 && j > 0 && buildings[i - 1][j - 1]) neighbors++;

                int baseChance = rand() % 84;
                if (baseChance < 10 + 15 * neighbors) {
                    buildings[i][j] = 1;
                    buildingCount++;
                }
            }
        }
        
        // Clear paths where needed
        for (int i = 1; i < 31; ++i) {
            for (int j = 1; j < 22; ++j) {
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
    FuelStation() : cellSize(0), gridX(-1), gridY(-1) {}
    
    explicit FuelStation(int cSize) : cellSize(cSize), gridX(-1), gridY(-1) {}
    
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
    
    int getX() const { return gridX; }
    int getY() const { return gridY; }
};

class Garage {
private:
    int gridX, gridY;
    int cellSize;
    
public:
    explicit Garage(int cSize) : cellSize(cSize), gridX(30), gridY(1) {}
    
    void draw() {
        int x = gridX * cellSize;
        int y = gridY * cellSize;
        
        // Draw garage base
        DrawSquare(x, y, cellSize, colors[DARK_BLUE]);
        
        // Draw garage door
        DrawRectangle(x + cellSize * 0.15, y + cellSize * 0.1, cellSize * 0.7, cellSize * 0.6, colors[GRAY]);
        
        // Draw roof (triangle)
        DrawTriangle(x, y + cellSize, 
                    x + cellSize, y + cellSize, 
                    x + cellSize/2, y + cellSize * 1.5, 
                    colors[RED]);
        
        // Draw 'P' letter above
        DrawString(x + cellSize * 0.4, y + cellSize * 0.8, "P", colors[WHITE]);
    }
    
    bool isAtGarage(int carGridX, int carGridY) {
        return (carGridX == gridX && carGridY == gridY);
    }
    
    int getX() const { return gridX; }
    int getY() const { return gridY; }
};

class Map : public Buildings {
private:
    int width, height;
    int cell;
    FuelStation fuelStations[2];

public:
    Map(int w, int h, int cSize) : width(w), height(h), cell(cSize) {
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
            
            // Fallback if no valid position found
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
        // Draw background grid
        for (int i = 0; i < width * cell; i += cell) {
            for (int j = 0; j < (height - 2) * cell; j += cell) {
                DrawSquare(i, j, cell, colors[WHITE]);
            }
        }

        // Draw borders
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

    int getCellSize() const { return cell; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
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
        bool hasPassenger;
        ColorNames carColor;
    
    public:
        Car(int x, int y, int s, int maxW, int maxH, int cSize, Map* mapPtr) 
            : xPos(x), yPos(y), size(s), maxWidth(maxW), maxHeight(maxH), 
              cellSize(cSize), gameMapPtr(mapPtr), fuel(maxFuel),
              hasPassenger(false), carColor(BLUE) {}
    
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
    
        // Combined movement function to reduce code duplication
        void move(int dirX, int dirY) {
            if (fuel <= 0) return; // Can't move if out of fuel
            
            int moveDistance = size + 20;
            int newX = xPos + dirX * moveDistance;
            int newY = yPos + dirY * moveDistance;
            
            // Check boundaries based on movement direction
            bool validMove = false;
            if (dirX < 0) {  // Moving left
                validMove = newX >= cellSize && gameMapPtr->canMoveTo(newX, yPos);
            } else if (dirX > 0) {  // Moving right
                validMove = newX + size < maxWidth - cellSize && gameMapPtr->canMoveTo(newX, yPos);
            } else if (dirY > 0) {  // Moving up
                validMove = newY + size < maxHeight - 2 * cellSize && gameMapPtr->canMoveTo(xPos, newY);
            } else if (dirY < 0) {  // Moving down
                validMove = newY >= cellSize && gameMapPtr->canMoveTo(xPos, newY);
            }
            
            if (validMove) {
                xPos = newX;
                yPos = newY;
                fuel = max(fuel - 1, 0);
            }
        }
    
        void moveLeft() {
            move(-1, 0);
        }
    
        void moveRight() {
            move(1, 0);
        }
    
        void moveUp() {
            move(0, 1);
        }
    
        void moveDown() {
            move(0, -1);
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
            if (gameMapPtr->isAtFuelStation(gridX, gridY) && money >= 50) {
                money -= 50;
                fuel = maxFuel;
                return true;
            }
            return false;
        }
    
        int getX() const { return xPos; }
        int getY() const { return yPos; }
        int getFuel() const { return fuel; }
        bool getHasPassenger() const { return hasPassenger; }
    };
    
    class Box {
    private:
        int xPos, yPos;
        int destX, destY;
        bool isVisible;
        bool hasDestination;
        int size;
        Map* gameMapPtr;
        int cellSize;
    
    public:
        Box(Map* mapPtr, int cSize) 
            : gameMapPtr(mapPtr), cellSize(cSize), size(cSize / 3), 
              isVisible(false), hasDestination(false), 
              xPos(0), yPos(0), destX(0), destY(0) {}
    
        // Helper function to find valid position on map
        bool findValidPosition(int& x, int& y, int origX = -1, int origY = -1) {
            int maxAttempts = 50;
            
            // First try random positions
            for (int attempt = 0; attempt < maxAttempts; attempt++) {
                int gridX = rand() % 30 + 1;
                int gridY = rand() % 21 + 1;
                
                int testX = gridX * cellSize;
                int testY = gridY * cellSize;
                
                if (gameMapPtr->canMoveTo(testX, testY) && 
                    (origX == -1 || testX != origX || testY != origY)) {
                    x = testX;
                    y = testY;
                    return true;
                }
            }
            
            // Fallback: search systematically
            for (int i = 1; i < 30; i++) {
                for (int j = 1; j < 22; j++) {
                    int testX = i * cellSize;
                    int testY = j * cellSize;
                    
                    if (gameMapPtr->canMoveTo(testX, testY) && 
                        (origX == -1 || testX != origX || testY != origY)) {
                        x = testX;
                        y = testY;
                        return true;
                    }
                }
            }
            
            return false;
        }
    
        void spawn() {
            if (findValidPosition(xPos, yPos)) {
                isVisible = true;
            }
        }
    
        void draw() {
            if (hasDestination) {
                DrawSquare(destX, destY, cellSize, colors[BROWN]);
            }
    
            if (isVisible) {
                // Draw a package/box
                float centerX = xPos + cellSize / 2;
                float centerY = yPos + cellSize / 2;
                float boxSize = size * 0.8;
                
                DrawSquare(centerX - boxSize/2, centerY - boxSize/2, boxSize, colors[BROWN]);
                // Add "tape" lines on the box
                DrawLine(centerX - boxSize/2, centerY, centerX + boxSize/2, centerY, 2, colors[YELLOW]);
                DrawLine(centerX, centerY - boxSize/2, centerX, centerY + boxSize/2, 2, colors[YELLOW]);
            }
        }
    
        bool isPickedUp(int carX, int carY) {
            if (!isVisible) return false;
    
            int boxGridX = xPos / cellSize;
            int boxGridY = yPos / cellSize;
            int carGridX = carX / cellSize;
            int carGridY = carY / cellSize;
    
            return (boxGridX == carGridX && boxGridY == carGridY);
        }
    
        void pickup() {
            isVisible = false;
            generateDestination();
        }
    
        void generateDestination() {
            if (findValidPosition(destX, destY, xPos, yPos)) {
                hasDestination = true;
            }
        }
    
        bool isAtDestination(int carX, int carY) {
            if (!hasDestination) return false;
    
            int destGridX = destX / cellSize;
            int destGridY = destY / cellSize;
            int carGridX = carX / cellSize;
            int carGridY = carY / cellSize;
    
            return (destGridX == carGridX && destGridY == carGridY);
        }
    
        void resetDestination() {
            hasDestination = false;
        }
    
        bool getHasDestination() const { return hasDestination; }
        bool getIsVisible() const { return isVisible; }
    };
    
    class NPCCar {
    private:
        int xPos, yPos;
        int size;
        int cellSize;
        Map* gameMapPtr;
        ColorNames carColor;
        int moveDirection; // 0-3: Left, Right, Up, Down
        int maxWidth, maxHeight;  // Track grid boundaries
    
    public:
        NPCCar(Map* mapPtr, int cSize) 
            : size(15), cellSize(cSize), gameMapPtr(mapPtr), carColor(RED),
              moveDirection(rand() % 4) {
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
                
                // Center in cell
                xPos = gridX * cellSize + (cellSize - size) / 2;
                yPos = gridY * cellSize + (cellSize - size) / 2;
                
                if (gameMapPtr->canMoveTo(xPos, yPos)) {
                    validPosition = true;
                }
                attempt++;
            }
            
            if (!validPosition) {
                // Default position centered in cell (3,3)
                xPos = 3 * cellSize + cellSize / 2 - size / 2;
                yPos = 3 * cellSize + cellSize / 2 - size / 2;
                
                if (!gameMapPtr->canMoveTo(xPos, yPos)) {
                    for (int i = 1; i < 30; i++) {
                        for (int j = 1; j < 22; j++) {
                            if (gameMapPtr->canMoveTo(i * cellSize, j * cellSize)) {
                                // Center in this found valid cell
                                xPos = i * cellSize + cellSize / 2 - size / 2;
                                yPos = j * cellSize + cellSize / 2 - size / 2;
                                return;
                            }
                        }
                    }
                }
            }
        }
        
        void draw() {
            // Center of the cell
            float centerX = (xPos / cellSize) * cellSize + cellSize / 2;
            float centerY = (yPos / cellSize) * cellSize + cellSize / 3;
        
            // Car body
            float bodyLength = size;
            float bodyHeight = size * 0.6;
            DrawRoundRect(centerX - bodyLength / 2, centerY - bodyHeight / 2, bodyLength, bodyHeight, colors[RED]);
        
            // Cabin
            float cabinWidth = size * 0.8;
            float cabinHeight = size * 0.5;
            DrawRectangle(centerX - cabinWidth / 2, centerY + bodyHeight / 2, cabinWidth, cabinHeight, colors[BLACK]);
        
            // Wheels
            float wheelRadius = size * 0.2;
            DrawCircle(centerX - size * 0.3, centerY - bodyHeight / 2 - wheelRadius, wheelRadius, colors[DARK_GRAY]);
            DrawCircle(centerX + size * 0.3, centerY - bodyHeight / 2 - wheelRadius, wheelRadius, colors[DARK_GRAY]);
        }
        
        void moveOnTick() {
            // Sometimes change direction
            if (rand() % 10 == 0) {
                moveDirection = rand() % 4;
            }
            
            // Try to move in current direction
            bool moved = false;
            int originalDirection = moveDirection;
            
            // Directions: 0=Left, 1=Right, 2=Up, 3=Down
            const int dirX[] = {-1, 1, 0, 0};
            const int dirY[] = {0, 0, 1, -1};
            
            do {
                int newX = xPos + dirX[moveDirection] * cellSize;
                int newY = yPos + dirY[moveDirection] * cellSize;
                
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
        
        bool collidesWith(int carX, int carY, int carSize) {
            // Collision detection
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
        
        int getX() const { return xPos; }
        int getY() const { return yPos; }
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
    
                // Convert to pixel coordinates
                int x = gridX * cellSize;
                int y = gridY * cellSize;
    
                // Check if position is valid
                if (gameMapPtr->canMoveTo(x, y)) {
                    xPos = x;
                    yPos = y;
                    validPosition = true;
                    isVisible = true;
                    return;
                }
                attempt++;
            }
    
            // Fallback position logic
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
                    return;
                }
                attempt++;
            }
            
            // Fallback logic
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
    
        bool getHasDestination() const { return hasDestination; }
        bool getIsVisible() const { return isVisible; }
    };
    
    class HighScore {
    public:
        char playerName[20];  // Fixed-size array for player name
        int score;
        int gameMode;         // 1 for TAXI_MODE, 2 for DELIVERY_MODE
    
        HighScore() : score(0), gameMode(0) {
            playerName[0] = '\0';  // Initialize to empty string
        }
    
        HighScore(const string& name, int s, int mode) : score(s), gameMode(mode) {
            strncpy(playerName, name.c_str(), sizeof(playerName) - 1);
            playerName[sizeof(playerName) - 1] = '\0'; // Ensure null termination
        }
    };
    
    class Leaderboard {
    private:
        static const int MAX_SCORES = 10;
        HighScore scores[MAX_SCORES];
        int numScores;
        const string filename = "highscores.txt";
    
    public:
        Leaderboard() : numScores(0) {
            loadFromFile();
        }
        
        void loadFromFile() {
            ifstream file(filename, ios::binary);
            if (!file) {
                cout << "No existing highscores file found. Creating new leaderboard." << endl;
                numScores = 0;
                return;
            }
            
            // Get file size
            file.seekg(0, ios::end);
            streampos fileSize = file.tellg();
            file.seekg(0, ios::beg);
            
            // Calculate number of records
            numScores = fileSize / sizeof(HighScore);
            if (numScores > MAX_SCORES) {
                numScores = MAX_SCORES;
            }
            
            // Read records
            file.read(reinterpret_cast<char*>(scores), numScores * sizeof(HighScore));
            file.close();
        }
        
        void saveToFile() {
            ofstream file(filename, ios::binary | ios::trunc);
            if (!file) {
                cout << "Error: Could not open highscores file for writing." << endl;
                return;
            }
            
            file.write(reinterpret_cast<const char*>(scores), numScores * sizeof(HighScore));
            file.close();
        }
        
        bool isHighScore(int score) const {
            // If we have fewer than MAX_SCORES, any score can be added
            if (numScores < MAX_SCORES) {
                return true;
            }
            
            // Otherwise, check if this score is higher than any existing score
            for (int i = 0; i < numScores; i++) {
                if (score > scores[i].score) {
                    return true;
                }
            }
            
            return false;
        }
        
        void addScore(const string& playerName, int score, int gameMode) {
            if (!isHighScore(score)) {
                return;
            }
            
            // Create new high score
            HighScore newScore(playerName, score, gameMode);
            
            // If we're not at capacity yet, just add the new score
            if (numScores < MAX_SCORES) {
                scores[numScores++] = newScore;
            } else {
                // Find the lowest score and replace it
                int lowestIdx = 0;
                for (int i = 1; i < numScores; i++) {
                    if (scores[i].score < scores[lowestIdx].score) {
                        lowestIdx = i;
                    }
                }
                
                scores[lowestIdx] = newScore;
            }
            
            // Sort scores in descending order
            sortScores();
            
            // Save to file
            saveToFile();
        }
        
        void sortScores() {
            // Sort scores in descending order
            for (int i = 0; i < numScores - 1; i++) {
                for (int j = 0; j < numScores - i - 1; j++) {
                    if (scores[j].score < scores[j + 1].score) {
                        // Swap
                        HighScore temp = scores[j];
                        scores[j] = scores[j + 1];
                        scores[j + 1] = temp;
                    }
                }
            }
        }
        
        void draw(int width, int height) {
            // Draw leaderboard background
            DrawRectangle(0, 0, width, height, colors[SLATE_GRAY]);
    
            // Draw title
            DrawString(width / 2 - 150, height - 100, "RUSH HOUR LEADERBOARD", colors[YELLOW]);
            
            // Draw column headers
            DrawString(width / 4 - 150, height - 200, "RANK", colors[WHITE]);
            DrawString(width / 4 - 50, height - 200, "PLAYER", colors[WHITE]);
            DrawString(width / 4 + 800, height - 200, "SCORE", colors[WHITE]);
            
            // Display scores with alternating row colors
            for (int i = 0; i < numScores && i < MAX_SCORES; i++) {
                // Alternating row colors
                if (i % 2 == 0) {
                    DrawRectangle(width / 4 - 160, height - 240 - i * 40, 
                                  width / 2 + 100, 35, colors[DARK_SLATE_GRAY]);
                }
                
                // Rank
                DrawString(width / 4 - 150, height - 240 - i * 40, 
                           to_string(i + 1), colors[WHITE]);
                
                // Player name
                DrawString(width / 4 - 50, height - 240 - i * 40, 
                           scores[i].playerName, colors[WHITE]);
                
                // Score
                DrawString(width / 4 + 150, height - 240 - i * 40, 
                           to_string(scores[i].score), colors[WHITE]);
                
                // Game mode
                string modeText = scores[i].gameMode == TAXI_MODE ? "Taxi" : "Delivery";
                int modeColor = scores[i].gameMode == TAXI_MODE ? YELLOW : GREEN;
                DrawString(width / 4 + 250, height - 240 - i * 40, 
                           modeText, colors[modeColor]);
            }
        }
    };
    class GameManager {
        private:
            Map* gameMap;
            Car* playerCar;
            static const int NUM_PASSENGERS = 2;
            Passenger* passengers[NUM_PASSENGERS];
            static const int NUM_BOXES = 2;
            Box* boxes[NUM_BOXES];
            int activePassengerIndex = -1;
            int activeBoxIndex = -1;
            int score = 0;
            int money = 0;
            int width, height;
            bool refuelMessage = false;
            int refuelMessageTimer = 0;
            string statusMessage = "";
            static const int NUM_NPC_CARS = 4;
            NPCCar* npcCars[NUM_NPC_CARS];
            bool gameOver = false;
            int moveTimerCounter = 0; 
            int gameTimer = 0;
            int gameState = MENU_STATE;
            Menu* menu;
            Leaderboard* leaderboard;
            Garage* garage;
            string playerName = "Player"; 
            bool nameEntered = false;
            string currentInput = "";
            bool showNameInput = false;
        
        public:
            GameManager(int windowWidth, int windowHeight) {
                width = windowWidth;
                height = windowHeight;
                menu = new Menu();  
                garage = new Garage(40);
                leaderboard = new Leaderboard();
                gameMap = new Map(32, 24, 40);
           
                for (int i = 0; i < NUM_NPC_CARS; i++) {
                    npcCars[i] = new NPCCar(gameMap, 38);
                }
                
                playerCar = new Car(50, 847, 20, width, height, 40, gameMap);
                
                for (int i = 0; i < NUM_PASSENGERS; i++) {
                    passengers[i] = new Passenger(gameMap, 40);
                }
                
                for (int i = 0; i < NUM_BOXES; i++) {
                    boxes[i] = new Box(gameMap, 40);
                }
            }
            
            ~GameManager() {
                delete gameMap;
                delete playerCar;
                delete menu;
                delete garage;
                delete leaderboard;
                
                for (int i = 0; i < NUM_PASSENGERS; i++) {
                    delete passengers[i];
                }
                
                for (int i = 0; i < NUM_BOXES; i++) {
                    delete boxes[i];
                }
                
                for (int i = 0; i < NUM_NPC_CARS; i++) {
                    delete npcCars[i];
                }
            }
            
            void initGameMode(int mode) {
                score = 0;
                money = 0;
                gameTimer = 0;
                gameOver = false;
                refuelMessageTimer = 0;
                statusMessage = "";
                
                if (mode == TAXI_MODE) {
                    for (int i = 0; i < NUM_PASSENGERS; i++) {
                        passengers[i]->spawn();
                    }
                    activePassengerIndex = -1;
                } 
                else if (mode == DELIVERY_MODE) {
                    for (int i = 0; i < NUM_BOXES; i++) {
                        boxes[i]->spawn();
                    }
                    activeBoxIndex = -1;
                }
                
                delete playerCar;
                playerCar = new Car(50, 847, 20, width, height, 40, gameMap);
                
                for (int i = 0; i < NUM_NPC_CARS; i++) {
                    delete npcCars[i];
                    npcCars[i] = new NPCCar(gameMap, 38);
                }
            }
            
            void update() {
                if (gameState == MENU_STATE || gameState == LEADERBOARD_STATE || 
                    showNameInput || (gameOver && !nameEntered)) {
                    return;
                }
                
                if (refuelMessageTimer > 0 && --refuelMessageTimer == 0) {
                    statusMessage = "";
                }
                
                if (gameState == TAXI_MODE) {
                    if (!playerCar->getHasPassenger()) {
                        for (int i = 0; i < NUM_PASSENGERS; i++) {
                            if (!passengers[i]->getIsVisible()) {
                                passengers[i]->spawn();
                            }
                        }
                    }
                }
                else if (gameState == DELIVERY_MODE) {
                    if (!playerCar->getHasPassenger()) {
                        for (int i = 0; i < NUM_BOXES; i++) {
                            if (!boxes[i]->getIsVisible()) {
                                boxes[i]->spawn();
                            }
                        }
                    }
                }
                
                moveTimerCounter++;
                
                if (moveTimerCounter % 10 == 0) {
                    gameTimer++;
                }
                
                if (gameTimer > 0 && gameTimer % 2 == 0 && moveTimerCounter % 10 == 0) {
                    for (int i = 0; i < NUM_NPC_CARS; i++) {
                        npcCars[i]->moveOnTick();
                    }
                }
                
                for (int i = 0; i < NUM_NPC_CARS; i++) {
                    if (npcCars[i]->collidesWith(playerCar->getX(), playerCar->getY(), 20)) {
                        int penalty = (gameState == TAXI_MODE) ? 3 : 5;
                        score -= penalty;
                        statusMessage = "Collision! -" + to_string(penalty) + " points";
                        
                        if (score < 0) {
                            score = 0;
                            gameOver = true;
                            statusMessage = "GAME OVER! Score too low!";
                            refuelMessageTimer = 300;
                        } 
                    }
                }
            }
                
            void showGameOver() {
                statusMessage = "GAME OVER!";
                refuelMessageTimer = 300;
                
                if (leaderboard->isHighScore(score)) {
                    showNameInput = true;
                    currentInput = playerName;
                }
            }
            
            void submitHighScore() {
                if (currentInput.empty()) {
                    currentInput = "Anonymous";
                }
                
                playerName = currentInput;
                leaderboard->addScore(playerName, score, gameState);
                nameEntered = true;
                showNameInput = false;
            }
            
            void draw() {
                glClearColor(0, 0, 0.0, 0);
                glClear(GL_COLOR_BUFFER_BIT);
                
                if (gameState == MENU_STATE) {
                    menu->draw(width, height);
                }
                else if (gameState == LEADERBOARD_STATE) {
                    leaderboard->draw(width, height);
                }
                else {
                    gameMap->draw();
                    garage->draw();
                    
                    if (gameState == TAXI_MODE) {
                        for (int i = 0; i < NUM_PASSENGERS; i++) {
                            passengers[i]->draw();
                        }
                    }
                    else if (gameState == DELIVERY_MODE) {
                        for (int i = 0; i < NUM_BOXES; i++) {
                            boxes[i]->draw();
                        }
                    }
                    
                    for (int i = 0; i < NUM_NPC_CARS; i++) {
                        npcCars[i]->draw();
                    }
                    playerCar->draw();
                    
                    drawUI();
                    if (showNameInput) {
                        drawNameInputDialog();
                    }
                }
                
                glutSwapBuffers();
            }
            
            void drawNameInputDialog() {
                DrawRectangle(0, 0, width, height, colors[BLACK]);
                DrawRoundRect(width / 2 - 200, height / 2 - 100, 400, 200, colors[DARK_BLUE]);
                DrawRoundRect(width / 2 - 195, height / 2 - 95, 390, 190, colors[BLUE]);
                DrawString(width / 2 - 150, height / 2 + 50, "Enter your name:", colors[WHITE]);
                DrawRectangle(width / 2 - 150, height / 2, 300, 30, colors[WHITE]);
                DrawString(width / 2 - 145, height / 2 + 5, currentInput, colors[BLACK]);
                DrawRoundRect(width / 2 - 50, height / 2 - 50, 100, 30, colors[YELLOW]);
                DrawString(width / 2 - 45, height / 2 - 40, "Submit", colors[BLACK]);
                DrawString(width / 2 - 180, height / 2 - 80, "Press ENTER to submit", colors[WHITE]);
            }
            
            void drawUI() {
                int fuelWidth = (200 * playerCar->getFuel()) / 100;
                DrawRectangle(1000, 910, 200, 20, colors[GRAY]);
                DrawRectangle(1000, 910, fuelWidth, 20, colors[RED]);
                DrawString(1000, 930, "Fuel", colors[BLACK]);
                
                DrawString(1000, 880, "Timer: " + to_string(gameTimer) + "s", colors[BLACK]);
                DrawString(550, 900, gameState == TAXI_MODE ? "Taxi Rush" : "Delivery Rush", colors[BLACK]);
                DrawString(40, 920, "Score=" + to_string(score), colors[BLACK]);
                DrawString(40, 890, "Money=$" + to_string(money), colors[BLACK]);
                
                if (!statusMessage.empty()) {
                    DrawString(width / 2 - 130, height - 30, statusMessage, colors[BLACK]);
                }
                
                if (gameOver && !showNameInput) {
                    DrawString(width / 2 - 150, height / 2, "GAME OVER! Press 'R' to restart", colors[RED]);
                }
            }
            
            void handleSpecialKey(int key) {
                if (gameState == MENU_STATE) {
                    if (key == GLUT_KEY_UP) {
                        menu->moveSelection(true);
                    }
                    else if (key == GLUT_KEY_DOWN) {
                        menu->moveSelection(false);
                    }
                    else if (key == GLUT_KEY_RIGHT) {
                        int selection = menu->getSelection();
                        if (selection == 0) {
                            gameState = TAXI_MODE;
                            initGameMode(gameState);
                        } else if (selection == 1) {
                            gameState = DELIVERY_MODE;
                            initGameMode(gameState);
                        } else if (selection == 2) {
                            gameState = LEADERBOARD_STATE;
                        }
                    }
                }
                else if (showNameInput || (gameOver && !nameEntered)) {
                    return; 
                }
                else if (gameState != LEADERBOARD_STATE) {
                    switch(key) {
                        case GLUT_KEY_LEFT: playerCar->moveLeft(); break;
                        case GLUT_KEY_RIGHT: playerCar->moveRight(); break;
                        case GLUT_KEY_UP: playerCar->moveUp(); break;
                        case GLUT_KEY_DOWN: playerCar->moveDown(); break;
                    }
                }
            }
            
            void handleKey(unsigned char key) {
                if (showNameInput) {
                    handleNameInput(key);
                    return;
                }
                
                if (key == 27) { // Escape key
                    if (gameState != MENU_STATE) {
                        gameState = MENU_STATE;
                    } else {
                        exit(1);
                    }
                }
                else if ((key == 'r' || key == 'R') && gameOver) {
                    initGameMode(gameState);
                }
                else if (key == 13 && gameState == MENU_STATE) { // Enter key
                    gameState = menu->getSelection() == 0 ? TAXI_MODE : DELIVERY_MODE;
                    initGameMode(gameState);
                }
                else if (key == 32) { // Space bar
                    if (gameState == TAXI_MODE) {
                        handleTaxiPickupDropoff();
                    } else if (gameState == DELIVERY_MODE) {
                        handleDeliveryPickupDropoff();
                    }
                }
                else if (key == 'f' || key == 'F') {
                    if (playerCar->tryRefuel(money)) {
                        statusMessage = "Car refueled!";
                        refuelMessageTimer = 30;
                    }
                }
                else if (key == 'p' || key == 'P') {
                    int carGridX = playerCar->getX() / gameMap->getCellSize();
                    int carGridY = playerCar->getY() / gameMap->getCellSize();
                    
                    if (garage->isAtGarage(carGridX, carGridY)) {
                        gameState = DELIVERY_MODE;
                        initGameMode(gameState);
                        statusMessage = "Switched to Delivery Mode!";
                        refuelMessageTimer = 30;
                    }
                }
            }
            
            void handleNameInput(unsigned char key) {
                if (key == 8 || key == 127) { // Backspace or Delete
                    if (!currentInput.empty()) {
                        currentInput.pop_back();
                    }
                }
                else if (key == 13) { // Enter
                    submitHighScore();
                }
                else if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || 
                         (key >= '0' && key <= '9') || key == ' ') {
                    if (currentInput.length() < 19) {
                        currentInput += key;
                    }
                }
            }
            
            void handleTaxiPickupDropoff() {
                if (!playerCar->getHasPassenger()) {
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
                } 
                else if (playerCar->getHasPassenger() && activePassengerIndex >= 0) {
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
            
            void handleDeliveryPickupDropoff() {
                if (!playerCar->getHasPassenger()) {
                    for (int i = 0; i < NUM_BOXES; i++) {
                        if (boxes[i]->isPickedUp(playerCar->getX(), playerCar->getY())) {
                            playerCar->pickupPassenger();
                            boxes[i]->pickup();
                            activeBoxIndex = i;
                            statusMessage = "Package picked up!";
                            refuelMessageTimer = 30;
                            break;
                        }
                    }
                } 
                else if (playerCar->getHasPassenger() && activeBoxIndex >= 0) {
                    if (boxes[activeBoxIndex]->isAtDestination(playerCar->getX(), playerCar->getY())) {
                        playerCar->dropoffPassenger(); 
                        boxes[activeBoxIndex]->resetDestination();
                        activeBoxIndex = -1;
                        score += 15; 
                        int payment = GetRandInRange(35, 75); 
                        money += payment;
                        statusMessage = "Delivery completed! +$" + to_string(payment);
                        refuelMessageTimer = 30;
                    }
                }
            }
            
            void handleMouseClick(int button, int state, int x, int y) {
                if (gameState == MENU_STATE && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
                    menu->handleMouseClick(x, height - y, height);
                }
            }
        };
        
        GameManager* gameManagerPtr = nullptr;
        
        void GameDisplay() {
            if (gameManagerPtr) gameManagerPtr->draw();
        }
        
        void NonPrintableKeys(int key, int x, int y) {
            if (gameManagerPtr) gameManagerPtr->handleSpecialKey(key);
            glutPostRedisplay();
        }
        
        void PrintableKeys(unsigned char key, int x, int y) {
            if (gameManagerPtr) gameManagerPtr->handleKey(key);
            glutPostRedisplay();
        }
        
        void Timer(int m) {
            if (gameManagerPtr) gameManagerPtr->update();
            glutPostRedisplay();
            glutTimerFunc(100, Timer, 0);
        }
        
        void MouseClicked(int button, int state, int x, int y) {
            if (gameManagerPtr) gameManagerPtr->handleMouseClick(button, state, x, y);
            glutPostRedisplay();
        }
        
        void SetCanvasSize(int width, int height) {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, 0, height, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
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
            glutPassiveMotionFunc([](int x, int y){ glutPostRedisplay(); });
            glutMotionFunc([](int x, int y){ glutPostRedisplay(); });
            
            glutMainLoop();
            delete gameManagerPtr;
            return 0;
        }
#endif /* RushHour_CPP_ */