#pragma once

/*
	Pong paddle .h
	With Vec2 positioning
*/

#include "Vec2.h"
#include <SDL.h>

const int PADDLE_WIDTH = 12;
const int PADDLE_HEIGHT = 120;
const float PADDLE_SPEED = 1.0f;

class Paddle
{
public:
	Paddle(Vec2 position, Vec2 velocity, int windowHeight)
		: position(position), velocity(velocity), windowHeight(windowHeight)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = PADDLE_WIDTH;
		rect.h = PADDLE_HEIGHT;
	}

	void Update(float dt)
	{
		// dt used to make movement frame rate-independent
		position += velocity * dt;

		// Restrict to top/bottom of the window
		if (position.y < 0) 
		{ 
			position.y = 0; 
		}
		else if (position.y > (windowHeight - PADDLE_HEIGHT)) 
		{
			position.y = windowHeight - PADDLE_HEIGHT;
		}
	}

	void Draw(SDL_Renderer* renderer)
	{
		// Only y-axis movement
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(renderer, &rect);
	}
	
	Vec2 position;
	Vec2 velocity;
	SDL_Rect rect{};

private:
	int windowHeight;
};

