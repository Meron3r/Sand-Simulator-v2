#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <cstdlib>

// Constants
#define HEIGHT 200
#define WIDTH 300
#define PIX_SIZE 5

// Typedef
typedef unsigned int u32;

// Create Window
sf::RenderWindow window(sf::VideoMode({WIDTH * PIX_SIZE, HEIGHT * PIX_SIZE}), "Sand Simulator v2.0");

// Other vars...
bool buttonPressed;
u32 mouseType;

// Cell TYPE
typedef struct cell_t
{
    u32 id;
    int life_time;
    sf::Color color;
    bool updated;
    u32 weigh;
    u32 flamibility;
} cell_t;

// Pre-set types of Cells
#define AIR cell_t {0, -1, sf::Color::Black, false, 0, 0}
#define SAND cell_t {1, -1, (rand() & 1 ? sf::Color::Yellow : sf::Color(255, 255, 150)), false, 2, 5}
#define WATER cell_t {2, -1, sf::Color::Blue, false, 1, 0}
#define STONE cell_t {3, -1, sf::Color(169, 169, 169), false, 10, 0}
#define FIRE cell_t {4, 10, sf::Color::Red, false, 0}
#define SMOKE cell_t {5, 25, sf::Color(30, 30, 30), false, 0}

// MouseCell
cell_t mouseCell;

// Game Grid
cell_t grid[HEIGHT][WIDTH];

void swapCells(u32 i1, u32 j1, u32 i2, u32 j2)
{
    auto temp = grid[i2][j2];
    grid[i2][j2] = grid[i1][j1];

    if (grid[i2][j2].id == 2 && temp.id == 4)
        grid[i1][j1] = SMOKE;
    else 
        grid[i1][j1] = temp;
}

bool isWithinBounds(u32 x, u32 y)
{
    bool validX = (x >= 0 && x < HEIGHT);
    bool validY = (y >= 0 && y < WIDTH);
    return validX && validY;
}

void updateSand(u32 i, u32 j)
{
    grid[i][j].updated = true;
    
    if (isWithinBounds(i + 1, j) && grid[i + 1][j].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j);
    
    else if (isWithinBounds(i + 1, j + 1) && isWithinBounds(i + 1, j - 1) && grid[i + 1][j + 1].weigh < grid[i][j].weigh && grid[i + 1][j - 1].weigh < grid[i][j].weigh)
    {
        switch (rand() & 1) 
        {
            case 0:
                swapCells(i, j, i + 1, j + 1);
                break;
            case 1:
                swapCells(i, j, i + 1, j - 1);
                break;
        }
    }

    else if (isWithinBounds(i + 1, j + 1) && grid[i + 1][j + 1].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j + 1);
    else if (isWithinBounds(i + 1, j - 1) && grid[i + 1][j - 1].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j - 1);
}

void updateWater(u32 i, u32 j)
{
    grid[i][j].updated = true;

    if (isWithinBounds(i + 1, j) && grid[i + 1][j].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j); 
    
    else if (isWithinBounds(i + 1, j + 1) && grid[i + 1][j + 1].weigh < grid[i][j].weigh && isWithinBounds(i + 1, j - 1) && grid[i + 1][j - 1].weigh < grid[i][j].weigh)
    {
        switch (rand() & 1)
        {
            case 0:
                swapCells(i, j, i + 1, j + 1);
                break;
            case 1:
                swapCells(i, j, i + 1, j - 1);
                break;
        }
    }

    else if (isWithinBounds(i + 1, j + 1) && grid[i + 1][j + 1].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j + 1);
    else if (isWithinBounds(i + 1, j - 1) && grid[i + 1][j - 1].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j - 1);

    else if (isWithinBounds(i, j + 1) && grid[i][j + 1].weigh < grid[i][j].weigh && isWithinBounds(i, j - 1) && grid[i][j - 1].weigh < grid[i][j].weigh)
    {
        switch (rand() & 1)
        {
            case 0:
                swapCells(i, j, i, j + 1);
                break;
            case 1:
                swapCells(i, j, i, j - 1);
                break;
        }
    }

    else if (isWithinBounds(i, j + 1) && grid[i][j + 1].weigh < grid[i][j].weigh)
        swapCells(i, j, i, j + 1);
    else if (isWithinBounds(i, j - 1) && grid[i][j - 1].weigh < grid[i][j].weigh) 
        swapCells(i, j, i, j - 1); 
}

void updateFire(u32 i, u32 j)
{
    grid[i][j].updated = true;

    if (isWithinBounds(i - 1, j) && grid[i - 1][j].id == 0 && rand() & 1)
        swapCells(i, j, i - 1, j);
}

void updateSmoke(u32 i, u32 j)
{
    grid[i][j].updated = true;
    
    if (isWithinBounds(i - 1, j) && grid[i - 1][j].id == 0)
        swapCells(i, j, i - 1, j);
    
    else if (isWithinBounds(i - 1, j + 1) && isWithinBounds(i - 1, j - 1) && grid[i - 1][j + 1].id == 0 && grid[i - 1][j - 1].id == 0)
    {
        switch (rand() & 1) 
        {
            case 0:
                swapCells(i, j, i - 1, j + 1);
                break;
            case 1:
                swapCells(i, j, i - 1, j - 1);
                break;
        }
    }

    else if (isWithinBounds(i - 1, j + 1) && grid[i - 1][j + 1].id == 0)
        swapCells(i, j, i - 1, j + 1);
    else if (isWithinBounds(i - 1, j - 1) && grid[i - 1][j - 1].id == 0)
        swapCells(i, j, i - 1, j - 1);
}

void init()
{
    // Set every cell to AIR
    for (u32 i = 0; i < HEIGHT; i++)
        for (u32 j = 0; j < WIDTH; j++)
            grid[i][j] = AIR;
    
    // Set framerate limit
    window.setFramerateLimit(80);
    
    // Setup rand()
    srand(time(0));

    // Set mouseType
    mouseType = 0;
}

void render()
{
    // Clear the screen
    window.clear(sf::Color::Black);
    
    // Render Cells
    sf::RectangleShape temp(sf::Vector2f (PIX_SIZE, PIX_SIZE));
    for (u32 i = 0; i < HEIGHT; i++)
    {
        for (u32 j = 0; j < WIDTH; j++)
        {
            temp.setPosition(j * PIX_SIZE, i * PIX_SIZE);
            temp.setFillColor(grid[i][j].color);

            if (grid[i][j].id == 0)
                continue;

            window.draw(temp);
        }
    }
    
    // Display to the window
    window.display();
}

void update()
{ 
    for (u32 i = 0; i < HEIGHT; i++)
        for (u32 j = 0; j < WIDTH; j++)
            grid[i][j].updated = false;

    for (u32 i = 0; i < HEIGHT; i++)
    {
        for (u32 j = 0; j < WIDTH; j++)
        {
            if (grid[i][j].id == 0 || grid[i][j].updated == true)
                continue;

            if (grid[i][j].life_time != -1 && grid[i][j].life_time == 0)
            {
                if (grid[i][j].id == 4 && rand() & 1)
                    grid[i][j] = SMOKE;
                else
                    grid[i][j] = AIR;
            }
            
            switch (grid[i][j].id) 
            {
                case 1:
                    updateSand(i, j);
                    break;
                case 2:
                    updateWater(i, j);
                    break;
                case 4:
                    updateFire(i, j);
                    break;
                case 5:
                    updateSmoke(i, j);
                    break;
            }

            if (grid[i][j].life_time != -1)
            {
                grid[i][j].life_time--;
                if (grid[i][j].id == 4)
                    grid[i][j].color = (rand() & 1 ? sf::Color::Red : sf::Color (255, 165, 0));
            }
        }
    }
    
    switch (mouseType)
    {
        case 0:
            mouseCell = AIR;
            break;
        case 1:
            mouseCell = SAND;
            break;
        case 2:
            mouseCell = WATER;
            break;
        case 3:
            mouseCell = STONE;
            break;
        case 4:
            mouseCell = FIRE;
            break;
    }

    if (buttonPressed)
    {
        auto X = sf::Mouse::getPosition(window).y / PIX_SIZE, Y = sf::Mouse::getPosition(window).x / PIX_SIZE;
        
        if (Y + 1 < HEIGHT)
            grid[X][Y + 1] = mouseCell;
        if (X - 1 >= 0)
            grid[X - 1][Y] = mouseCell;
        if (X + 1 < WIDTH)
            grid[X + 1][Y] = mouseCell;
        if (Y - 1 >= 0)
            grid[X][Y - 1] = mouseCell;

        grid[X][Y] = mouseCell;
    }
}

void keys(sf::Event event)
{
    // Check if they close the program
    if (event.type == sf::Event::Closed)
        window.close();
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        buttonPressed = true;
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        buttonPressed = false;

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
        mouseType = (mouseType + 1) % 5;
}

int main()
{
    init();
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            keys(event);

        update();
        render();
    }
}
