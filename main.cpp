#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <random>

// Constants
#define HEIGHT 200
#define WIDTH 400
#define PIX_SIZE 4
#define BRUSH_LIMIT 9

// Typedef
typedef unsigned int u32;

// Create Window
sf::RenderWindow window(sf::VideoMode({WIDTH * PIX_SIZE, HEIGHT * PIX_SIZE}), "Sand Simulator v2.0");

// Other vars...
bool buttonPressed;
u32 mouseType;
int BRUSH_SIZE;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, RAND_MAX);

// Cell TYPE
typedef struct cell_t
{
    u32 id;
    int life_time;
    sf::Color color;
    bool updated;
    u32 weigh;
    u32 flamibility = 0;
    u32 fuel = 0;
    u32 friction = 1;
    bool isFreeFalling = true;
} cell_t;

// MouseCell and Game Grid
cell_t mouseCell;
cell_t grid[HEIGHT][WIDTH];

// Pre-set types of Cells
#define AIR cell_t {0, -100, sf::Color::Black, false, 0}
#define SAND cell_t {1, -100, (dis(gen) & 1 ? sf::Color::Yellow : sf::Color(255, 255, 150)), false, 2, 10}
#define WATER cell_t {2, -100, sf::Color::Blue, false, 1}
#define STONE cell_t {3, -100, sf::Color(169, 169, 169), false, 10}
#define FIRE cell_t {4, 15, (dis(gen) & 1 ? sf::Color::Red : sf::Color (255, 165, 0)), false, 0}
#define SMOKE cell_t {5, 150, sf::Color(70, 70, 70), false, 0}
#define WOOD cell_t {6, -100, sf::Color(155, 103, 60), false, 10, 200, 300}
#define GUNPOWDER cell_t {7, -100, (dis(gen) & 1 ? sf::Color(150, 150, 150) : sf::Color(125, 125, 125)), false, 2, 5, 5}
#define COAL cell_t {8, -100, sf::Color(20, 20, 20), false, 10, 500, 2000}
#define LAVA cell_t {9, -100, sf::Color(255, 128, 0), false, 1, 0, 1000}

void chooseBrushElement()
{
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
        case 5:
            mouseCell = WOOD;
            break;
        case 6:
            mouseCell = GUNPOWDER;
            break;
        case 7:
            mouseCell = COAL;
            break;
        case 8:
            mouseCell = LAVA;
            break;
    }
}

void swapCells(u32 i1, u32 j1, u32 i2, u32 j2)
{
    auto temp = grid[i2][j2];
    grid[i2][j2] = grid[i1][j1];

    if ((grid[i2][j2].id == 2 || grid[i2][j2].id == 1) && temp.id == 4)
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

    if (grid[i][j].isFreeFalling && isWithinBounds(i, j - 1) && dis(gen) % grid[i][j - 1].friction == 0)
        grid[i][j - 1].isFreeFalling = true;
    if (grid[i][j].isFreeFalling && isWithinBounds(i, j + 1) && dis(gen) % grid[i][j + 1].friction == 0)
        grid[i][j + 1].isFreeFalling = true; 
    
    if (isWithinBounds(i + 1, j) && grid[i + 1][j].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j);
    
    else if (!grid[i][j].isFreeFalling)
        return;
    
    else if (isWithinBounds(i + 1, j + 1) && isWithinBounds(i + 1, j - 1) && grid[i + 1][j + 1].weigh < grid[i][j].weigh && grid[i + 1][j - 1].weigh < grid[i][j].weigh)
    {
        switch (dis(gen) & 1) 
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

    else
        grid[i][j].isFreeFalling = false;
}

void updateWater(u32 i, u32 j)
{
    grid[i][j].updated = true;

    if (isWithinBounds(i + 1, j) && grid[i + 1][j].weigh < grid[i][j].weigh)
        swapCells(i, j, i + 1, j); 
    
    else if (isWithinBounds(i + 1, j + 1) && grid[i + 1][j + 1].weigh < grid[i][j].weigh && isWithinBounds(i + 1, j - 1) && grid[i + 1][j - 1].weigh < grid[i][j].weigh)
    {
        switch (dis(gen) & 1)
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
        switch (dis(gen) & 1)
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

    for (int di = -1; di <= 1; ++di)
    {
        for (int dj = -1; dj <= 1; ++dj)
        {
            if (di == 0 && dj == 0) 
                continue;

            u32 ni = i + di;
            u32 nj = j + dj;

            if (isWithinBounds(ni, nj) && grid[ni][nj].flamibility > 0)
            {
                if (dis(gen) % grid[ni][nj].flamibility == 0)
                {
                    auto temp = grid[ni][nj].fuel;
                    grid[ni][nj] = FIRE;
                    grid[ni][nj].life_time += temp;
                }
            }
        }
    }

    if (isWithinBounds(i - 1, j) && grid[i][j].life_time <= 15 && grid[i - 1][j].id == 0 && dis(gen) & 1)
        swapCells(i, j, i - 1, j);
}

void updateSmoke(u32 i, u32 j)
{
    grid[i][j].updated = true;
    
    if (isWithinBounds(i - 1, j) && grid[i - 1][j].id == 0)
        swapCells(i, j, i - 1, j);
    
    else if (isWithinBounds(i - 1, j + 1) && isWithinBounds(i - 1, j - 1) && grid[i - 1][j + 1].id == 0 && grid[i - 1][j - 1].id == 0)
    {
        switch (dis(gen) & 1) 
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

void updateLava(u32 i, u32 j)
{
    updateWater(i, j);

    for (int di = -1; di <= 1; ++di)
    {
        for (int dj = -1; dj <= 1; ++dj)
        {
            if (di == 0 && dj == 0) 
                continue;

            u32 ni = i + di;
            u32 nj = j + dj;

            if (isWithinBounds(ni, nj) && grid[ni][nj].flamibility > 0)
            {
                if (dis(gen) % grid[ni][nj].flamibility == 0)
                {
                    auto temp = grid[ni][nj].fuel;
                    grid[ni][nj] = FIRE;
                    grid[ni][nj].life_time += temp;
                }
            }
        }
    }

    for (int di = -1; di <= 1; ++di)
    {
        for (int dj = -1; dj <= 1; ++dj)
        {
            u32 ni = i + di;
            u32 nj = j + dj;
            if (isWithinBounds(ni, nj) && grid[ni][nj].id == 2)
            {
                grid[i][i] = AIR;
                grid[ni][nj] = SMOKE;
                return;
            }
        }
    }
}

void updateElements(u32 i, u32 j)
{
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
        case 7:
            updateSand(i, j);
            break;
        case 9:
            updateLava(i, j);
            break;
    }
}

void drawUI()
{
    const int buttonSize = 40;
    const int padding = 10;
    const int startX = 10;
    const int startY = 10;

    sf::RectangleShape button(sf::Vector2f(buttonSize, buttonSize));

    sf::Color colors[BRUSH_LIMIT] = 
    {
        sf::Color::Black, // AIR
        sf::Color::Yellow, // SAND
        sf::Color::Blue, // WATER
        sf::Color(169, 169, 169), // STONE
        sf::Color::Red, // FIRE
        sf::Color(155, 103, 60), // WOOD
        sf::Color(150, 150, 150), // GUNPOWDER
        sf::Color(20, 20, 20), // COAL
        sf::Color(255, 128, 0) // LAVA
    };

    for (int i = 0; i < BRUSH_LIMIT; ++i)
    {
        button.setPosition(startX + i * (buttonSize + padding), startY);
        button.setFillColor(colors[i]);
        button.setOutlineThickness(1);
        button.setOutlineColor(sf::Color::White); // Set the outline color to white
        window.draw(button);
    }
}

void handleUIClick(int mouseX, int mouseY)
{
    const int buttonSize = 40;
    const int padding = 10;
    const int startX = 10;
    const int startY = 10;

    if (mouseY >= startY && mouseY <= startY + buttonSize)
    {
        for (int i = 0; i < BRUSH_LIMIT; ++i)
        {
            int buttonX = startX + i * (buttonSize + padding);
            if (mouseX >= buttonX && mouseX <= buttonX + buttonSize)
            {
                mouseType = i;
                chooseBrushElement();
                return; // Exit after handling UI click
            }
        }
    }
}

void init()
{
    // Set every cell to AIR
    for (u32 i = 0; i < HEIGHT; i++)
        for (u32 j = 0; j < WIDTH; j++)
            grid[i][j] = AIR;
    
    // Set framerate limit
    //window.setFramerateLimit(80);

    // Set variables
    mouseType = 0;
    BRUSH_SIZE = 4;
}

void render()
{
    // Clear the screen
    window.clear(sf::Color::Black);

    // Create a texture to represent the grid
    sf::RenderTexture renderTexture;
    renderTexture.create(WIDTH * PIX_SIZE, HEIGHT * PIX_SIZE);
    renderTexture.clear(sf::Color::Black);

    // Render Cells onto the texture
    sf::RectangleShape temp(sf::Vector2f(PIX_SIZE, PIX_SIZE));
    for (u32 i = 0; i < HEIGHT; i++)
    {
        for (u32 j = 0; j < WIDTH; j++)
        {
            if (grid[i][j].id == 0)
                continue;

            temp.setPosition(j * PIX_SIZE, i * PIX_SIZE);
            temp.setFillColor(grid[i][j].color);
            renderTexture.draw(temp);
        }
    }

    // Finalize the texture
    renderTexture.display();

    // Draw the texture to the window
    sf::Sprite sprite(renderTexture.getTexture());
    window.draw(sprite);

    // Draw the outline for the brush
    if (!buttonPressed)
    {
        int mouseX = sf::Mouse::getPosition(window).x / PIX_SIZE;
        int mouseY = sf::Mouse::getPosition(window).y / PIX_SIZE;

        sf::RectangleShape outline(sf::Vector2f(BRUSH_SIZE * PIX_SIZE, BRUSH_SIZE * PIX_SIZE));
        outline.setPosition((mouseX - BRUSH_SIZE / 2) * PIX_SIZE, (mouseY - BRUSH_SIZE / 2) * PIX_SIZE);
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor(sf::Color::White);
        outline.setOutlineThickness(1);

        window.draw(outline);
    }

    // Draw the UI
    drawUI();

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

            if (grid[i][j].life_time != -100 && grid[i][j].life_time <= 0)
            {
                if (grid[i][j].id == 4 && dis(gen) & 1)
                    grid[i][j] = SMOKE;
                else
                    grid[i][j] = AIR;
            }
            
            updateElements(i, j);

            if (grid[i][j].life_time != -100)
            {
                grid[i][j].life_time--;
                if (grid[i][j].id == 4)
                    grid[i][j].color = (dis(gen) & 1 ? sf::Color::Red : sf::Color (255, 165, 0));
            }
        }
    }
    
    chooseBrushElement();

    if (buttonPressed)
    {
        int X = sf::Mouse::getPosition(window).y / PIX_SIZE, Y = sf::Mouse::getPosition(window).x / PIX_SIZE;

        for (int i = int(-(BRUSH_SIZE / 2)); i <= int((BRUSH_SIZE / 2)); i++)
        {
            for (int j = int(-(BRUSH_SIZE / 2)); j <= int((BRUSH_SIZE / 2)); j++)
            {
                if (isWithinBounds(i + X, j + Y))
                    grid[i + X][j + Y] = mouseCell;
            }
        }
    }
}

void keys(sf::Event event)
{
    // Check if they close the program
    if (event.type == sf::Event::Closed)
        window.close();

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        // Check if the click is within the UI area
        int mouseX = event.mouseButton.x;
        int mouseY = event.mouseButton.y;
        const int buttonSize = 40;
        const int startY = 10;

        if (mouseY >= startY && mouseY <= startY + buttonSize)
            handleUIClick(mouseX, mouseY);
        else
            buttonPressed = true; // Only set buttonPressed if the click is outside the UI
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        buttonPressed = false;

    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Up)
            BRUSH_SIZE = std::min(BRUSH_SIZE + 1, WIDTH / 2);
        else if (event.key.code == sf::Keyboard::Down)
            BRUSH_SIZE = std::max(BRUSH_SIZE - 1, 1);
    }

    if (event.type == sf::Event::MouseWheelMoved)
    {
        if (event.mouseWheel.delta > 0)
            BRUSH_SIZE = std::min(BRUSH_SIZE + 1, WIDTH / 2);
        else if (event.mouseWheel.delta < 0)
            BRUSH_SIZE = std::max(BRUSH_SIZE - 1, 1);
    }
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
