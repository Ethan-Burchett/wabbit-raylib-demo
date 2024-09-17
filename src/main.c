
#include "raylib.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#define NUM_FRAMES_PER_LINE 3
#define NUM_LINES 4

int main()
{
	const int screenWidth = 1200;
	const int screenHeight = 800;

	// Tell the window to use vysnc and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Hello Raylib");

	// Load a texture from the resources directory
	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Init variables for animation
	// ----Chungus----
	Texture2D chungus = LoadTexture("chungus-sprite.png"); // sprite chungus png
	Vector2 chungusPosition = {(screenWidth/2)-50, 600};	//(x,y)(width,height)	   //{ (float)screenHeight - 10};	   //{(float)screenWidth / 2, (float)screenHeight / 2};
	float frameWidth = (float)(chungus.width / NUM_FRAMES_PER_LINE); // Sprite one frame rectangle width
	float frameHeight = (float)(chungus.height / NUM_LINES);		 // Sprite one frame rectangle height
	int currentFrame = 0;
	int currentLine = 0;
	Rectangle frameRec = {0, 0, frameWidth, frameHeight};
	int framesCounter = 0;

	// ----Wabbit----
	Texture wabbit = LoadTexture("Big-Chungus-PNG.png"); // Static large chungus png
	Vector2 wabbitPosition = {(float)screenWidth / 2, (float)screenHeight / 2};

	// ----Projectile----
	Texture2D projectile = LoadTexture("wabbit_alpha.png");
	Vector2 projectilePosition = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	

	SetTargetFPS(20);
	// game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// Update
		//----------------------------------------------------------------------------------
		if (IsKeyDown(KEY_RIGHT)){
			chungusPosition.x += 6.0f;
		}
		if (IsKeyDown(KEY_LEFT)){
			chungusPosition.x -= 6.0f;
		}
		// if (IsKeyDown(KEY_UP)){
		// 	chungusPosition.y -= 2.0f;
		// }
		// if (IsKeyDown(KEY_DOWN)){
		// 	chungusPosition.y += 2.0f;
		// }

		// Animate Sprite
		//----------------------------------------------------------------------------------

		// Iterate over sprite frames - using FPS - I think the game loop has one loop per frame - not 100% though
		framesCounter++;
		if (framesCounter > 2)
		{
			currentFrame++;

			if (currentFrame >= NUM_FRAMES_PER_LINE)
			{
				currentFrame = 0;
				currentLine++;

				if (currentLine >= NUM_LINES)
				{
					currentLine = 0;
				}
			}
			framesCounter = 0; // this is when it restarts and loops back around
		}

		frameRec.x = frameWidth * currentFrame; // need to choose a frame over the png for each frame to display
		frameRec.y = frameHeight * currentLine;

		// drawing
		BeginDrawing();

		// Setup the backbuffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		DrawText("hello beautiful wabbit", 20, 200, 20, RED);

		// draw our texture to the screen
		DrawTexture(wabbit, 50, 50, WHITE);

		DrawTextureRec(chungus, frameRec, chungusPosition, WHITE);

		DrawTexture(projectile, 50, 50, WHITE);

		// // draw our texture to the screen
		// DrawTexture(chungus, chungusPosition.x, chungusPosition.y, WHITE);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destory the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}