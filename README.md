# Pongduino

## PONG Requirements (brainstorm)
https://austinmorlan.com/posts/pong_clone/

### Graphics
	A paddle for Player One		-> rendered as a white rectangle
	A paddle for Player Two		-> rendered as a white rectangle
	A ball						-> rendered as a white square
	A net						-> rendered as a dashed line
	A score for each player		-> rendered as a white number above each player’s zone

### Control
	WASD control Player One’s paddle		: LEFT side of the window
	Arrow keys control Player Two’s paddle	: RIGHT side of the window
	The paddles should not move beyond the bounds of the window

### Interactions
	To detect a collision between the ball and the paddles, we’ll make use of the Separating Axis Theorem (SAT)**. 
	The SAT says (in simplified terms) that if you can show that the projections of two objects onto an axis have a gap, then the objects are not colliding.
	**It gets pretty complicated if your 2D objects are rotated or you’re working in 3D, but for PONG, it’s simpler.

	In the beginning, a ball starts in the center of the window and then shoots off to the RIGHT

	2D Physics(ish)
		The ball should reflect off of the TOP and BOTTOM of the window
		If the ball hits the TOP		from the LEFT,		it should reflect DOWNWARD and RIGHT
		If the ball hits the TOP		from the RIGHT,		it should reflect DOWNWARD and LEFT
		If the ball hits the BOTTOM		from the LEFT,		it should reflect UPWARD and RIGHT
		If the ball hits the BOTTOM		from the RIGHT,		it should reflect UPWARD and LEFT

		The ball should reflect off of a player’s paddle
		If the ball hits a paddle in the TOP third,			it should reflect UPWARD
		If the ball hits a paddle in the MIDDLE third,		it should reflect back horizontally
		If the ball hits a paddle in the botom third,		its should reflect DOWNWARD

	The ball should pass through the LEFT and the RIGHT sides of the window

### Scoring
	If the ball passes through the LEFT side of the window (past Player One),	=> point for Player Two
	If the ball passes through the RIGHT side of the window (past Player Two),	=> point for Player One
	After a score occurs, the ball should start in the center and shoot off in the direction of the LOSING player

### ¿Audio?
	If the ball hits a paddle, a sound effect should play.
	If the ball hits a wall, a sound effect should play.
