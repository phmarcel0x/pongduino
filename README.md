## C++ PONG CLONE - Clone with a Few Tweaks
**Credits:** [Austin Morlan's Pong Clone](https://austinmorlan.com/posts/pong_clone/)

![GitHub last commit](https://img.shields.io/github/last-commit/phmarcel0x/pongduino)
![GitHub issues](https://img.shields.io/github/issues/phmarcel0x/pongduino)

### Requirements

#### Graphics
- **Paddle for Player One** - rendered as a white rectangle
- **Paddle for Player Two** - rendered as a white rectangle
- **Ball** - rendered as a white square
- **Net** - rendered as a dashed line
- **Score for each player** - rendered as a white number above each player’s zone

#### Control
- **WASD** controls Player One’s paddle (LEFT side of the window)
- **Arrow keys** control Player Two’s paddle (RIGHT side of the window)
- The paddles should not move beyond the bounds of the window

#### Interactions
To detect a collision between the ball and the paddles, we’ll make use of the **Separating Axis Theorem (SAT)**. The SAT, in simplified terms, says that if you can show that the projections of two objects onto an axis have a gap, then the objects are not colliding. It gets more complicated with rotations or in 3D, but for PONG, it's simple enough.

- Initially, the ball starts in the center of the window and shoots off to the RIGHT.

##### 2D Physics(ish)
- The ball should reflect off the TOP and BOTTOM of the window.
  - If the ball hits the TOP from the LEFT, it should reflect DOWNWARD and RIGHT.
  - If the ball hits the BOTTOM from the LEFT, it should reflect UPWARD and RIGHT.
  - Reflects similarly when hitting from the RIGHT side but in opposite horizontal directions.
- The ball should reflect off a player’s paddle:
  - In the TOP third, it should reflect UPWARD.
  - In the MIDDLE third, it should reflect back horizontally.
  - In the BOTTOM third, it should reflect DOWNWARD.
- The ball passes through the LEFT and the RIGHT sides of the window for scoring.

#### Scoring
- If the ball passes through the LEFT side (past Player One), point for Player Two.
- If the ball passes through the RIGHT side (past Player Two), point for Player One.
- After a score, the ball starts in the center and shoots off in the direction of the LOSING player.

#### ¿Arduino Joystick? (TBD)
- Normal joystick functionality.
