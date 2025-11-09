#include "breakout.hpp"
#include "raylib.h"
#include <vector>
#include <iostream>

// Menu frame class
Menu::Menu()
{
    rect = {100, 100, 200, 50};
}

void Menu::draw()
{
    DrawRectangleRec(rect, RED);
}

void Menu::update(Frame_Type &frame)
{
    rect.x += 1;
}

//--------------------------------------

// Levels frame class

Levels::Levels()
{
    x = 100;
    y = 50;
}

void Levels::draw()
{
    DrawText("Mytexte,", x, y, 30, BLUE);
}

void Levels::update(Frame_Type &frame)
{
    x += 1;
}

//--------------------------------------------------------

// Game frame class

Game::Game()
{
    InitAudioDevice();
    paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * 2), float(brick_height)};
    file.open("levels/level4.json");
    if (file.is_open())
    {
        std::cout << "Ouvert" << std::endl;
    }
    file >> j;
    bricks = j["bricks"].get<std::vector<Brick>>();
    ball = {{paddle.x + brick_width, paddle.y - 5}, {2.5, -2.5}, 7, PURPLE};
    isBegin = true;
    isFinish = false;
    b_sound = LoadSound("resources/Hit1.wav");
    m_sound = LoadSound("resources/Pickup8.wav");
    allInactive = false;
    lives = 3;
    livesRaduis = 15;
}

void Game::draw()
{
    for (auto &brick : bricks)
    {
        if (brick.isActive)
        {
            DrawRectangleRec(brick.b, brick.color);
            DrawRectangleLines(brick.b.x, brick.b.y, brick.b.width, brick.b.height, LIGHTGRAY);
        }
    }

    DrawRectangleRec(ui_frame, BLUE);
    DrawRectangleRec(paddle, GREEN);
    DrawCircleV(ball.ball_pos, ball.raduis, ball.color);

    for (int i = 0; i < lives; i++)
    {
        DrawCircle((ui_frame.x + livesRaduis + 10) + i * 50, livesRaduis * 2, livesRaduis, PURPLE);
    }

    if (isBegin)
    {
        DrawText("Press SPACE to start", 60, brick_height * 20, 20, GREEN);
    }

    if (allInactive)
    {
        DrawText("Gagner", 60, brick_height * 20, 20, GREEN);
    }

    if (lives <= 0)
    {
        DrawText("Perdu", 60, brick_height * 20, 20, GREEN);
    }

    // draw particule
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            DrawCircle(particles[i].pos.x, particles[i].pos.y, particles[i].raduis, particles[i].color);
        }
    }
}

// This function checks if all breakable bricks are inactive
bool Game::AllBrokenBricksDeactivated(const std::vector<Brick> &bricks)
{
    for (const Brick &brick : bricks)
    {
        if (brick.isActive == true && brick.numbers_of_lives <= 3)
        {
            return false;
        }
    }
    return true; // aucune brique cassée trouvée active
}

// Initialise
void Game::spawnExplosion(Vector2 pos, Color color)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].active = true;
        particles[i].pos = pos;
        particles[i].life = 0.5f + (float)GetRandomValue(0, 50) / 100; // 0.5 → 1 sec
        particles[i].vel = {
            (float)GetRandomValue(-200, 200) / 100,
            (float)GetRandomValue(-200, 200) / 100};
        particles[i].raduis = GetRandomValue(1, 5);
        particles[i].color = color;
    }
}

void Game::updateParticles(float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            particles[i].pos.x += particles[i].vel.x;
            particles[i].pos.y += particles[i].vel.y;
            particles[i].life -= dt;

            if (particles[i].life <= 0)
                particles[i].active = false;
        }
    }
}

// --------------------- COLLISION PARFAITE ---------------------
void Game::handleBallBrickCollision()
{
    for (auto &brick : bricks)
    {
        if (!brick.isActive)
            continue;

        if (CheckCollisionCircleRec(ball.ball_pos, ball.raduis, brick.b))
        {
            // Calculer le centre relatif
            float ballCenterX = ball.ball_pos.x;
            float ballCenterY = ball.ball_pos.y;
            float brickCenterX = brick.b.x + brick.b.width / 2;
            float brickCenterY = brick.b.y + brick.b.height / 2;

            float dx = ballCenterX - brickCenterX;
            float dy = ballCenterY - brickCenterY;

            // Rebond selon la direction dominante
            if (fabs(dx) > fabs(dy))
                ball.vel.x *= -1; // rebond horizontal
            else
                ball.vel.y *= -1; // rebond vertical

            // Brique cassable
            if (brick.brick_type != UNBREAKABLE)
            {
                brick.numbers_of_lives--;
                spawnExplosion({brick.b.x + brick.b.width / 2, brick.b.y + brick.b.height / 2}, brick.color);

                if (brick.numbers_of_lives <= 0)
                    brick.isActive = false;
            }

            PlaySound(b_sound);
        }
    }
}

void Game::update(Frame_Type &frame)
{
    Vector2 mouse = GetMousePosition();

    // Handle events
    if ((mouse.x > 0 && mouse.x <= ui_frame.x) && !isBegin && !isFinish)
    {
        paddle.x = mouse.x - paddle.width / 2;
    }

    if (mouse.x >= ui_frame.x - paddle.width / 2 && !isBegin)
    {
        paddle.x = ui_frame.x - paddle.width;
    }
    if (mouse.x <= paddle.width / 2 && !isBegin)
    {
        paddle.x = 0;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        isBegin = false;
    }

    // --
    if (!isBegin)
    {
        ball.ball_pos.y += ball.vel.y;
        ball.ball_pos.x += ball.vel.x;
    }

    // Check collisions
    if (ball.ball_pos.x <= 0)
    {
        ball.vel.x *= -1;
        PlaySound(m_sound);
    }
    if (ball.ball_pos.x >= ui_frame.x)
    {
        ball.vel.x *= -1;
        PlaySound(m_sound);
    }
    if (ball.ball_pos.y <= 0)
    {
        ball.vel.y *= -1;
        PlaySound(m_sound);
    }

    if (!isBegin && CheckCollisionCircleRec(ball.ball_pos, ball.raduis, paddle))
    {
        ball.vel.y *= -1;
        PlaySound(m_sound);
    }
    //------------END collision cheking--------------

    if (ball.ball_pos.y >= GetScreenHeight())
    {
        isBegin = true;
        paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * 2), float(brick_height)};
        ball.ball_pos = {paddle.x + brick_width, paddle.y - 5};
        // PlaySound(m_sound);
        lives--;
    }

    handleBallBrickCollision();
    //-------------------------------------------------

    // Check all bricks is inactive
    allInactive = AllBrokenBricksDeactivated(bricks);

    // Reset ball and paddle position if all brick are destroys
    if (allInactive || lives <= 0)
    {
        isFinish = true;
        ball = {{paddle.x + brick_width, paddle.y - 5}, {2.0, -2.0}, 5, PURPLE};
        paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * 2), float(brick_height)};
    }

    updateParticles(GetFrameTime());
}
//-------------------------------------------------------------