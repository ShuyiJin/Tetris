#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define STTY_UP "stty raw -echo -F "
#define STTY_DOWN "stty -raw echo -F "
#define PATH "/dev/tty"

#define HEIGHT 21
#define WIDTH 14
#define Y_INDEX 21
#define X_INDEX_1 6
#define X_INDEX_2 15
#define PERIOD 100000
#define SENSITIVITY 15 

#define YELLOW "\033[1;33m"
#define BROWN "\033[0;33m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define WHITE "\033[0m"

struct position
{
	int x;
	int y;
};


//===================================
// Global Variable
int block[HEIGHT][WIDTH] = {0}; // game board
struct position point[4];       // shape made of 4 points
int score = 0;
int current, next;				// current & next shape
int speed = SENSITIVITY;		// the smaller the speed, the faster the shape drops
int level = 1;                  // 10 levels in total
int state = 0;                  // initial state of shapes

//====================================
// Function Declaration
void init_block();
int rand_shape();
//////////////////////////////////////////
// 0 ■ ■ ■ ■      1 ■ ■ ■    2  ■ ■ ■   //
//                  ■             ■     //
// 3 ■ ■     4 ■ ■       5   ■ ■        //
//   ■ ■         ■ ■       ■ ■          //
//////////////////////////////////////////

void gotoxy(int x, int y);
void init_point();
void point_move_down();
void point_move_left();
void point_move_right();
void rotate();
void update_board();
void clean_row();
void drop_points(int rol);
int is_over();

void draw_point();
void print_next();
void display_all();
int get_char();
int usr_in();


int main(int argc, const char *argv[])
{
	system(STTY_UP PATH); // disable new line
	srand(time(0));
	init_block();
	current = rand_shape(); 
	next = rand_shape();
	init_point();

	while(1)
	{
		if(!usr_in())
			break;

		point_move_down();

		if(is_over())
		{
			break;
		}
	}

	return 0;
}

void drop_points(int row)
{
	int i, j;
	for(i = row; i > 0; i--)
	{
		for(j = 1; j < WIDTH-1; j++)
		{
			block[i][j] = block[i-1][j];
		}
	}
}

void clean_row()
{
	int i, j, n = 0;
	for(i = HEIGHT-2; i > 0; i--)
	{
		int flg = 1;
		for(j = 1; j < WIDTH-1; j++)
		{
			if(block[i][j] == 0)
			{
				flg = 0; // row is not filled
				break;
			}
		}
		if(flg) // row is filled
		{
			n++;
			drop_points(i);
			score += n*100;

			if(score%1000 == 0)
			{
				level++;
				if(level > 10)
					level = 10;
				else
					speed--;
			}

			i++;
		}
	}
}

int get_char() // non-blocking IO
{
	fd_set rfds;
	struct timeval tv;
	int ch = 0;

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 10; // set waiting time

	// check user input
	if (select(1, &rfds, NULL, NULL, &tv) > 0)
	{
		ch = getchar(); 
	}
	return ch;
}

int usr_in()
{
	int i;
	for(i = 0; i < speed; i++)
	{
		char letter = get_char();
		switch(letter)
		{
		case 'a':
			point_move_left();
			break;
		case 's':
			point_move_down();
			break;
		case 'd':
			point_move_right();
			break;
		case 'w':
			rotate();
			break;
		case' ':   // pause
			getchar();
			break;
		case 3:
			system(STTY_DOWN PATH);
			return 0;
			break;
		default:
			break;
		}
		display_all();
		usleep(PERIOD);
	}
	return 1;
}

int is_over()
{
	int i;
	for( i = 1; i < WIDTH-1; i++)
	{
		if(block[0][i] != 0)
		{
			gotoxy(WIDTH-2, HEIGHT+2);
			printf(WHITE);
			printf("OVER");
			gotoxy(0, Y_INDEX+5);
			system(STTY_DOWN PATH);

			return 1;
		}
	}
	return 0;
}

void update_board()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		block[ point[i].y ][ point[i].x ] = 2;
	}
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
	system("clear");
	gotoxy(0,0);
	int i, j;
	for(i = 0; i < HEIGHT; i++)
	{
		gotoxy(0, i);
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
			case 2:
				printf("■");
				break;
			default:
				break;
			}
			printf(" ");
		}
	}
	
	// Draw current shape
	draw_point();

	// Display next random shape	
	print_next();
	
	// Display score
	gotoxy(X_INDEX_2, Y_INDEX);
	printf("Score: %d", score);

	// Finish up 

	fflush(stdout);
}
void point_move_left()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		if(block[ point[i].y ][ point[i].x-1] != 0)
		{
			return;
		}
	}

	for(i = 0; i < 4; i++)
	{
		point[i].x--;
	}
}

void point_move_right()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		if(block[ point[i].y ][ point[i].x+1] != 0)
		{
			return;
		}
	}

	for(i = 0; i < 4; i++)
	{
		point[i].x++;
	}
}

void point_move_down()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		if(block[ point[i].y+1 ][ point[i].x ] != 0)
		{
			update_board();
			clean_row();
			current = next;
			next = rand_shape();
			init_point();
		}
	}

	for(i = 0; i < 4; i++)
	{
		point[i].y++;
	}

	return;
}

void draw_point()
{
	switch(current)
	{
	case 0:
		printf(GREEN);
		break;
	case 1:
		printf(BLUE);
		break;
	case 2:
		printf(CYAN);
		break;
	case 3:
		printf(PURPLE);
		break;
	case 4:
		printf(YELLOW);
		break;
	case 5:
		printf(BROWN);
		break;
	default:
		break;
	}

	int i; 
	for(i=0; i<4; i++)
	{
		gotoxy(point[i].x * 2 , point[i].y ); 
		printf("■ ");
	}
	printf(WHITE);
	gotoxy(0, Y_INDEX+2);
}

void init_point()
{
	state = 0;

	point[0].x = 4;
	point[0].y = 0;

	point[1].x = 5;
	point[1].y = 0;
	
	if (current == 0)
	{
		point[2].x = 6;
		point[2].y = 0;

		point[3].x = 7;
		point[3].y = 0;
	}
	else if (current == 1)
	{
		point[2].x = 6;
		point[2].y = 0;

		point[3].x = 4;
		point[3].y = 1;
	}
	else if (current == 2)
	{
		point[2].x = 6;
		point[2].y = 0;

		point[3].x = 5;
		point[3].y = 1;
	}
	else if (current == 3)
	{
		point[2].x = 4;
		point[2].y = 1;

		point[3].x = 5;
		point[3].y = 1;
	}
	else if (current == 4)
	{
		point[2].x = 5;
		point[2].y = 1;
		point[3].x = 6;
		point[3].y = 1;
	}
	else if (current == 5)
	{
		point[2].x = 3;
		point[2].y = 1;
		point[3].x = 4;
		point[3].y = 1;
	}
}

void print_next()
{
	gotoxy(0, Y_INDEX);
	printf(WHITE);
	printf("Next: ");
	gotoxy(X_INDEX_1, Y_INDEX);
	switch(next)
	{
	case 0:
		printf(GREEN);
		printf("■ ■ ■ ■");
		break;
	case 1:
		printf(BLUE);
		printf("■ ■ ■");
		gotoxy(X_INDEX_1, Y_INDEX+1);
		printf("■");
		break;
	case 2:
		printf(CYAN);
		printf("■ ■ ■");
		gotoxy(X_INDEX_1, Y_INDEX+1);
		printf("  ■");
		break;
	case 3:
		printf(PURPLE);
		printf("■ ■");
		gotoxy(X_INDEX_1, Y_INDEX+1);
		printf("■ ■");
		break;
	case 4:
		printf(YELLOW);
		printf("■ ■");
		gotoxy(X_INDEX_1, Y_INDEX+1);
		printf("  ■ ■");
		break;
	case 5:
		printf(BROWN);
		printf("  ■ ■");
		gotoxy(X_INDEX_1, Y_INDEX+1);
		printf("■ ■");
		break;
	default:
		break;
	}
	printf(WHITE);
}

void gotoxy(int x, int y)	
{
	printf("\033[%d;%dH", y + 1, x + 1);	
}

int rand_shape()
{
	return rand()%6;
}

void avoid_boarder()
{
	int i, offset = 0;
	for (i = 0; i < 4; i++)
	{
		if(point[i].x  == 0)
		{
			offset = 1;
		}
		else if(point[i].x == -1)
		{
			offset = 2;
		}
		else if(point[i].x == WIDTH-1)
		{
			offset = -1;
		}
		else if(point[i].x == WIDTH)
		{
			offset = -2;
		}
	}
	for(i = 0; i < 4; i++)
	{
		point[i].x += offset;
	}
}

void rotate()
{
	if(current == 0 && state == 0)  // ■ ■ ■ ■
	{                               
		state = 1;
		point[0].x ++;
		point[0].y --;
		point[2].x --;
		point[2].y ++;
		point[3].x -= 2;
		point[3].y += 2;
	}
	else if(current == 0 && state == 1) //  ■
	{                                   //  ■
		state = 0;                      //  ■
		point[0].x --;                  //  ■
		point[0].y ++;
		point[2].x ++;
		point[2].y --;
		point[3].x += 2;
		point[3].y -= 2;
	}
	else if(current == 1 && state == 0)  //   ■ ■ ■
	{                                    //   ■
		state = 1;
		point[1].x --;
		point[1].y ++;
		point[2].x -= 2;
		point[2].y += 2;
		point[3].x --;
		point[3].y --;
	}
	else if(current == 1 && state == 1)   //  ■ ■
	{                                     //    ■
        state = 2;                        //    ■
		point[1].x --;
		point[1].y --;
		point[2].x -= 2;
		point[2].y -= 2;
		point[3].x ++;
		point[3].y --;
	}
	else if (current == 1 && state == 2)   //     ■
	{                                      // ■ ■ ■
		state = 3;
		point[1].x ++;
		point[1].y --;
		point[2].x += 2;
		point[2].y -= 2;
		point[3].x ++;
		point[3].y ++;
	}
	else if (current == 1 && state == 3)  // ■
	{                                     // ■
		state = 0;                        // ■ ■
		point[1].x ++;
		point[1].y ++;
		point[2].x += 2;
		point[2].y += 2;
		point[3].x --;
		point[3].y ++;
	}
	else if(current == 2 && state == 0)   //  ■ ■ ■
	{                                     //    ■
        state = 1;
		point[0].x ++;
		point[0].y --;
		point[2].x --;
		point[2].y ++;
		point[3].x --;
		point[3].y --;
	}
	else if(current == 2 && state == 1)   //    ■ 
	{                                     //  ■ ■
        state = 2;                        //    ■
		point[0].x ++;
		point[0].y ++;
		point[2].x --;
		point[2].y --;
		point[3].x ++;
		point[3].y --;
	}
	else if(current == 2 && state == 2)   //    ■ 
	{                                     //  ■ ■ ■
        state = 3;
		point[0].x --;
		point[0].y ++;
		point[2].x ++;
		point[2].y --;
		point[3].x ++;
		point[3].y ++;
	}
	else if(current == 2 && state == 3)   //  ■ 
	{                                     //  ■ ■
        state = 0;                        //  ■
		point[0].x --;
		point[0].y --;
		point[2].x ++;
		point[2].y ++;
		point[3].x --;
		point[3].y ++;
	}
	else if (current == 4 && state == 0)   //  ■ ■
	{                                      //    ■ ■
		state = 1;
		point[0].x ++;
		point[0].y --;
		point[2].x --;
		point[2].y --;
		point[3].x -= 2;
	}
	else if (current == 4 && state == 1)   //    ■ 
	{                                      //  ■ ■
		state = 0;                         //  ■ 
		point[0].x --;
		point[0].y ++;
		point[2].x ++;
		point[2].y ++;
		point[3].x += 2;
	}
	else if (current == 5 && state == 0)   //   ■ ■
	{                                      // ■ ■
		state = 1;
		point[1].x --;
		point[1].y ++;
		point[2].y -= 2;
		point[3].x --;
		point[3].y --;
	}
	else if (current == 5 && state == 1)   // ■ 
	{                                      // ■ ■
		state = 0;                         //   ■
		point[1].x ++;
		point[1].y --;
		point[2].y += 2;
		point[3].x ++;
		point[3].y ++;
	}
	avoid_boarder(); 

}


