#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board[row][column]
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

//dashes needed to draw the board
int dashes;

//number of tiles
int tiles;

//where the blank on the board currently is
int zerorow;
int zerocol;

/* Function prototypes */
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);
void pdash(int n);
void findzero(void);

int main(int argc, char *argv[]) {
	// greet user with instructions
	greet();

	// ensure proper usage
	if (argc != 2)
	{
		printf("Usage: %s d\n", argv[0]);
		return 1;
	}

	// ensure valid dimensions
	d = atoi(argv[1]);
	if (d < DIM_MIN || d > DIM_MAX)
	{
		printf("Board must be between %d x %d and %d x %d, inclusive.\n",
						DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
		return 2;
	}

	// initialize the board
	init();

	// accept moves until game is won
	while (true)
	{
		// clear the screen
		clear();

		// draw the current state of the board
		draw();

		// check for win
		if (won())
		{
			printf("ftw!\n");
			break;
		}

		// prompt for move
		printf("Tile to move: ");
		int tile = GetInt();

		// move if possible, else report illegality
		if (!move(tile))
		{
			printf("\nIllegal move.\n");
			usleep(500000);
		}

		// sleep thread for animation's sake
		usleep(500000);
	}

	// that's all folks
	return 0;
}

/*
 * Clears screen using ANSI escape sequences.
 */
void clear(void) {
	printf("\033[2J");
	printf("\033[%d;%dH", 0, 0);
}

/*
 * Greets player.
 */
void greet(void) {
	clear();
	printf("WELCOME TO THE GAME OF FIFTEEN\n");
	//usleep(2000000);
	usleep(700000);
}

/*
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).
 */
void init(void) {
	tiles = d * d - 1;
	int tile = tiles;
	for (int row = 0; row < d; row++)
	{
		for (int column = 0; column < d; column++)
		{
			board[row][column] = tile--;
		}
	}

	if (d % 2 == 0)
	{
		// reverse the '1' with the '2'
		board[d - 1][d - 3] = 1;
		board[d - 1][d - 2] = 2;
	}
	dashes = d * 5 + 1;
	zerorow = d - 1;
	zerocol = d - 1;
}

/*
 * Prints the board in its current state.
 */
void draw(void) {
	for (int row = 0; row < d; row++)
	{
		pdash(dashes);
		printf("|");
		for (int column = 0; column < d; column++)
		{
			if (0 == board[row][column])
				printf("    |");
			else
				printf("%3d |", board[row][column]);
		}
		printf("\n");
	}
	pdash(dashes);
}

/*
 * If tile borders empty space, moves tile and returns true, else
 * returns false.
 */
bool move(int tile) {
	//check if tile is withing possible range.
	if (!(tile > 0 && tile <= tiles))
		return false;

	if (board[zerorow][zerocol] != 0)
	{
		printf("\nERROR, I misplaced my empty space!\n");
		findzero();
		return false;
	}

	// look for the tile in the border tiles (ie around the blank space)
	//horizontally
	//printf("zerorow=%d,zerocol=%d\n", zerorow,zerocol);
	if (zerorow > 0 && board[zerorow - 1][zerocol] == tile)
	{
		board[zerorow - 1][zerocol] = 0;
		board[zerorow][zerocol] = tile;
		zerorow--;
		return true;
	}
	else if (zerorow < d - 1 && board[zerorow + 1][zerocol] == tile)
	{
		board[zerorow + 1][zerocol] = 0;
		board[zerorow][zerocol] = tile;
		zerorow++;
		return true;
	}
		//vertically
	else if (zerocol > 0 && board[zerorow][zerocol - 1] == tile)
	{
		board[zerorow][zerocol - 1] = 0;
		board[zerorow][zerocol] = tile;
		zerocol--;
		return true;
	}
	else if (zerocol < d - 1 && board[zerorow][zerocol + 1] == tile)
	{
		board[zerorow][zerocol + 1] = 0;
		board[zerorow][zerocol] = tile;
		zerocol++;
		return true;
	}
	return false;
}

/*
 * Returns true if game is won (i.e., board is in winning configuration),
 * else false.
 */
bool won(void) {
	int tile = 1;
	//quick and dirty preliminary check
	if (board[d - 1][d - 1] != 0)
		return false;

	for (int row = 0; row < d; row++)
	{
		for (int column = 0; column < d; column++)
		{
			//the last grid position.
			if (row == d - 1 && column == d - 1)
				return true;

			if (tile != board[row][column])
				return false;
			tile++;
		}
	}
	return false;
}

/*
 * Prints dashes.
 *
 */
void pdash(int n) {
	for (; n > 0; n--)
	{
		printf("-");
	}
	printf("\n");
}

/*
 * finds the zero tile, in case we misplaced it....
 *
 */
void findzero(void) {
	for (int row = 0; row < d; row++)
	{
		for (int column = 0; column < d; column++)
		{
			if (0 == board[row][column])
			{
				zerorow = row;
				zerocol = column;
			}
		}
	}
}
