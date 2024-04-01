#include <iostream>
#include <vector>
#include <cmath>
#include <SDL.h>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const int FOV = 75;
const int PITCH = 50;
const int WALL_HEIGHT = 600;

Uint32 SCREEN_BUFFER[SCREEN_HEIGHT][SCREEN_WIDTH];

struct Player
{
	float x = 1.5f;
	float y = 1.5f;
	float angle = 90;
	float movementSpeed = 0.03f;
	float rotationSpeed = 1.0f;
};

struct Map
{
	const int size = 16;
	std::vector<int> layout =
	{
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};
};

struct ColorPalette
{
	const static Uint32 wall		= 0x777777;
	const static Uint32 darkWall	= 0x707070;
	const static Uint32 floor		= 0x202020;
	const static Uint32 ceiling		= 0x161616;
};

int main(int argc, char* argv[])
{
	// Game components
	Player player;
	Map map;

	// SDL components
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* screenTexture = NULL;

	/* SDL INITIALIZATION PROCESS:
	*	1. Start the SDL subsystems
	*	2. Assign the SDL componentes
	*/
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL couldn't be initialized.\n ERROR: %s\n", SDL_GetError();
		exit(-1);
	}

	window = SDL_CreateWindow("Raycast Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
	if (window == NULL)
	{
		std::cout << "An error occurred while creating the window.\n ERROR: %s\n", SDL_GetError();
		exit(-1);
	}

	renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		std::cout << "An error occurred while creating the renderer.\n ERROR: %s\n", SDL_GetError();
		exit(-1);
	}

	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (screenTexture == NULL)
	{
		std::cout << "An error occurred while creating the Raycast Texture.\n ERROR: %s\n", SDL_GetError();
		exit(-1);
	}

	/* HANDLE THE RUNTIME EVENTS
	*	1. Handle the SDL events:
	*		1.1 Check when the user press on Quit
	*	2. Handle the RUNTIME game logic:
	*		2.1. Handle the keyboard input to apply movement and rotation to the player class
	*	3. Render the game graphics
	*		3.1. Refresh the rendering
	*		3.2. Render the Floor and Ceiling
	*		3.3. Render the First Person View
	*/
	SDL_Event e = {};
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				break;
			}
		}

		const Uint8* key = SDL_GetKeyboardState(NULL);
		if (key[SDL_SCANCODE_UP])
		{
			float targetX = player.x + player.movementSpeed * cos(player.angle * M_PI / 180);
			float targetY = player.y + player.movementSpeed * sin(player.angle * M_PI / 180);
			if (map.layout[(int)targetX + map.size * (int)targetY] == 0)
			{
				player.x = targetX;
				player.y = targetY;
			}
		}
		if (key[SDL_SCANCODE_DOWN])
		{
			float targetX = player.x - player.movementSpeed * cos(player.angle * M_PI / 180);
			float targetY = player.y - player.movementSpeed * sin(player.angle * M_PI / 180);
			if (map.layout[(int)targetX + map.size * (int)targetY] == 0)
			{
				player.x = targetX;
				player.y = targetY;
			}
		}
		if (key[SDL_SCANCODE_LEFT])
		{
			player.angle -= player.rotationSpeed;
		}
		if (key[SDL_SCANCODE_RIGHT])
		{
			player.angle += player.rotationSpeed;
		}

		// Clear the screen buffer
		memset(SCREEN_BUFFER, 0, sizeof(SCREEN_BUFFER));

		/* RAYCAST RENDERING LOOP
		*	1. Shoot a ray foreach pixel of the width of the screen
		*	2. Calculate angle of the current ray being shot
		*	3. Apply a DDA ALGORITHM to check map intersection
		*	4. When finding a tile, calculates the distance between the player and the wall
		*	5. Draw a column at the specific screen position based on the distance of the player and the wall
		*/
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			float rayPosX = player.x;
			float rayPosY = player.y;
			float angle = (player.angle - FOV / 2.0f) + ((float)x / SCREEN_WIDTH) * FOV;
			float rayDirX = cos(angle * M_PI / 180);
			float rayDirY = sin(angle * M_PI / 180);

			float stepSizeX = sqrt(1 + (rayDirY / rayDirX) * (rayDirY / rayDirX));
			float stepSizeY = sqrt(1 + (rayDirX / rayDirY) * (rayDirX / rayDirY));
			int stepDirX;
			int stepDirY;

			int mapX = rayPosX;
			int mapY = rayPosY;

			float rayLengthX;
			float rayLengthY;
			if (rayDirX < 0)
			{
				stepDirX = -1;
				rayLengthX = (rayPosX - float(mapX)) * stepSizeX;
			}
			else
			{
				stepDirX = 1;
				rayLengthX = (float(mapX + 1) - rayPosX) * stepSizeX;
			}

			if (rayDirY < 0)
			{
				stepDirY = -1;
				rayLengthY = (rayPosY - float(mapY)) * stepSizeY;
			}
			else
			{
				stepDirY = 1;
				rayLengthY = (float(mapY + 1) - rayPosY) * stepSizeY;
			}

			bool hit = false;
			bool hitSide;
			float distance = 0;
			while (!hit && distance < map.size)
			{
				if (rayLengthX < rayLengthY)
				{
					mapX += stepDirX;
					distance = rayLengthX;
					rayLengthX += stepSizeX;
					hitSide = false;
				}
				else
				{
					mapY += stepDirY;
					distance = rayLengthY;
					rayLengthY += stepSizeY;
					hitSide = true;
				}

				if (mapX >= 0 && mapX < map.size && mapY >= 0 && mapY < map.size)
				{
					int tileValue = map.layout[(int)mapX + map.size * (int)mapY];
					if (tileValue > 0)
					{
						hit = true;

						float height	= WALL_HEIGHT / distance;
						int drawStart	= -height / 2 + WALL_HEIGHT / 2 + PITCH;
						int drawEnd		= height / 2 + WALL_HEIGHT / 2 + PITCH;
						for (int y = 0; y < SCREEN_HEIGHT - 1; y++)
						{
							if (y < drawStart)					SCREEN_BUFFER[y][x] = ColorPalette::ceiling;
							if (y >= drawStart && y < drawEnd)	SCREEN_BUFFER[y][x] = hitSide ? ColorPalette::wall : ColorPalette::darkWall;
							if (y >= drawEnd)					SCREEN_BUFFER[y][x] = ColorPalette::floor;
						}
					}
				}
			}
		}

		SDL_UpdateTexture(screenTexture, NULL, SCREEN_BUFFER, SCREEN_WIDTH * sizeof(unsigned int));
		SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	renderer = NULL;
	screenTexture = NULL;
	window = NULL;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(screenTexture);
	SDL_Quit();

	return 0;
}