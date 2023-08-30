/*
    C++ PONG clone
    Tutorial by Austin Morlan: https://austinmorlan.com/posts/pong_clone/
    Mods by me
*/

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <chrono>
#include <cstdlib>
#include <sstream>
#include "SerialPort.h"
#include "Ball.h"
#include "Paddle.h"
#include "PlayerScore.h"
#include "Collision.h"

// Enum and struct definitions
enum Buttons
{
    PaddleOneUp = 0,
    PaddleOneDown,
    PaddleTwoUp,
    PaddleTwoDown,
};

// Constant variables
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

void Ball::CollideWithWall(Contact const& contact)
{
    // Bounce from top and bottom walls at an angle
    if ((contact.type == CollisionType::Top) || (contact.type == CollisionType::Bottom))
    {
        position.y += contact.penetration;
        velocity.y = -velocity.y;
    }

    // Scoring, ball moves back to center
    else if (contact.type == CollisionType::Left)
    {
        position.x = WINDOW_WIDTH / 2.0f;
        position.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * WINDOW_HEIGHT;
        velocity.x = BALL_SPEED;

        // Randomize the angle of y-velocity
        velocity.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
        velocity.y *= 0.75f * BALL_SPEED; // Scale by speed
    }
    else if (contact.type == CollisionType::Right)
    {
        position.x = WINDOW_WIDTH / 2.0f;
        position.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * WINDOW_HEIGHT;
        velocity.x = -BALL_SPEED;

        // Randomize the angle of y-velocity
        velocity.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
        velocity.y *= 0.75f * BALL_SPEED; // Scale by speed
    }
}

Contact CheckWallCollision(Ball const& ball)
{
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + BALL_WIDTH;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + BALL_HEIGHT;

    Contact contact{};

    if (ballLeft < 0.0f)
    {
        contact.type = CollisionType::Left;
    }
    else if (ballRight > WINDOW_WIDTH)
    {
        contact.type = CollisionType::Right;
    }
    else if (ballTop < 0.0f)
    {
        contact.type = CollisionType::Top;
        contact.penetration = -ballTop;
    }
    else if (ballBottom > WINDOW_HEIGHT)
    {
        contact.type = CollisionType::Bottom;
        contact.penetration = WINDOW_HEIGHT - ballBottom;
    }

    return contact;
}


// Using the Separating Axis Theorem (SAT) to detect collision
Contact CheckPaddleCollision(Ball const& ball, Paddle const& paddle)
{
    // "Projections" onto axes
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + BALL_WIDTH;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + BALL_HEIGHT;

    float paddleLeft = paddle.position.x;
    float paddleRight = paddle.position.x + PADDLE_WIDTH;
    float paddleTop = paddle.position.y;
    float paddleBottom = paddle.position.y + PADDLE_HEIGHT;

    Contact contact{};

    if (ballLeft >= paddleRight)
    {
        return contact;
    }

    if (ballRight <= paddleLeft)
    {
        return contact;
    }

    if (ballTop >= paddleBottom)
    {
        return contact;
    }

    if (ballBottom <= paddleTop)
    {
        return contact;
    }

    // Set paddle regions for angled ball return
    float paddleRangeUpper = paddleBottom - (2.0f * PADDLE_HEIGHT / 3.0f);
    float paddleRangeMiddle = paddleBottom - (PADDLE_HEIGHT / 3.0f);

    if (ball.velocity.x < 0)
    {
        // Left paddle
        contact.penetration = paddleRight - ballLeft;
    }
    else if (ball.velocity.x > 0)
    {
        // Right paddle
        contact.penetration = paddleLeft - ballRight;
    }

    if ((ballBottom > paddleTop) && (ballBottom < paddleRangeUpper))
    {
        contact.type = CollisionType::Top;
    }
    else if ((ballBottom > paddleRangeUpper) && (ballBottom < paddleRangeMiddle))
    {
        contact.type = CollisionType::Middle;
    }
    else
    {
        contact.type = CollisionType::Bottom;
    }

    return contact;
}


int SDL_main(int argc, char* argv[]) 
{
    // Initialize SDL components
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("PONG", 200, 200, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    TTF_Init();

    // Initialize the font and color
    TTF_Font* scoreFont = TTF_OpenFont("DejaVuSansMono.ttf", 40);
    SDL_Color scoreColor = { 0xFF, 0xFF, 0xFF, 0xFF };

    // Initialize the serial port for Arduino communication
    const char* serialPortName = "COM10";  // Adjust the COM port as needed
    const int baudRate = 9600;
    SerialPort arduino(serialPortName, baudRate);

    // Create the player score text fields
    PlayerScore playerOneScoreText(Vec2(WINDOW_WIDTH / 4, 20), renderer, scoreFont, scoreColor);
    PlayerScore playerTwoScoreText(Vec2(3 * WINDOW_WIDTH / 4, 20), renderer, scoreFont, scoreColor);

    // Create the ball
    Ball ball(
        Vec2((WINDOW_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f), (WINDOW_HEIGHT / 2.0f) - (BALL_HEIGHT / 2.0f)),
        Vec2(BALL_SPEED, 0.0f));

    // Create the paddles
    Paddle paddleOne(
        Vec2(40.0f, WINDOW_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f), 
        Vec2(0.0f, 0.0f), WINDOW_HEIGHT);

    Paddle paddleTwo(
        Vec2(WINDOW_WIDTH - 40.0f, WINDOW_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f), 
        Vec2(0.0f, 0.0f), WINDOW_HEIGHT);

    // Game logic & rendering
    {

        int playerOneScore = 0;
        int playerTwoScore = 0;
        bool gameOver = false;

        bool running = true;
        bool buttons[4] = {};

        float dt = 0.0f;

        // Continue looping and processing events until user exits
        while (running) 
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            std::string arduinoData = arduino.Readline();
            std::stringstream ss(arduinoData);

            SDL_Event event;
            while (SDL_PollEvent(&event)) 
            {
                if (event.type == SDL_QUIT) // Quit game
                {
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN) // Key is pressed
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE) // Quit game with ESC
                    {
                        running = false;
                    }
                    else if (event.key.keysym.sym == SDLK_w)
                    {
                        buttons[Buttons::PaddleOneUp] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_s)
                    {
                        buttons[Buttons::PaddleOneDown] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_UP)
                    {
                        buttons[Buttons::PaddleTwoUp] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        buttons[Buttons::PaddleTwoDown] = true;
                    }
                }
                else if (event.type == SDL_KEYUP) // Key is "unpressed"
                {
                    if (event.key.keysym.sym == SDLK_w)
                    {
                        buttons[Buttons::PaddleOneUp] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_s)
                    {
                        buttons[Buttons::PaddleOneDown] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_UP)
                    {
                        buttons[Buttons::PaddleTwoUp] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        buttons[Buttons::PaddleTwoDown] = false;
                    }
                }
            }

            if (buttons[Buttons::PaddleOneUp])
            {
                paddleOne.velocity.y = -PADDLE_SPEED;
            }
            else if (buttons[Buttons::PaddleOneDown])
            {
                paddleOne.velocity.y = PADDLE_SPEED;
            }
            else
            {
                paddleOne.velocity.y = 0.0f;
            }

            if (buttons[Buttons::PaddleTwoUp])
            {
                paddleTwo.velocity.y = -PADDLE_SPEED;
            }
            else if (buttons[Buttons::PaddleTwoDown])
            {
                paddleTwo.velocity.y = PADDLE_SPEED;
            }
            else
            {
                paddleTwo.velocity.y = 0.0f;
            }

            // Update paddle positions
            paddleOne.Update(dt);
            paddleTwo.Update(dt);

            // Update ball position
            ball.Update(dt);

            // Check for collisions and send ball back in other direction
            Contact contact;
            if (!gameOver)
            {
                if ((contact = CheckPaddleCollision(ball, paddleOne)).type != CollisionType::None)
                {
                    ball.CollideWithPaddle(contact);
                }
                else if ((contact = CheckPaddleCollision(ball, paddleTwo)).type != CollisionType::None)
                {
                    ball.CollideWithPaddle(contact);
                }
                else if ((contact = CheckWallCollision(ball)).type != CollisionType::None)
                {
                    ball.CollideWithWall(contact);

                    if (contact.type == CollisionType::Left)
                    {
                        ++playerTwoScore;
                        playerTwoScoreText.SetScore(playerTwoScore);
                    }
                    else if (contact.type == CollisionType::Right)
                    {
                        ++playerOneScore;
                        playerOneScoreText.SetScore(playerOneScore);
                    }

                    // Check for winner
                    if (playerOneScore >= 10 || playerTwoScore >= 10)
                    {
                        gameOver = true;
                    }
                }
            }

            // Clear the window to black
            SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
            SDL_RenderClear(renderer);

            // RENDERING: -------------------------------------------------------------------------

            // Set draw colour to white
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

            // Draw the net (wherever y is not a multiple of 5 pixels => dashed line)
            for (int y = 0; y < WINDOW_HEIGHT; y++) 
            {
                if (y % 5) SDL_RenderDrawPoint(renderer, WINDOW_WIDTH / 2, y);
            }
           
            // Draw the ball
            ball.Draw(renderer);

            // Draw the paddles
            paddleOne.Draw(renderer);
            paddleTwo.Draw(renderer);

            // Display the scores
            playerOneScoreText.Draw();
            playerTwoScoreText.Draw();

            // Display the winner if the game is over
            if (gameOver)
            {
                SDL_Color winnerColor = { 0xFF, 0xFF, 0xFF, 0xFF };
                TTF_Font* winnerFont = TTF_OpenFont("DejaVuSansMono.ttf", 60);

                SDL_Surface* winnerSurface = nullptr;
                SDL_Texture* winnerTexture = nullptr;
                SDL_Rect winnerRect;

                if (playerOneScore >= 10)
                {
                    winnerSurface = TTF_RenderText_Solid(winnerFont, "Player One Wins!", winnerColor);
                }
                else if (playerTwoScore >= 10)
                {
                    winnerSurface = TTF_RenderText_Solid(winnerFont, "Player Two Wins!", winnerColor);
                }

                if (winnerSurface)
                {
                    winnerTexture = SDL_CreateTextureFromSurface(renderer, winnerSurface);
                    SDL_FreeSurface(winnerSurface);

                    if (winnerTexture)
                    {
                        SDL_QueryTexture(winnerTexture, nullptr, nullptr, &winnerRect.w, &winnerRect.h);
                        winnerRect.x = (WINDOW_WIDTH - winnerRect.w) / 2;
                        winnerRect.y = (WINDOW_HEIGHT - winnerRect.h) / 2;

                        SDL_RenderCopy(renderer, winnerTexture, nullptr, &winnerRect);
                        SDL_DestroyTexture(winnerTexture);
                    }
                }

                TTF_CloseFont(winnerFont);
            }


            // Present the backbuffer
            SDL_RenderPresent(renderer);

            auto stopTime = std::chrono::high_resolution_clock::now();
            dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count(); // delta time
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(scoreFont);
    TTF_Quit();
    SDL_Quit();

    return 0;
}