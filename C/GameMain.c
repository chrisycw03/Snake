#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#ifdef _WIN32
	#include <conio.h>
	#include <windows.h>
#endif

#define UP (Coordinate){0, -1}
#define DOWN (Coordinate){0, 1}
#define LEFT (Coordinate){-1, 0}
#define RIGHT (Coordinate){1, 0}

typedef struct Coordinate{
    int x;
    int y;
} Coordinate;

typedef struct LinkList{
    Coordinate body;
    struct LinkList* next;
	struct LinkList* prev;
} SnakeBody;

const Coordinate map_range = {20, 18};
const Coordinate info_range = {20, map_range.y};
const Coordinate range = {map_range.x + info_range.x,  map_range.y};

int record_score = 0;
char key_ctrl = '\0';
Coordinate snake_direction;
SnakeBody* snake_head;
SnakeBody* snake_tail;
int snake_speed;
int snake_speed_level = 0;
int snake_score;
Coordinate food;

void gotoxy(int x, int y){
    printf("%c[%d;%dH", 0x1B, y + 1, x + 1);
}

void gotoEnd(){
    printf("%c[%d;%dH", 0x1B, range.y + 3, 0);
}

void TimeToSleep(int milisec){
	struct timespec request = {0, milisec * 1000000};
	struct timespec remain;
	nanosleep(&request, &remain);
}

void EraseDisplay(){
    printf("%c[%dJ", 0x1B, 1);
}

void RefreshDisplay(){
    char* blank = (char*)calloc(map_range.x - 2, sizeof(char));
    memset(blank, ' ', map_range.x - 2);
    for(int i = 1; i < map_range.y - 1; i++)
    {
        gotoxy(1, i);
        printf("%s", blank);
    }
}

void InitDisplay(){
    gotoxy(0, 0);
    for(int i = 0; i < range.y; i++)
    {
        for(int j = 0; j < range.x; j++)
        {
            if(i == 0 || i == range.y - 1)
            {
                printf("%c", '#');
            }
            else if(j == 0 || j == map_range.x - 1 || j == range.x - 1)
            {
                printf("%c", '#');
            }
            else
            {
                printf("%c", ' ');
            }
        }
        printf("\n");
    }

    gotoxy(map_range.x + 1, 2);
    printf("Score:\t%d", 0);
    gotoxy(map_range.x + 1, 3);
    printf("Speed:\t%d", 0);
    gotoxy(map_range.x + 1, 4);
    printf("Record:\t%d", record_score);
    gotoxy(map_range.x + 1, 6);
    printf("Up:\tw");
    gotoxy(map_range.x + 1, 7);
    printf("Down:\ts");
    gotoxy(map_range.x + 1, 8);
    printf("Left:\ta");
    gotoxy(map_range.x + 1, 9);
    printf("Right:\td");
    gotoxy(map_range.x + 1, 10);
    printf("Start:\tENTER");
    gotoxy(map_range.x + 1, 11);
    printf("Pause:\tp");
    gotoxy(map_range.x + 1, 12);
    printf("EXIT:\tESC");
    gotoxy(map_range.x + 1, 14);
    printf("Version: %s", "1.0");
    gotoxy(map_range.x + 1, 15);
    printf("Author: %s", "chrisycw03");

    gotoEnd();
}

void DisplayStart(){
    gotoxy(4, map_range.y / 2);
    printf("%s", "PRESS 'ENTER'");
    gotoxy(6, map_range.y / 2 + 1);
    printf("%s", "TO START");

    gotoEnd();
}

void DisplayGameOver(){
	key_ctrl = '\0';
	
	gotoxy(6, map_range.y / 2 - 1);
    printf("%s", "GAME OVER");
	gotoxy(4, map_range.y / 2);
    printf("%s", "PRESS 'ENTER'");
    gotoxy(6, map_range.y / 2 + 1);
    printf("%s", "TO START");

    gotoEnd();
}

void DisplayGameExit(){
	RefreshDisplay();
	gotoxy(6, map_range.y / 2);
    printf("%s", "GAME EXIT");
    gotoEnd();
}

void RefreshScore(int score){
    gotoxy(map_range.x + 12, 2);
    printf("%-4d", score);
	
	if(score > record_score){
		record_score = score;
		gotoxy(map_range.x + 12, 4);
		printf("%d", record_score);
	}
	
    gotoEnd();
}

void RefreshSpeed(int speed){
    gotoxy(map_range.x + 12, 3);
    printf("%d", speed);
    gotoEnd();
}

void InitSnake(){
    RefreshDisplay();

    snake_direction = (Coordinate){-1, 0};
    snake_speed = 450;
	snake_speed_level = 0;
    snake_score = 0;
	
	RefreshScore(snake_score);
	RefreshSpeed(snake_speed_level);

    int init_length = 4;
    SnakeBody* cur_node = (SnakeBody*)malloc(sizeof(SnakeBody));
    snake_head = cur_node;

    for(int i = 0; i < init_length; i++)
    {
        snake_tail = (SnakeBody*)malloc(sizeof(SnakeBody));
        snake_tail->body.x = map_range.x / 2 + i;
        snake_tail->body.y = map_range.y / 2;
        snake_tail->next = NULL;
		snake_tail->prev = cur_node;
		
        cur_node->next = snake_tail;
        cur_node = cur_node->next;

        gotoxy(cur_node->body.x, cur_node->body.y);
		printf("\033[0;32m");
        printf("%wc", '*');
		printf("\033[0m");
    }

    gotoEnd();
}

int IsCollided(){
    SnakeBody* head = snake_head->next;
    SnakeBody* node = head->next;
	
	
    if(head->body.x == 0 || head->body.x == map_range.x - 1 || head->body.y == 0 || head->body.y == map_range.y - 1){
        return 1;
    }

    while(node != NULL){
        if(head->body.x == node->body.x && head->body.y == node->body.y){
            return 1;
        }
        node = node->next;
    }
	
	return 0;
}

void CreateFood(){
    srand(time(NULL));
    food.x = rand() % (map_range.x - 2) + 1;
    food.y = rand() % (map_range.y - 2) + 1;
    SnakeBody* node = snake_head->next;

    while(node != NULL)
    {
        if(food.x == node->body.x)
        {
            food.x = rand() % (map_range.x - 2) + 1;
            node = snake_head->next;
        }
        else
        {
            node = node->next;
        }
    }

    while(node != NULL)
    {
        if(food.y == node->body.y)
        {
            food.y = rand() % (map_range.y - 2) + 1;
            node = snake_head->next;
        }
        else
        {
            node = node->next;
        }
    }

    gotoxy(food.x, food.y);
	printf("\033[0;33m");
    printf("%c", '$');
	printf("\033[0m");
    gotoEnd();
}

int IsEaten(){
    SnakeBody* head = snake_head->next;
    if(head->body.x == food.x && head->body.y == food.y)
    {
        CreateFood();
        snake_score++;
		RefreshScore(snake_score);
		
		if(snake_speed > 100){
			snake_speed = snake_speed - 50;
			RefreshSpeed(++snake_speed_level);
		}
        return 1;
    }

    return 0;

}

void RefreshSnake(){
	SnakeBody* node = snake_head->next;
	SnakeBody* new_head = (SnakeBody*)malloc(sizeof(SnakeBody));
	new_head->body.x = node->body.x + snake_direction.x;
	new_head->body.y = node->body.y + snake_direction.y;
	
	new_head->next = snake_head->next;
	snake_head->next->prev = new_head;
	snake_head->next = new_head;
	
	if(IsCollided() == 1)
		return;
	
	if(IsEaten() != 1){
		node = snake_tail->prev;
		gotoxy(snake_tail->body.x, snake_tail->body.y);
		printf("%c", ' ');
		
		free(snake_tail);
		snake_tail = node;
		snake_tail->next = NULL;
	}else{
		IsEaten();
	}
	
	gotoxy(snake_head->next->body.x, snake_head->next->body.y);
	printf("\033[0;32m");
	printf("%c", '*');
	printf("\033[0m");
	gotoEnd();
}

void GameCtrl(){
	
	key_ctrl = getch();
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	
	if(key_ctrl == 'p' || key_ctrl == 'P'){
		
		key_ctrl = '\0';
		
		while(key_ctrl != 'p'){
			key_ctrl = getch();
		}
		return;
	}
	
    switch(key_ctrl){
	case 0x1B:
	case 0x0D:
		break;
	case 'P':
	case 'p':
		break;
    case 'W':
    case 'w':
        if(snake_direction.y != 1) snake_direction = UP;
        break;
    case 'S':
    case 's':
        if(snake_direction.y != -1) snake_direction = DOWN;
        break;
    case 'A':
    case 'a':
        if(snake_direction.x != 1) snake_direction = LEFT;
        break;
    case 'D':
    case 'd':
        if(snake_direction.x != -1) snake_direction = RIGHT;
        break;
    }
}

int main(){
	
	EraseDisplay();
	InitDisplay();
	DisplayStart();
	
	while(key_ctrl != 0x1B)
	{
		if(key_ctrl != 0x0D){
			GameCtrl();
			continue;
		}
		
		InitSnake();
		CreateFood();
		
		do{
			TimeToSleep(snake_speed);
			
			if(kbhit())
			{
				GameCtrl();
			}
			
			RefreshSnake();
			
		} while(IsCollided() != 1 && key_ctrl != 0x1B);
		
		DisplayGameOver();
		GameCtrl();
		
		
	}
	
	DisplayGameExit();

    return 0;
}