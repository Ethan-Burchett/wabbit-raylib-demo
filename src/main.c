
#include "raylib.h"
#include "stdio.h"
#include <math.h>
#include "resource_dir.h" // utility header for SearchAndSetResourceDir


// Defines -------------------
#define NUM_FRAMES_PER_LINE 3
#define NUM_LINES 4


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
} Sprite;

typedef struct Character
{
	Vector2 position;
	Vector2 speed;
	bool collision;
	Texture2D texture;
	Sprite sprite;
} Character;



// Globals -------------------------------------------------------------
const int screenWidth = 1200;
const int screenHeight = 800;

static Character endWabbit = {0};

static Character chungus = {0};

static Character projectile = {0};

bool gameOver = false;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitEngine(void);        // Initalize game engine - run once
static void InitGame(void);		     // Initialize game
static void UpdateGame(void);	     // Update game (one frame)
static void DrawGame(void);		     // Draw game (one frame)
static void UnloadGame(void);	     // Unload game
static void UpdateDrawFrame(void);   // Update and Draw (one frame)
void initSprite(Character *character);
void initSpriteChungus(void); // Init chungus sprite - future work to make this general
void updateSpriteChungus(void);

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

void InitEngine(void){
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

	SetTargetFPS(20);
}

// Initialize game 
// Run each time on game over and reset 
void InitGame(void){

	//---endWabbit ---------
	endWabbit.position = (Vector2){100,100};

	//---chungus (player) ------------
	// initSpriteChungus();
	initSprite(&chungus);
	chungus.position = (Vector2){(screenWidth / 2) - 50, 600};

	//---projectile-----
	projectile.position = (Vector2){1000,1000};
	gameOver = false;
}

// update one frame of the game 
void UpdateGame(void)
{
	// Update
	//----------------------------------------------------------------------------------
	if (IsKeyDown(KEY_RIGHT))
	{
		if (chungus.position.x <= screenWidth - 150) // prevents going off screen
		{
			chungus.position.x += 20.0f;
		}
	}
	if (IsKeyDown(KEY_LEFT))
	{
		if (chungus.position.x >= 0)
		{
			chungus.position.x -= 20.0f;
		}
	}

	if (IsKeyPressed(KEY_ENTER))
	{
		InitGame(); // restarts game
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		projectile.position.x = chungus.position.x + 58;
		projectile.position.y = chungus.position.y;
	}

	// Update chungus on end screen
	if (gameOver)
	{
		if (endWabbit.position.y >= 150)
		{
			endWabbit.position.y -= 30.0f;
		}
	}

	projectile.position.y -= 20.0f;

	updateSpriteChungus();
	
}

void DrawGame(void)
{
	BeginDrawing();

	// Setup the backbuffer for drawing (clear color and depth buffers)
	ClearBackground(BLACK);

	// draw some text using the default font

	DrawText("hello beautiful wabbit", 20, 200, 20, RED);

	// draw our texture to the screen
	DrawTexture(endWabbit.texture, endWabbit.position.x, endWabbit.position.y, WHITE);

	DrawTextureRec(chungus.texture, chungus.sprite.frameRec, chungus.position, WHITE);

	DrawTexture(projectile.texture, projectile.position.x, projectile.position.y, WHITE);

	// end the frame and get ready for the next one  (display frame, poll input, etc...)
	EndDrawing();
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

void initSpriteChungus(void)
{
	chungus.sprite.frameHeight = (float)(chungus.texture.height / NUM_LINES);		  // Sprite one frame rectangle height
	chungus.sprite.frameWidth = (float)(chungus.texture.width / NUM_FRAMES_PER_LINE); // Sprite one frame rectangle width
	chungus.sprite.currentFrame = 0;
	chungus.sprite.currentLine = 0;
	chungus.sprite.frameRec = (Rectangle){0, 0, chungus.sprite.frameWidth, chungus.sprite.frameHeight};
	chungus.sprite.framesCounter = 0;
}

void initSprite(Character * character)
{
	character->sprite.frameHeight = (float)(character->texture.height / NUM_LINES);
	character->sprite.frameWidth = (float)(character->texture.width / NUM_FRAMES_PER_LINE); // Sprite one frame rectangle width
	character->sprite.currentFrame = 0;
	character->sprite.currentLine = 0;
	character->sprite.frameRec = (Rectangle){0, 0, character->sprite.frameWidth, character->sprite.frameHeight};
	character->sprite.framesCounter = 0;
}



void updateSpriteChungus(void)
{
	chungus.sprite.framesCounter++;
	if (chungus.sprite.framesCounter > 2)
	{
		chungus.sprite.currentFrame++;

		if (chungus.sprite.currentFrame >= NUM_FRAMES_PER_LINE)
		{
			chungus.sprite.currentFrame = 0;
			chungus.sprite.currentLine++;

			if (chungus.sprite.currentLine >= NUM_LINES)
			{
				chungus.sprite.currentLine = 0;
			}
		}
		chungus.sprite.framesCounter = 0; // this is when it restarts and loops back around
	}
	chungus.sprite.frameRec.x = chungus.sprite.frameWidth * chungus.sprite.currentFrame; // need to choose a frame over the png for each frame to display
	chungus.sprite.frameRec.y = chungus.sprite.frameHeight * chungus.sprite.currentLine;
}