/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script 

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event 
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial 
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you 
--  wrote the original software. If you use this software in a product, an acknowledgment 
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1

int main ()
{
	const int screenWidth = 1200;
    const int screenHeight = 800;

	// Tell the window to use vysnc and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Hello Raylib");

	Texture2D chungus = LoadTexture("resources/chungus-sprite.png"); // sprite chungus png

	Texture wabbit = LoadTexture("Big-Chungus-PNG.png"); // Static large chungus png

	Vector2 chungusPosition = { (float)screenWidth/2, (float)screenHeight/2 };
	Vector2 wabbitPosition = { (float)screenWidth/2, (float)screenHeight/2 };

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	SetTargetFPS(60);  

	// Load a texture from the resources directory
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{

		// Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) wabbitPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) wabbitPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) wabbitPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) wabbitPosition.y += 2.0f;


		// drawing
		BeginDrawing();


		// Setup the backbuffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		DrawText("hello beautiful wabbit", 150,200,20,RED);

		// draw our texture to the screen
		DrawTexture(wabbit, wabbitPosition.x, wabbitPosition.y, WHITE);
		
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