/*
	Pong ball cpp
*/

#include "Ball.h"
#include "Paddle.h"
#include "Collision.h"

Ball::Ball(Vec2 position, Vec2 velocity)
    : position(position), velocity(velocity)
{
    rect.x = static_cast<int>(position.x);
    rect.y = static_cast<int>(position.y);
    rect.w = BALL_WIDTH;
    rect.h = BALL_HEIGHT;
}

void Ball::CollideWithPaddle(Contact const& contact)
{
    position.x += contact.penetration;
    velocity.x = -velocity.x;

    if (contact.type == CollisionType::Top)
    {
        velocity.y = -0.75f * BALL_SPEED;
    }
    else if (contact.type == CollisionType::Bottom)
    {
        velocity.y = 0.75f * BALL_SPEED;
    }
}

void Ball::Update(float dt)
{
    position += velocity * dt;
}

void Ball::Draw(SDL_Renderer* renderer)
{
    rect.x = static_cast<int>(position.x);
    rect.y = static_cast<int>(position.y);

    SDL_RenderFillRect(renderer, &rect);
}