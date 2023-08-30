#pragma once

/*
	Pong ball .h
	With Vec2 positioning
*/

#include "Vec2.h"
#include "Collision.h"
#include <SDL.h>

// Forward declarations
enum class CollisionType;
struct Contact;

const int BALL_WIDTH = 15;
const int BALL_HEIGHT = 15;
const float BALL_SPEED = 0.9f;

class Ball
{
public:
    Ball(Vec2 position, Vec2 velocity);

    void CollideWithWall(Contact const& contact);
    void CollideWithPaddle(Contact const& contact);

    void Update(float dt);

    void Draw(SDL_Renderer* renderer);

    Vec2 position;
    Vec2 velocity;
    SDL_Rect rect{};
};