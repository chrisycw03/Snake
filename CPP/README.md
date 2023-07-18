# 版本
    1.0 First Release
    1.1 Add Display::RefreshMap()

# 概況
* 編譯器：MinGW
* 開發語言：C++, Windows API
* 作業系統：Windows 11
* 遊戲畫面：
![](https://hackmd.io/_uploads/HJzD1JhYh.png)


# 整體規劃
![](https://hackmd.io/_uploads/HksTAnjth.png)

# 01 顯示類
```cpp = 1
class Display
{
    friend Snake;

public:
    static void DrawMapInfo();
    static void DrawScore(int score);
    static void DrawSpeed(int speedLevel);
    static void RefreshMap();
}
```
## 1.1 DrawMapInfo()
* 顯示遊戲畫面的範圍
* 顯示遊戲控制操作、其他資訊
## 1.2 DrawScore(int score)
* 顯示目前分數
* 顯示歷史最高分
## 1.3 DrawSpeed(int speedLevel)
* 顯示貪吃蛇目前速度

# 02 貪吃蛇類
```cpp=1
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
    Snake();
    int getSpeed();
    void SnakeCtrl(char key);
    void SnakeMove(Food& food);
    bool isEatFood(Food& food);
    bool isCollision();
}
```
## 2.1 SnakeCtrl(char key)
* 傳入鍵盤輸入的字符來改變貪吃蛇的方向
* 貪吃蛇**無法掉頭**，屬於無效輸入
## 2.2 SnakeMove(Food& food)
* 傳入Food物件，獲取Food物件實例的座標
* 貪吃蛇移動**只改變頭部跟尾部位置**：首先新增頭部位置，判斷如果貪吃蛇沒有吃到食物，則刪除尾部位置
* 新增的頭部位置為目前的**頭部位置加上行進方向的向量**
## 2.3 isEatFood(Food& food)
* 傳入Food物件，獲取Food物件實例的座標，判斷貪吃蛇是否吃到食物
* 吃到食物後加分
* 吃到食物時，呼叫Food成員函數isEaten()
* 吃到食物後加快貪吃蛇速度
## 2.4 isCollision()
* 判斷新增的貪吃蛇頭部是否碰撞牆壁
* 判斷新增的貪吃蛇頭部是否碰撞自身

# 03 食物類
```cpp=1
class Food
{
    friend class Snake;
    
private:
    COORD m_location;   // food location
    
public:
    Food(Snake snake);
    void isEaten(Snake snake)
```
## 3.1 isEaten(Snake snake)
* 當返回值為真，表示食物已經被吃掉，會在隨機位置生成一個新的食物
* 食物位置不可以超出遊戲範圍，且食物不可以生成在貪吃蛇身上
* 當Snake的成員函數isEatFood()判斷貪吃蛇吃到食物時，會呼叫Food::isEaten()

# 04 gotoxy()函數
* 將游標移動到Console的(x, y)位置
```cpp=1
void gotoxy(short x, short y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
```


# 05 Main()函數
* 首先初始化遊戲畫面
* 等待使用者輸入'ENTER'開始遊戲
```cpp=1
Display::DrawMapInfo();
Display::DrawScore(0);
Display::DrawSpeed(0);

gotoxy(map_x / 2 - 11, map_y / 2);
cout << "PRESS 'ENTER' TO START";

// get key input
char ctrl = getch();
```
* 每次輸入只接受一個控制，所以接收鍵盤輸入的字符後，需要清空緩衝區，避免下次先從緩衝區讀取字符
* 開始遊戲後先建構貪吃蛇和食物，貪吃蛇碰撞後會刪除存在Heap的貪吃蛇和食物變數，等重新開始遊戲再重新配置記憶體


```cpp=1
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
    Display::RefreshMap();

    Snake *snake = new Snake;
    Food *food = new Food(*snake);
    
    // start game
    while(true){...}
}
```

* 用conio.h的函數kbhit()判斷只有在鍵盤有輸入的時候才接收自符
* 將鍵盤輸入傳給snakeCtrl()控制貪吃蛇
```cpp=1
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
```
* 當貪吃蛇碰撞時，結束遊戲，等待重新開始
* 貪吃蛇速度由Sleep()控制
```cpp=1
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
```