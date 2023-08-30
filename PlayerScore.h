#pragma once

/*
	Player Score .h
	With Vec2 positioning
*/

#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Vec2.h"


class PlayerScore
{
public:
	PlayerScore(Vec2 position, SDL_Renderer* renderer, TTF_Font* font, SDL_Color textColor)
		: renderer(renderer), font(font), textColor(textColor)
	{
		surface = TTF_RenderText_Solid(font, "0", textColor);
		texture = SDL_CreateTextureFromSurface(renderer, surface);


		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = width;
		rect.h = height;
	}

	~PlayerScore()
	{
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	void SetScore(int score)
	{
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		surface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), { 0xFF, 0xFF, 0xFF, 0xFF });
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
		rect.w = width;
		rect.h = height;
	}

	void Draw()
	{
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}

	Vec2 position;
	SDL_Rect rect{};
	SDL_Renderer* renderer;
	SDL_Surface* surface{};
	SDL_Texture* texture{};
    SDL_Color textColor;
	TTF_Font* font;
};

