
#include "raylib.h"
#include "stdio.h"
#include <math.h>
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

// Defines -------------------
#define NUM_FRAMES_PER_LINE 3
#define NUM_LINES 4
#define MAX_BALLS 5
#define GRAVITY 0.2f
#define ELASTICITY 1.01f

typedef struct Sprite
{
	float frameHeight;
	float frameWidth;
	int currentFrame;
	int currentLine;
	int num_frames_per_line;
	int num_lines;
	Rectangle frameRec;
	int framesCounter;
	int tick;
} Sprite;

typedef struct Character
{
	Vector2 position;
	Vector2 speed;
	Vector2 starting;
	bool collision;
	bool active;
	Texture2D texture;
	Sprite sprite;
	Rectangle box;
} Character;

typedef struct Shot
{
	Vector2 starting;
	bool collision;
	bool active;
	bool allowed;
	int height;
	Rectangle box;
} Shot;

typedef struct Ball
{
	Vector2 position;
	Vector2 speed;
	bool active;
	bool collision;
	Rectangle box;
	int size;
	Color color;
} Ball;

typedef struct Wall
{
	Rectangle box;
	Color color;
} Wall;

// Globals -------------------------------------------------------------
const int screenWidth = 1200;
const int screenHeight = 800;

static Character endWabbit = {0};

//static Texture2D gameOverWabbit = {0};

static Character chungus = {0};

static Character projectile = {0};

static Ball ball[MAX_BALLS] = {0};

static Shot shot = {0};

static Wall wall_ceiling = {{0,0, screenWidth,15},GRAY};
static Wall wall_floor = {{0,730, screenWidth, 60}, GRAY};
static Wall wall_left = {{0, -1000, 15, screenHeight+985}, GRAY};
static Wall wall_right = {{screenWidth - 15, -1000, 15, screenHeight + 985}, GRAY};

bool gameOver = false;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitEngine(void);	   // Initalize game engine - run once
static void InitGame(void);		   // Initialize game
static void UpdateGame(void);	   // Update game (one frame)
static void DrawGame(void);		   // Draw game (one frame)
static void UnloadGame(void);	   // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)
void UpdateBalls(void);
void initSprite(Character *character);
void updateSprite(Character *character);

int main()
{
	InitEngine();

	InitGame();

	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		UpdateGame();

		DrawGame();
	}

	UnloadGame();

	return 0;
}

void InitEngine(void)
{
	// Tell the window to use vysnc and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "w a b b i t");

	// Load a texture from the resources directory
	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load textures ----------
	endWabbit.texture = LoadTexture("Big-Chungus-PNG.png");
	chungus.texture = LoadTexture("chungus-sprite.png");
	projectile.texture = LoadTexture("wabbit_alpha.png");

	SetTargetFPS(60);
}

// Initialize game
// Run each time on game over and reset
void InitGame(void)
{
	//---endWabbit ---------
	endWabbit.position = (Vector2){60, 1000};

	//---chungus (player) ------------
	initSprite(&chungus);
	chungus.position = (Vector2){(screenWidth / 2) - 50, 600};
	chungus.collision = false;

	//---projectile-----
	projectile.position = (Vector2){1500, 1};
	
	//---shot-----------
	shot.active = false;
	shot.box = (Rectangle){0,0,0,0};
	shot.height = 0;
	shot.allowed = true;

	//----BALL-----
	for (int i = 1; i <= MAX_BALLS; i++)
	{
		ball[i].size = 90;
		ball[i].box = (Rectangle){GetRandomValue(100, 1000), GetRandomValue(0, 400), ball[i].size, ball[i].size}; //(x,y)
		ball[i].speed = (Vector2){GetRandomValue(-3, 3), GetRandomValue(-8, 0)};
		ball[i].color = (Color){GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(150, 255)};
	}
		gameOver = false;

	//----WALL-----
	
}

// update one frame of the game
void UpdateGame(void)
{
	// Update
	//----------------------------------------------------------------------------------
	if (IsKeyDown(KEY_RIGHT))
	{
		if (!chungus.collision && (chungus.position.x <= screenWidth - 150)) // prevents going off screen
		{
			chungus.position.x += 5.0f;
		}
		else
		{
			chungus.position.x += 1.0f; // move slowly on game end
		}
	}
	if (IsKeyDown(KEY_LEFT))
	{
		if (!chungus.collision && (chungus.position.x >= 0))
		{
			chungus.position.x -= 5.0f;
		}
		else
		{
			chungus.position.x -= 1.0f;
		}
	}

	if (IsKeyPressed(KEY_ENTER))
	{
		// gameOver = true;
		InitGame(); // restarts game
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		if (!chungus.collision)
		{
			shot.active = true;
		}
	}

	if (IsKeyPressed('E')) // simulate game end
	{
		gameOver = true;
		chungus.collision = true;
	}

	//Update chungus on end screen
	if (gameOver)
	{
		if (endWabbit.position.y >= 150)
		{
			endWabbit.position.y -= 30.0f;
		}
	}

	// -----SHOT UPDATE--------
	if(shot.active == true)
	{
		if(shot.allowed == true)
		{
			shot.starting = (Vector2){chungus.position.x + 70, chungus.position.y + 140};
			projectile.position.x = chungus.position.x + 57;
			projectile.position.y = chungus.position.y + 120;
		}
		shot.allowed = false;
		shot.height +=7.0f;
	}

	if(shot.height > 720)
	{
		shot.allowed = true;
		shot.active = false;
		shot.height = 0;
	}

	shot.box = (Rectangle){shot.starting.x, shot.starting.y - shot.height, 7, shot.height};

	// ------- Projectile Update -------
	projectile.box = (Rectangle){projectile.position.x, projectile.position.y, projectile.texture.width, projectile.texture.height};
	projectile.position.y -= 7.0f;


	// updateSpriteChungus();
	updateSprite(&chungus);

	// check collision
	for(int i = 0; i <= MAX_BALLS; i++)
	{
		if (CheckCollisionRecs(projectile.box,ball[i].box))
		{
			ball[i].collision = true;
			projectile.collision = true;
			ball[i].speed = (Vector2){GetRandomValue(-3, 3), GetRandomValue(-8, 0)};
			ball[i].box = (Rectangle){GetRandomValue(100, 1000), GetRandomValue(0, 400), ball[i].size, ball[i].size};
		}

		if (CheckCollisionRecs(shot.box,ball[i].box))
		{
			ball[i].collision = true;
			ball[i].speed = (Vector2){GetRandomValue(-3, 3), GetRandomValue(-8, 0)};
			ball[i].box = (Rectangle){GetRandomValue(100, 1000), GetRandomValue(0, 400), ball[i].size, ball[i].size};
		}
	}

	UpdateBalls();

}

void UpdateBalls(void)
{
	// ---- update direction

	// --- update gravity
	for (int i = 0; i <= MAX_BALLS; i++)
	{
		ball[i].speed.y += GRAVITY;
		ball[i].box.y += ball[i].speed.y ;
		ball[i].box.x += ball[i].speed.x;
	}

	// check for collision with walls
	for (int i = 0; i <= MAX_BALLS; i++)
	{
		if (CheckCollisionRecs(ball[i].box,wall_floor.box)) // wall_floor
		{
			ball[i].speed.y = -ball[i].speed.y * ELASTICITY;
			ball[i].box.y = wall_floor.box.y - ball[i].box.height; 
			
		}
		if (CheckCollisionRecs(ball[i].box, wall_ceiling.box)) // wall_ceiling NOT WORKING - possibly ignore ceiling?? 
		{
			// ball[i].speed.y = -ball[i].speed.y * ELASTICITY;
			// ball[i].box.y = wall_ceiling.box.y - ball[i].box.height;
		}
		if (CheckCollisionRecs(ball[i].box, wall_left.box)) // wall_left
		{
			ball[i].speed.x = -ball[i].speed.x * ELASTICITY;
			//ball[i].box.y = wall_left.box.y - ball[i].box.height;
		}
		if (CheckCollisionRecs(ball[i].box, wall_right.box)) // wall_right
		{
			ball[i].speed.x = -ball[i].speed.x * ELASTICITY;
			// ball[i].box.y = wall_left.box.y - ball[i].box.height;
		}
	}

	// check for collision with player - end game
}

void DrawGame(void)
{
	BeginDrawing();

	// Setup the backbuffer for drawing (clear color and depth buffers)
	ClearBackground(BLACK);

	// draw some text using the default font


	DrawText("hello beautiful wabbit", 20, 200, 20, RED);

	// draw our texture to the screen
	DrawRectangleRec(shot.box, RED);
	DrawTexture(projectile.texture, projectile.position.x, projectile.position.y, WHITE);

	DrawRectangleRec(wall_floor.box,wall_floor.color);
	//DrawRectangleRec(wall_ceiling.box, wall_ceiling.color);
	DrawRectangleRec(wall_left.box, wall_left.color);
	DrawRectangleRec(wall_right.box, wall_right.color);

	DrawTextureRec(chungus.texture, chungus.sprite.frameRec, chungus.position, WHITE);
	//DrawTexture(gameOverWabbit, 100, 100, WHITE);

	//DrawTexture(endWabbit.texture, endWabbit.position.x, endWabbit.position.y,WHITE);

	//DrawRectangleRec((Rectangle){100,100,100,100},WHITE);

	for (int i = 0; i <= MAX_BALLS; i++){
		DrawRectangleRec(ball[i].box, ball[i].color);
	}

	// end the frame and get ready for the next one  (display frame, poll input, etc...)
	EndDrawing();
}

// Init a sprite - pass in a &refrence for the character and will init the sprite
void initSprite(Character *character)
{
	character->sprite.frameHeight = (float)(character->texture.height / NUM_LINES);
	character->sprite.frameWidth = (float)(character->texture.width / NUM_FRAMES_PER_LINE); // Sprite one frame rectangle width
	character->sprite.currentFrame = 0;
	character->sprite.currentLine = 0;
	character->sprite.frameRec = (Rectangle){0, 0, character->sprite.frameWidth, character->sprite.frameHeight};
	character->sprite.framesCounter = 0;
}

void updateSprite(Character *character)
{
	character->sprite.tick++;
	if (character->sprite.tick % 3 == 0)
		character->sprite.framesCounter++;
	if (character->sprite.framesCounter > 2)
	{
		character->sprite.currentFrame++;

		if (character->sprite.currentFrame >= NUM_FRAMES_PER_LINE)
		{
			character->sprite.currentFrame = 0;
			character->sprite.currentLine++;

			if (character->sprite.currentLine >= NUM_LINES)
			{
				character->sprite.currentLine = 0;
			}
		}
		character->sprite.framesCounter = 0; // this is when it restarts and loops back around
	}
	character->sprite.frameRec.x = character->sprite.frameWidth * character->sprite.currentFrame; // need to choose a frame over the png for each frame to display
	character->sprite.frameRec.y = character->sprite.frameHeight * character->sprite.currentLine;
}

// UnloadGame - Final Cleanup
void UnloadGame(void)
{
	UnloadTexture(endWabbit.texture);
	UnloadTexture(chungus.texture);
	UnloadTexture(projectile.texture);

	// destory the window and cleanup the OpenGL context
	CloseWindow();
}

