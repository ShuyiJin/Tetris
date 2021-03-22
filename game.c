#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 21
#define WIDTH 14
#define Y_INDEX 21
#define X_INDEX_1 6
#define X_INDEX_2 15

#define YELLOW "\033[1;33m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"

//===================================
// Global Variable
int block[HEIGHT][WIDTH] = {0};
int score = 0;
int current;

//====================================
// Function
void init_block();
int rand_shape();
/////////////////////////////////////
// 0 ■ ■ ■ ■        1 ■ ■ ■        //
//                    ■            //
// 2 ■ ■ ■	  3 ■ ■     4 ■ ■      //
//     ■        ■ ■         ■ ■    //
/////////////////////////////////////

void gotoxy(int x, int y);	
void print_next(int n, int x, int y);
void display_all();
void display_current_shape(int n, int x, int y);


int main(int argc, const char *argv[])
{
	srand(time(0));
	init_block();
	current = rand_shape(); // init the 1st shape
	display_all();
	return 0;
}

void init_block()
{
	int i;
	for(i = 0; i < HEIGHT; i++)
	{
		block[i][0] = 1;
		block[i][WIDTH-1] = 1;
	}
	for(i = 1; i < WIDTH-1; i++ )
	{
		block[HEIGHT-1][i] = 1;
	}
}

void display_all()
{
	gotoxy(0,0);
	int i, j;
	for(i = 0; i < HEIGHT; i++)
	{
		for(j = 0; j < WIDTH; j++)
		{
			switch(block[i][j])
			{
			case 0:
				printf(" ");
				break;
			case 1:
				printf("◆");
				break;
			default:
				break;
			}
			printf(" ");
		}
		printf("\n");
	}
	
	display_current_shape(current, X_INDEX_1, 0);

	// Display next random shape	
	printf("Next: ");
	print_next(rand_shape(), X_INDEX_1, Y_INDEX);
	
	// Display score
	gotoxy(X_INDEX_2, Y_INDEX);
	printf("Score: %d", score);

	// Finish up 
	printf("\033[0m");
	gotoxy(0, Y_INDEX+5);
}

void display_current_shape(int n, int x, int y)
{
}

void print_next(int n, int x, int y)
{
	gotoxy(x, y);
	switch(n)
	{
	case 0:
		printf(GREEN);
		printf("■ ■ ■ ■");
		gotoxy(x, y+1);
		printf("       ");
		break;
	case 1:
		printf(YELLOW);
		printf("■ ■ ■  ");
		gotoxy(x, y+1);
		printf("■      ");
		break;
	case 2:
		printf(CYAN);
		printf("■ ■ ■  ");
		gotoxy(x, y+1);
		printf("  ■    ");
		break;
	case 3:
		printf(PURPLE);
		printf("■ ■    ");
		gotoxy(x, y+1);
		printf("■ ■    ");
		break;
	case 4:
		printf(YELLOW);
		printf("■ ■    ");
		gotoxy(x, y+1);
		printf("  ■ ■  ");
		break;
	default:
		break;

	}
	printf("\033[0m");
}

void gotoxy(int x, int y)	
{
	printf("\033[%d;%dH", y + 1, x + 1);	
}

int rand_shape()
{
	return rand()%5;
}
