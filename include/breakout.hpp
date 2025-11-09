#pragma once
#include "raylib.h"
#include "json.hpp"
#include <fstream>
#include "editor.hpp"
#include <vector>
#include <memory>

using json = nlohmann::json;

enum Frame_Type
{
    MENU,
    LEVELS,
    LEVEL1,
};

// Ball struct
struct Ball
{
    Vector2 ball_pos;
    Vector2 vel;
    int raduis;
    Color color;
};

// Particule struct
struct Particle
{
    Vector2 pos;
    Vector2 vel;
    float life;
    bool active;
    int raduis;
    Color color;
};

// Virtual bas class for frame system manager
class Frame
{
protected:
public:
    virtual ~Frame() = default;
    virtual void draw() = 0;
    virtual void update(Frame_Type &frame) = 0;
};
// ------------------------------------------------

// Menu frame class
class Menu : public Frame
{
private:
    Rectangle rect;

public:
    Menu();
    void draw() override;
    void update(Frame_Type &frame) override;
};

//--------------------------------------------------

// Levels frame class
class Levels : public Frame
{
private:
    int x, y;

public:
    Levels();
    void draw() override;
    void update(Frame_Type &frame) override;
};

//----------------------------------------------------

// Gameplay frame class
class Game : public Frame
{
private:
    // Window seting values
    const int brick_width = 30;
    const int brick_height = 15;
    const int max_brick_verticale_number = 30;
    const int max_brick_horizontale_number = 12;
    const int ui_part = 10;
    Rectangle ui_frame = {float(brick_width * (max_brick_horizontale_number)), 0.0f, float(brick_width *(ui_part)), float(max_brick_verticale_number *brick_height)};

    Rectangle paddle;
    std::ifstream file;
    json j;
    std::vector<Brick> bricks; // The struct Brick is define in header editor.hpp
    Ball ball;
    bool isBegin;

    Sound b_sound;
    Sound m_sound;

    bool isFinish;
    bool allInactive;

    int lives;
    float livesRaduis;

    static const int MAX_PARTICLES = 50;
    Particle particles[MAX_PARTICLES];

public:
    Game();
    void draw() override;
    void update(Frame_Type &frame) override;
    bool AllBrokenBricksDeactivated(const std::vector<Brick> &bricks);
    void spawnExplosion(Vector2 pos,Color color);
    void updateParticles(float dt);
    void handleBallBrickCollision();
};

//----------------------------------------------------

// Breakout game manager
class Breakout
{
private:
    const int brick_width = 30;
    const int brick_height = 15;
    const int max_brick_verticale_number = 30;
    const int max_brick_horizontale_number = 12;
    const int ui_part = 10;

    std::unique_ptr<Frame> current_frame;
    Frame_Type frame;

public:
    Breakout();
    ~Breakout();
    void draw();
    void update();
    void run();
};

//-------------------------------------------------