// main.c
// #include "SDL.h"
//#include "SDL.h"
#include <stdio.h>
#include <time.h>
#include "SDL.h"
#include "SDL_image.h"
#include <stdlib.h>

#define GRAVITY 0.35f

typedef struct
{
	float x, y;
	float dy, dx;
	short life;
	char* name;
	int onLedge;
} Man;
typedef struct
{
	int x, y, w, h;
} Ledge;
typedef struct
{
	int x, y;
} Star;
typedef struct
{
	//player
	Man man;

	//
	Ledge ledges[100];

	// stars
	Star stars[100];

	//Images
	SDL_Texture* star;
	SDL_Texture* manFrames[2];
	SDL_Texture* bricks;

	SDL_Renderer* renderer;
} GameState;

void process(GameState* gameState)
{
	Man* man = &gameState->man;
	man->y += man->dy;
	man->x += man->dx;
	man->dy += GRAVITY;
}
int processEvents(SDL_Window* window, GameState* gameState)
{
	int done = 0;
	SDL_Event event;
	// Check Events
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_WINDOWEVENT_CLOSE:
		{
			if (window)
			{
				SDL_DestroyWindow(window);
				window = NULL;
				done = 1;
			}
		}
		break;
		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				done = 1;
				break;
			case SDLK_UP:
				if (gameState->man.onLedge)
				{
					gameState->man.dy = -8;
					gameState->man.onLedge = 0;
				}
				break;
			}


		}
		break;
		case SDL_QUIT:
			done = 1;
			break;
		default:
			done = 0;
			break;
		}
	}
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_RETURN])
	{
		printf("<Return> is pressed.\n");
	}
	if (state[SDL_SCANCODE_RIGHT])
	{
		gameState->man.dx += 0.5;
		if (gameState->man.dx > 6) {
			gameState->man.dx = 6;
		}
	}
	else if (state[SDL_SCANCODE_LEFT])
	{
		gameState->man.dx -= 0.5;
		if (gameState->man.dx < -6) {
			gameState->man.dx = -6;
		}
	}
	else
	{
		gameState->man.dx *= 0.8f;
		if (fabsf(gameState->man.dx) < 0.1f)
		{
			gameState->man.dx = 0;
		}
	}
	//if (state[SDL_SCANCODE_UP])
	//{
	//	gameState->man.y -= 10;

	//}
	//if (state[SDL_SCANCODE_DOWN])
	//{
	//	gameState->man.y += 10;
	//}
	return done;
}
int collide2d(float x1, float y1, float x2, float y2, float wt1, float ht1, float wt2, float ht2)
{
	return (!((x1 > (x2 + wt2)) || (x2 > (x1 + wt1)) || (y1 > (y2 + ht2)) || (y2 > (y1 + ht1))));
}

void doRender(GameState* gameState)
{
	SDL_Texture* star = gameState->star;
	SDL_Renderer* renderer = gameState->renderer;
	// set the drawing color to blue
	SDL_SetRenderDrawColor(renderer, 128, 128, 255, 255);

	//Clear the screen 
	SDL_RenderClear(renderer);

	//set the drawing color to white
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


	//Draw the start image
	for (int i = 0; i < 100; i++)
	{
		SDL_Rect ledgeRect = { gameState->ledges[i].x, gameState->ledges[i].y,gameState->ledges[i].w, gameState->ledges[i].h };
		SDL_RenderCopy(renderer, gameState->bricks, NULL, &ledgeRect);
	}

	//Draw the rectangle at man's position
	SDL_Rect rect = { gameState->man.x, gameState->man.y, 48, 48 };
	SDL_RenderCopyEx(renderer, gameState->manFrames[0], NULL, &rect, 0, NULL, 0);

	SDL_RenderPresent(renderer);
}
void loadGame(GameState* gameState)
{
	SDL_Surface* surface = NULL;

	// Load images

	surface = IMG_Load("star.png");
	if (surface == NULL)
	{
		printf("Cannot find star.png");
		SDL_Quit();
		exit(1);
	}
	gameState->star = SDL_CreateTextureFromSurface(gameState->renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("bricks.png");
	if (surface == NULL)
	{
		printf("Cannot find bricks.png");
		SDL_Quit();
		exit(1);
	}
	gameState->bricks = SDL_CreateTextureFromSurface(gameState->renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("Run.png");
	if (surface == NULL)
	{
		printf("Cannot find Run.png");
		SDL_Quit();
		exit(1);
	}
	gameState->manFrames[0] = SDL_CreateTextureFromSurface(gameState->renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("Run2.png");
	if (surface == NULL)
	{
		printf("Cannot find Run2.png");
		SDL_Quit();
		exit(1);
	}
	gameState->manFrames[1] = SDL_CreateTextureFromSurface(gameState->renderer, surface);
	SDL_FreeSurface(surface);

	gameState->man.x = 320 - 40;
	gameState->man.y = 240 - 40;
	gameState->man.dy = 0;
	gameState->man.dx = 0;
	gameState->man.onLedge = 0;

	// init stars
	for (int i = 0; i < 100; i++)
	{
		gameState->stars[i].x = i * rand() % 640;
		gameState->stars[i].y = i * rand() % 480;
	}

	// init ledges
	for (int i = 0; i < 100; i++)
	{
		gameState->ledges[i].w = 256;
		gameState->ledges[i].h = 64;
		gameState->ledges[i].x = i * 256;
		gameState->ledges[i].y = 400;
	}
	gameState->ledges[99].x = 350;
	gameState->ledges[99].y = 200;

	gameState->ledges[98].x = 350;
	gameState->ledges[98].y = 350;
}

void collisionDetect(GameState* game)
{
	//Check for collision with any ledges (brick blocks)
	for (int i = 0; i < 100; i++)
	{
		float mw = 48, mh = 48;
		float mx = game->man.x, my = game->man.y;
		float bx = game->ledges[i].x, by = game->ledges[i].y, bw = game->ledges[i].w, bh = game->ledges[i].h;

		if (mx + mw / 2 > bx && mx + mw / 2 < bx + bw) {
			// are we bumping our head?
			if (my<by + bh && my> by && game->man.dy < 0)
			{
				// correct y
				game->man.y = by + bh;
				my = by + bh;
				//bumped our head, stop any jump velocity
				game->man.dy = 0;
				game->man.onLedge = 1;
			}
			else if (my + mh > by && my < by && game->man.dy > 0)
			{
				// correct y
				game->man.y = by - mh;

				//landed on this edge, stop any jump velocity
				game->man.dy = 0;
				game->man.onLedge = 1;

			}
		}
		if (my + mh > by && my < by + bh)
		{
			// rubbing against right edge
			if (mx<bx + bw && mx + mw>bx + bw && game->man.dx < 0) {
				printf(" rubbing against right edge %f\n\n", game->man.x);

				// correct x
				game->man.x = bx + bw;
				mx = bx + bw;
				game->man.dx = 0;
			}
			//rubbing against left edge
			else if (mx + mw > bx && mx < bx && game->man.dx > 0) {
				printf(" rubbing against left edge %f\n\n", game->man.x);

				// correct x
				game->man.x = bx - mw;
				mx = bx - mw;
				game->man.dx = 0;
			}
		}
	}
}


int main(int argc, const char* argv[])
{
	GameState gameState;
	SDL_Window* window;					// Declare a window

	SDL_Init(SDL_INIT_VIDEO);			// Initialize SDL2

	// init random
	srand((int)time(NULL));

	// Create an aplication window with the following settings:
	window = SDL_CreateWindow(
		"Game Window",					// window title
		SDL_WINDOWPOS_UNDEFINED,		// initial x position
		SDL_WINDOWPOS_UNDEFINED,		// initial y position
		640,
		480,
		0
	);

	// Create renderer
	gameState.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Load images and create redering textures from them
	loadGame(&gameState);


	// The window is open: enter program loop (see SDL_PollEvent)
	int done = 0;

	// Event loop
	while (!done)
	{

		// Render display
		done = processEvents(&window, &gameState);
		process(&gameState);
		collisionDetect(&gameState);

		doRender(&gameState);
		//SDL_Delay(100);
	}

	//Shutdown game and unload  all memory
	SDL_DestroyTexture(gameState.star);

	//Close and destroy the window
	SDL_DestroyWindow(&window);
	SDL_DestroyRenderer(gameState.renderer);

	SDL_Quit();
	return 0;
};