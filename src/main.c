
#include "raylib.h"
#include "stdio.h"
#include <math.h>
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

// Defines -------------------
#define NUM_FRAMES_PER_LINE 3
#define NUM_LINES 4
#define MAX_BALLS 2
#define GRAVITY 0.1f
#define VELOCITY 5
#define ELASTICITY 0.95f
#define BALL_SIZE 90

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
	int timer;
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
	Color color_temp;
	int id;
	char type; // s, m, l, x - small, medium, large, extra large
	
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

// static Texture2D gameOverWabbit = {0};

static Character chungus = {0};

static Character projectile = {0};

static Ball ball[MAX_BALLS] = {0};
//static Ball sBall[(MAX_BALLS * 2)] = {0};
static Ball sBall[100] = {0};
static int sBall_index = 0; // number of s balls

static Shot shot = {0};

static Wall wall_ceiling = {{0, 0, screenWidth, 15}, GRAY};
static Wall wall_floor = {{0, 730, screenWidth, 60}, GRAY};
static Wall wall_left = {{0, -1000, 15, screenHeight + 985}, GRAY};
static Wall wall_right = {{screenWidth - 15, -1000, 15, screenHeight + 985}, GRAY};

bool gameOver = false;
bool split = false;
int split_clock = 0;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitEngine(void);	   // Initalize game engine - run once
static void InitGame(void);		   // Initialize game
static void UpdateGame(void);	   // Update game (one frame)
static void DrawGame(void);		   // Draw game (one frame)
static void UnloadGame(void);	   // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)
void HitLargeBall(Ball *ball);
void CreateNewBall(Ball *ball, char type);
void ResolveElasticCollision(Ball *b1, Ball *b2);
void KeyPressHandler(void);
void GameOverState(void);
void UpdateBalls(void);
void UpdateProjectile(void);
void CheckBallProjectileCollision(void);
void initSprite(Character * character);
void updateSprite(Character * character);

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
		shot.box = (Rectangle){0, 0, 0, 0};
		shot.height = 0;
		shot.allowed = true;
		shot.timer = 0;

		//----BALL----- Init big balls
		for (int i = 0; i < MAX_BALLS; i++)
		{
			printf("init big ball: \n");
			// printf("ball_size: %d\n", BALL_SIZE);
			ball[i].size = BALL_SIZE;
			ball[i].box = (Rectangle){GetRandomValue(100, 1000), GetRandomValue(0, 400), ball[i].size, ball[i].size}; //(x,y)
			ball[i].speed = (Vector2){(GetRandomValue(-VELOCITY, VELOCITY) + 1) / 2.3, GetRandomValue(-8, 0)};		  // 4,-4
			ball[i].color = (Color){GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(250, 255)};
			ball[i].id = i + 10; // m balls have + 10 id ofset
			ball[i].type = 'm';
			ball[i].active = true;
		}

		if (false)
		{ //----sBALL----- Init small balls
			for (int i = 0; i < (MAX_BALLS * 2); i++)
			{
				sBall[i].size = BALL_SIZE / 2;
				// printf("ball_size: %d\n", sBall[i].size);
				//sBall[i].box = (Rectangle){GetRandomValue(100, 1000), GetRandomValue(0, 400), sBall[i].size, sBall[i].size}; //(x,y)
				sBall[i].box = (Rectangle){0, 0, sBall[i].size, sBall[i].size};
				sBall[i].speed = (Vector2){(GetRandomValue(-VELOCITY, VELOCITY) + 1) / 2.3, GetRandomValue(-8, 0)};			 // 4,-4
				sBall[i].color = (Color){GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(250, 255)};
				sBall[i].id = i;
				sBall[i].type = 's';
				sBall[i].active = false;
				printf("init small ball: \n");
			}
		}
		gameOver = false;
	}

	// update one frame of the game
	void UpdateGame(void)
	{
		KeyPressHandler();
		GameOverState();
		updateSprite(&chungus);
		UpdateProjectile();
		UpdateBalls();
	}

	void KeyPressHandler()
	{
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

		if (IsKeyPressed('G')) // simulate ball hit split
		{
			split = true;
			// split ball hit into smaller ball.
			// 1. just make the ball turn a different color - red.
		}
	}

	void GameOverState()
	{
		// Update chungus on end screen
		if (gameOver)
		{
			if (endWabbit.position.y >= 150)
			{
				endWabbit.position.y -= 30.0f;
			}
		}
	}

	void UpdateProjectile()
	{
		// -----SHOT UPDATE--------
		if (shot.timer < 1000)
		{
			if (shot.active == true)
			{
				if (shot.allowed == true)
				{
					shot.starting = (Vector2){chungus.position.x + 70, chungus.position.y + 140};
					projectile.position.x = chungus.position.x + 57;
					projectile.position.y = chungus.position.y + 120;
				}
				shot.allowed = false;
				shot.height += 7.0f;
			}

			if (shot.height > 720)
			{
				shot.allowed = true;
				shot.active = false;
				shot.height = 0;
			}

			// ------- Projectile Update -------
			shot.box = (Rectangle){shot.starting.x, shot.starting.y - shot.height, 7, shot.height};
			projectile.box = (Rectangle){projectile.position.x, projectile.position.y, projectile.texture.width, projectile.texture.height};
			projectile.position.y -= 7.0f;
		}
	}

	void CheckBallProjectileCollision()
	{
		for (int i = 0; i <= MAX_BALLS; i++)
		{
			if (CheckCollisionRecs(projectile.box, ball[i].box) || CheckCollisionRecs(shot.box, ball[i].box))
			{
				HitLargeBall(&ball[i]);
			}
		}

		for (int i = 0; i <= MAX_BALLS * 2; i++)
		{
			if (CheckCollisionRecs(projectile.box, sBall[i].box) || CheckCollisionRecs(shot.box, sBall[i].box))
			{
				sBall[i].collision = true;
				sBall[i].active = false;
				projectile.collision = true;
				sBall[i].box = (Rectangle){-100, -100, sBall[i].size, sBall[i].size};
				// sBall[i].speed = (Vector2){GetRandomValue(-3, 3), GetRandomValue(-8, 0)};
				// sBall[i].box = (Rectangle){GetRandomValue(100, 1000), GetRandomValue(0, 400), sBall[i].size, sBall[i].size};
			}
		}
	}


	// needs a global ball index each type of ball. 
	// put in a type of ball and then get a new one to add to the ball array
	void CreateNewBall(Ball *ball, char type)
	{
		if (type == 's')
		{
			sBall[sBall_index].size = BALL_SIZE / 2;
			sBall[sBall_index].box = (Rectangle){ball->box.x, ball->box.y, sBall[sBall_index].size, sBall[sBall_index].size};
			sBall[sBall_index].speed = (Vector2){(GetRandomValue(-VELOCITY, VELOCITY) + 1) / 2.3, GetRandomValue(-8, 0)}; // 4,-4
			sBall[sBall_index].color = (Color){GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(250, 255)};
			sBall[sBall_index].id = sBall_index;
			sBall[sBall_index].type = 's';
			sBall[sBall_index].active = true;
			printf("init small ball: \n");
			sBall_index++; // increment sball index
		}
	}

	void HitLargeBall(Ball *ball){
		//1 large is hit. set the location of two small balls to be its locations. set upwards motion for them and delete the large ball. 
		ball->active = false;

		shot.active = false;
		shot.timer++;
		printf("shot timer: %d",shot.timer);

		// 2. find center of the box and have two new boxes spring from it. 
		CreateNewBall(ball, 's');
		CreateNewBall(ball, 's');
		ball->box = (Rectangle){-100, -100, ball->size, ball->size}; // put off screen

		// reset shot

	}

	Ball UpdateBall(Ball single_ball)
	{
		if (single_ball.active == true)
		{
			if (single_ball.size < 90)
			{ // is small ball

				single_ball.speed.y += GRAVITY * 0.7;
				single_ball.box.y += single_ball.speed.y * 0.7;
				single_ball.box.x += single_ball.speed.x * 0.7;
			}
			else
			{
				// update movement and gravity
				// printf("speed y: %d\n",single_ball.speed.y);
				single_ball.speed.y += GRAVITY;
				single_ball.box.y += single_ball.speed.y;
				single_ball.box.x += single_ball.speed.x;
			}

			if (CheckCollisionRecs(single_ball.box, wall_floor.box)) // wall_floor
			{
				single_ball.speed.y = -single_ball.speed.y * ELASTICITY;
				single_ball.box.y = wall_floor.box.y - single_ball.box.height;
			}
			if (CheckCollisionRecs(single_ball.box, wall_left.box)) // wall_leftt
			{
				single_ball.speed.x = -single_ball.speed.x * ELASTICITY;
				single_ball.box.x = wall_left.box.x + wall_left.box.width;
			}
			if (CheckCollisionRecs(single_ball.box, wall_right.box)) // wall_right
			{
				single_ball.speed.x = -single_ball.speed.x * ELASTICITY;
				single_ball.box.x = wall_right.box.x - single_ball.box.width;
			}

			for (int i = 0; i < MAX_BALLS; i++)
			{
				if (single_ball.id != ball[i].id)
				{
					if (CheckCollisionRecs(single_ball.box, ball[i].box)) // check for colide with other balls if not itself
					{
						ResolveElasticCollision(&single_ball, &ball[i]);
					}
				}
			}

			// small ball collisions
			for (int i = 0; i < MAX_BALLS * 2; i++)
			{
				if (single_ball.id != sBall[i].id)
				{
					if (CheckCollisionRecs(single_ball.box, sBall[i].box)) // check for colide with other balls if not itself
					{
						ResolveElasticCollision(&single_ball, &ball[i]);
					}
				}
			}
		} else { 
			single_ball.position.x = 0; 
			single_ball.position.y = 0;
		}

		return single_ball;
	}

	// testing this fucntion from chatgpt - don't want to use it, want to create my own after quick fix
	void ResolveElasticCollision(Ball * b1, Ball * b2)
	{
		// 1. find difference in positions.
		Vector2 diff = {b2->box.x - b1->box.x, b2->box.y - b1->box.y};
		float distance = sqrtf(diff.x * diff.x + diff.y * diff.y); // sqrt(difx^2 + dify^2)
		Vector2 normal = {diff.x / distance, diff.y / distance};   // normalize the vector
																   // Compute relative velocity
		Vector2 relativeVel = {b2->speed.x - b1->speed.x, b2->speed.y - b1->speed.y};

		// Compute velocity along the normal direction
		float velAlongNormal = relativeVel.x * normal.x + relativeVel.y * normal.y;

		// Ignore collisions if velocities are separating
		if (velAlongNormal > 0)
			return;

		// Compute new velocities using elastic collision formula
		float elasticity = ELASTICITY; // Can be adjusted per material properties

		float impulse = (-(1 + elasticity) * velAlongNormal) / 2; // Assuming equal mass for simplicity

		Vector2 impulseVec = {impulse * normal.x, impulse * normal.y};

		// Apply impulse to both balls
		b1->speed.x -= impulseVec.x;
		b1->speed.y -= impulseVec.y;
		b2->speed.x += impulseVec.x;
		b2->speed.y += impulseVec.y;

		// **Separate the balls to prevent overlap**
		float overlap = (b1->size / 2 + b2->size / 2) - distance; // Calculate overlap distance
		if (overlap > 0)
		{
			Vector2 separation = {normal.x * overlap / 2, normal.y * overlap / 2};
			b1->box.x -= separation.x;
			b1->box.y -= separation.y;
			b2->box.x += separation.x;
			b2->box.y += separation.y;
		}
	}

	void UpdateBalls(void)
	{
		CheckBallProjectileCollision();

		for (int i = 0; i < MAX_BALLS; i++)
		{
			ball[i] = UpdateBall(ball[i]);
		}

		for (int i = 0; i < (MAX_BALLS * 2); i++)
		{
			sBall[i] = UpdateBall(sBall[i]);
		}

		// if (split == true)
		// {
		// 	if (split == true && split_clock == 0)
		// 	{ // keep track of the original ball color
		// 		printf("change color temp, split_clock: %d\n", split_clock);
		// 		for (int i = 0; i <= MAX_BALLS; i++)
		// 		{
		// 			printf("colors: r %d", ball[i].color_temp.r);
		// 			printf(" g  %d", ball[i].color_temp.g);
		// 			printf(" b %d\n", ball[i].color_temp.b);
		// 			ball[i].color_temp = ball[i].color;
		// 		}
		// 	}

		// 	split_clock += 1;
		// 	// printf("main split loop split_clock: %d\n", split_clock);
		// 	if (split_clock > 50)
		// 	{
		// 		printf("reset, split_clock: %d\n", split_clock);
		// 		split = false;
		// 		for (int i = 0; i <= MAX_BALLS; i++)
		// 		{
		// 			printf("colors: r %d", ball[i].color_temp.r);
		// 			printf(" g  %d", ball[i].color_temp.g);
		// 			printf(" b %d\n", ball[i].color_temp.b);
		// 			ball[i].color = ball[i].color_temp; // put color back after they turn red for a second
		// 		}
		// 		split_clock = 0;
		// 	}

		// 	for (int i = 0; i <= MAX_BALLS; i++)
		// 	{
		// 		ball[i].color = (Color){255, 0, 0, 255};
		// 		ball[i].speed.y = 0; //+= GRAVITY;
		// 							 // ball[i].speed.x = 0; //+= GRAVITY;
		// 							 // ball[i].box.y += 0;// ball[i].speed.y;
		// 							 // ball[i].box.x += 0;//ball[i].speed.x;
		// 	}
		// }

		// check for collision with player - end game
	}

	void DrawGame(void)
	{
		BeginDrawing();

		// Setup the backbuffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font

		// DrawText("hello beautiful wabbit", 20, 200, 20, RED);

		// draw our texture to the screen
		DrawRectangleRec(shot.box, RED);
		DrawTexture(projectile.texture, projectile.position.x, projectile.position.y, WHITE);

		DrawRectangleRec(wall_floor.box, wall_floor.color);
		// DrawRectangleRec(wall_ceiling.box, wall_ceiling.color);
		DrawRectangleRec(wall_left.box, wall_left.color);
		DrawRectangleRec(wall_right.box, wall_right.color);

		// draw chungus:
		DrawTextureRec(chungus.texture, chungus.sprite.frameRec, chungus.position, WHITE);
		// DrawTexture(gameOverWabbit, 100, 100, WHITE);

		// DrawTexture(endWabbit.texture, endWabbit.position.x, endWabbit.position.y,WHITE);

		// DrawRectangleRec((Rectangle){100,100,100,100},WHITE);

		for (int i = 0; i <= MAX_BALLS; i++)
		{
			DrawRectangleRec(ball[i].box, ball[i].color);
		}
		for (int i = 0; i <= MAX_BALLS * 2; i++)
		{
			DrawRectangleRec(sBall[i].box, sBall[i].color);
		}

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// Init a sprite - pass in a &refrence for the character and will init the sprite
	void initSprite(Character * character)
	{
		character->sprite.frameHeight = (float)(character->texture.height / NUM_LINES);
		character->sprite.frameWidth = (float)(character->texture.width / NUM_FRAMES_PER_LINE); // Sprite one frame rectangle width
		character->sprite.currentFrame = 0;
		character->sprite.currentLine = 0;
		character->sprite.frameRec = (Rectangle){0, 0, character->sprite.frameWidth, character->sprite.frameHeight};
		character->sprite.framesCounter = 0;
	}

	void updateSprite(Character * character)
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
