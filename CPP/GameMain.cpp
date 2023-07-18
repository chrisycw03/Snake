#include <iostream>
#include <Windows.h>
#include <deque>
#include <conio.h>
#include <stdio.h>
#include <ctime>

using namespace std;

// set sizes of map, information block and display area
const int map_x = 30;
const int map_y = 23;
const int info_x = 30;
const int info_y = 7;
const int display_x = map_x + info_x;
const int display_y = map_y;

// best score
int bestScore = 0;

// set Console cursor location
void gotoxy(short x, short y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

class Food;

// define snake behavior and properties
class Snake
{
    friend class Food;

private:
    deque<COORD> m_body;    // snake body location
    COORD m_direction;      // direction of snake
    int m_score;            // current score
    int m_speed;            // current speed rate (millisecond per step)
    int m_speedLevel;       // speed level

public:
    // constructor
    Snake()
    {
        // initial snake body length
        int init_length = 4;
        m_body.resize(init_length);

        // draw snake
        for(int i = 0; i < init_length; i++)
        {
            m_body[i].X = map_x / 2 + i;
            m_body[i].Y = map_y / 2;
            gotoxy(m_body[i].X, m_body[i].Y);
            cout << "*";
        }

        // initial other class member
        m_direction.X = -1;
        m_direction.Y = 0;
        m_score = 0;
        m_speed = 450;
        m_speedLevel = 0;

    }

    // get current speed
    int getSpeed()
    {
        return m_speed;
    }

    // get key input, and change direction of snake
    void SnakeCtrl(char key)
    {
        switch(key)
        {
        case 'W':
        case 'w':
            m_direction.X = 0;
            if(m_direction.Y != 1) m_direction.Y = -1;
            break;
        case 'S':
        case 's':
            m_direction.X = 0;
            if(m_direction.Y != -1) m_direction.Y = 1;
            break;
        case 'A':
        case 'a':
            if(m_direction.X != 1) m_direction.X = -1;
            m_direction.Y = 0;
            break;
        case 'D':
        case 'd':
            if(m_direction.X != -1) m_direction.X = 1;
            m_direction.Y = 0;
            break;
        default:
            break;
        }
    }

    // refresh and draw snake location
    // just need to update head and tail of snake
    void SnakeMove(Food& food)
    {
        // draw new snake head
        COORD newHead;
        newHead.X = m_body[0].X + m_direction.X;
        newHead.Y = m_body[0].Y + m_direction.Y;
        m_body.push_front(newHead);
        gotoxy(newHead.X, newHead.Y);
        cout << "*";

        // if snake did not eat food, then erase tail location at last step
        if(!isEatFood(food))
        {
            gotoxy(m_body.back().X, m_body.back().Y);
            cout << " ";
            m_body.pop_back();
        }

    }

    // if snake ate food, then return true
    bool isEatFood(Food& food);

    // if snake collided with itself or wall, then return true
    bool isCollision()
    {
        // check locations of snake head and wall
        if(m_body[0].X == 0 || m_body[0].X == (map_x - 1) || m_body[0].Y == 0 || m_body[0].Y == (map_y - 1))
        {
            return true;
        }

        // check locations of snake head and body
        for(size_t i = 1; i < m_body.size(); i++)
        {
            if(m_body[0].X == m_body[i].X && m_body[0].Y == m_body[i].Y)
                return true;
        }

        return false;
    }

};

// define food location and behavior
class Food
{
    friend class Snake;

private:
    COORD m_location;   // food location

public:
    // constructor
    Food(Snake snake)
    {
        // initialize random seed
        srand(time(NULL));

        // initialize food location
        isEaten(snake);
    }

    // if food is eaten, then generate new food
    void isEaten(Snake snake)
    {
        // generate food location in x direction
        size_t i = 0;
        while(i < snake.m_body.size())
        {
            // prevent food location coincide with snake
            if(m_location.X == snake.m_body[i].X || i == 0)
            {
                // make food locate inside map area
                m_location.X = rand() % (map_x - 2) + 1;
                i = 0;
            }
            i++;
        }

        // generate food location in y direction
        i = 0;
        while(i < snake.m_body.size())
        {
            // prevent food location coincide with snake
            if(m_location.Y == snake.m_body[i].Y || i == 0)
            {
                // make food locate inside map area
                m_location.Y = rand() % (map_y - 2) + 1;
                i = 0;
            }
            i++;
        }

        // draw food
        gotoxy(m_location.X, m_location.Y);
        cout << "$";

    }

};

class Display
{
    friend Snake;

public:
    static void DrawMapInfo()
    {
        // refresh Console
        system("cls");

        // draw game frame
        for(int i = 0; i < display_y; i++)
        {
            for(int j = 0; j < display_x; j++)
            {
                if(i == 0 || i == display_y - 1)
                {
                    cout << "=";
                }
                else if(j == 0 || j == map_x - 1 || j == display_x - 1)
                {
                    cout << "#";
                }
                else if(i == info_y && j >= map_x)
                {
                    cout << "=";
                }
                else
                {
                    cout << " ";
                }


            }
            cout << endl;
        }

        // draw information
        gotoxy(map_x + 2, info_y + 2);
        cout << "Control:";
        gotoxy(map_x + 2, info_y + 3);
        cout << "W:\t\tUp";
        gotoxy(map_x + 2, info_y + 4);
        cout << "S:\t\tDOWN";
        gotoxy(map_x + 2, info_y + 5);
        cout << "A:\t\tLEFT";
        gotoxy(map_x + 2, info_y + 6);
        cout << "D:\t\tRIGHT";

        gotoxy(map_x + 2, info_y + 7);
        cout << "Start:\t\t" << "ENTER";
        gotoxy(map_x + 2, info_y + 8);
        cout << "Quit:\t\t" << "ESC";

        gotoxy(map_x + 2, info_y + 11);
        cout << "Author:\t\t" << "chrisycw03";
        gotoxy(map_x + 2, info_y + 12);
        cout << "Version:\t" << "1.1";

    }

    // draw score
    static void DrawScore(int score)
    {
        gotoxy(map_x + 2, 2);
        cout << "Score:\t" << score;
        if(score > bestScore)
            bestScore = score;
        gotoxy(map_x + 2, 3);
        cout << "Best:\t" << bestScore;
    }

    // draw speed level
    static void DrawSpeed(int speedLevel)
    {
        gotoxy(map_x + 2, 4);
        cout << "Speed:\t" << speedLevel;
    }

    // refresh map area
    static void RefreshMap()
    {
        gotoxy(0, 0);
        cout << string(map_x, '=');
        gotoxy(0, map_y - 1);
        cout << string(map_x, '=');
        for(int i = 1; i < map_y - 1; i++)
        {
            gotoxy(0, i);
            cout << '#';
            cout << string(map_x - 2, ' ');
            cout << '#';
        }
    }
};

// if snake ate food, then return true
bool Snake::isEatFood(Food& food)
{
    // if snake head coincide with food, then snake ate food
    if(m_body.front().X == food.m_location.X && m_body.front().Y == food.m_location.Y)
    {
        m_score++;
        food.isEaten(*this);
        Display::DrawScore(m_score);

        // speed up for higher score
        switch(m_score)
        {
        case 1:
        case 2:
        case 3:
        case 4:
            if(m_speed > 50)
            {
                m_speed = m_speed - 100;
                m_speedLevel++;
            }
            Display::DrawSpeed(m_speedLevel);
            break;
        default:
            break;
        }

        return true;
    }

    return false;
}

int main()
{
    Display::DrawMapInfo();
    Display::DrawScore(0);
    Display::DrawSpeed(0);

    gotoxy(map_x / 2 - 11, map_y / 2);
    cout << "PRESS 'ENTER' TO START";

    // get key input
    char ctrl = getch();

    // quit game when input 'ESC'
    while(ctrl != 0x1b)
    {
        // wait for 'ENTER' to start
        if(ctrl != 0x0d)
        {
            ctrl = getch();
            FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
            continue;
        }

        // refresh
        Display::DrawMapInfo();
        Display::DrawScore(0);
        Display::DrawSpeed(0);

        Snake *snake = new Snake;
        Food *food = new Food(*snake);

        // start game
        while(true)
        {
            // if keyboard is hit, get input character
            if(kbhit())
            {
                ctrl = getch();
                // flush input to ensure receive only one character every time
                FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

                snake->SnakeCtrl(ctrl);

                // if input 'ESC', then quit game
                if(ctrl == 0x1b)
                {
                    break;
                }
            }

            // snake move
            snake->SnakeMove(*food);

            // if snake collided, then end current game
            // wait for restart or quit
            if(snake->isCollision())
            {
                gotoxy(map_x / 2 - 4, map_y / 2);
                cout << "GAME OVER";
                ctrl = '\0';
                gotoxy(map_x / 2 - 11, map_y / 2 + 1);
                cout << "PRESS 'ENTER' TO RESTART";
                delete snake;
                delete food;
                break;
            }

            // set speed rate
            Sleep(snake->getSpeed());
        }
    }

    // display for game exit
    Display::RefreshMap();
    gotoxy(map_x / 2 - 4, map_y / 2);
    cout << "GAME EXIT";
    Sleep(1000);
    gotoxy(0, map_y);

    return 0;
}
