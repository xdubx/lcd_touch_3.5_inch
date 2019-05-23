/////////////////////////////////////////
//                                     //
// Rewrite for LCDWIKI_LBV by DuB      //
// gamelogic from http://www.educ8s.tv //
//                                     //
/////////////////////////////////////////

#include <TouchScreen.h> //touch library
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

// Define chip selector pins
#define LCD_RD A0
#define LCD_WR A1
#define LCD_RS A2
#define LCD_CS A3
#define LCD_REST A4

#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

//param calibration from kbv
#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92
#define TS_MAXY 952

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

// Mega dig. pin:  29  28  27  26  25  24  23  22
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
LCDWIKI_KBV my_lcd(ILI9486, LCD_CS, LCD_RS, LCD_WR, LCD_RD, LCD_REST); //model,cs,cd,wr,rd,reset
boolean rotated = false;

// game varibles
extern uint8_t circle[];
extern uint8_t x_bitmap[];
int gameScreen = 1;
int moves = 1;
int winner = 1; //0 = Draw, 1 = Human, 2 = CPU
boolean buttonEnabled = true;
//Customs text
String customText = "SV2 ATMEGA 2560 Touch LCD";
String gameText = "Tic Tac Toe";
String startGameText = "Spiel start";
String gameEndText = "Spiel Ende";
String replayText = "Spiel neustart";
String drawText = "Draw";
String humanText = "You WIN";
String cpuText = "AT Mega WIN";

int board[] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // holds position data 0 is blank, 1 human, 2 is computer

void setup()
{
  Serial.begin(9600);
  Serial.print("Starting...");
  my_lcd.Init_LCD();
  my_lcd.Set_Rotation(1);
  rotated = true; // for the touch
  Serial.println(my_lcd.Read_ID(), HEX);
  randomSeed(analogRead(0));

  drawStartScreen();
}

void loop()
{
  TSPoint p = ts.getPoint(); //Get touch point

  if (gameScreen == 3)
  {
    buttonEnabled = true;
  }

  if (p.z > ts.pressureThreshhold)
  {
    int y = 0;
    int x = 0;
    //rotate touch input if needed
    if (rotated)
    {
      y = map(p.x, TS_MINX, TS_MAXX, 0, my_lcd.Get_Display_Height());
      x = map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Width(), 0);
    }
    else
    {
      y = map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(), 0);
      x = map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
    }
    Serial.print("X = ");
    Serial.print(x);
    Serial.print("\tY = ");
    Serial.print(y);
    Serial.print("\n");
    Serial.println(String(getCenterX() - 100) + " " + (getCenterX() + 100) + " " + (getCenterY() + 60) + " " + (getCenterY() + 120));
    if (x > getCenterX() - 100 && x < getCenterX() + 100 && y > getCenterY() + 60 && y < getCenterY() + 120 && buttonEnabled) // The user has pressed inside the red rectangle
    {
      buttonEnabled = false; //Disable button
      Serial.println("Button Pressed");
      resetGame();
      //This is important, because the libraries are sharing pins
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      drawGameScreen();
      //playGame();
    }
    delay(10);
  }
}

void resetGame()
{
  buttonEnabled = false;
  for (int i = 0; i < 9; i++)
  {
    board[i] = 0;
  }
  moves = 1;
  winner = 0;
  gameScreen = 2;
}

int getCenterX()
{
  return (my_lcd.Get_Display_Width() / 2);
}
int getCenterY()
{
  return (my_lcd.Get_Display_Height() / 2);
}

void show_string(String str, int16_t x, int16_t y, uint8_t csize, uint16_t fc, uint16_t bc, boolean mode)
{
  my_lcd.Set_Text_Mode(mode);
  my_lcd.Set_Text_Size(csize);
  my_lcd.Set_Text_colour(fc);
  my_lcd.Set_Text_Back_colour(bc);
  my_lcd.Print_String(str, x, y);
}

void drawStartScreen()
{
  Serial.println("Draw StartScreen");
  my_lcd.Fill_Screen(BLACK);
  show_string(customText, getCenterX() - (customText.length() * 6), getCenterY() - 80, 2, GREEN, BLACK, 1);
  show_string(gameText, getCenterX() - (gameText.length() * 8), getCenterY(), 3, WHITE, BLACK, 1);

  // create start button

  my_lcd.Set_Draw_color(RED);
  my_lcd.Fill_Rectangle(getCenterX() - 100, getCenterY() + 60, getCenterX() + 100, getCenterY() + 120);
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Draw_Rectangle(getCenterX() - 100, getCenterY() + 60, getCenterX() + 100, getCenterY() + 120);
  show_string(startGameText, getCenterX() - (gameText.length() * 6), getCenterY() + 80, 2, WHITE, RED, 1);
}

void drawGameScreen()
{
  Serial.println("Draw game Screen");
  my_lcd.Fill_Screen(BLACK);

  //Draw frame
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Draw_Rectangle(0, 1, my_lcd.Get_Display_Width() - 1, my_lcd.Get_Display_Height() - 1);
  
  //draw line vertical
  my_lcd.Draw_Line(my_lcd.Get_Display_Width() / 3, my_lcd.Get_Display_Height() - 20, (my_lcd.Get_Display_Width() / 3), 20);
  my_lcd.Draw_Line((my_lcd.Get_Display_Width() / 3) * 2, my_lcd.Get_Display_Height() - 20, (my_lcd.Get_Display_Width() / 3) * 2, 20);

  //draw line vertical
  my_lcd.Draw_Line(20, my_lcd.Get_Display_Height() / 3, (my_lcd.Get_Display_Width() - 20), my_lcd.Get_Display_Height() / 3);
  my_lcd.Draw_Line(20, (my_lcd.Get_Display_Height() / 3) * 2, (my_lcd.Get_Display_Width() - 20), (my_lcd.Get_Display_Height() / 3) * 2);

}

void drawGameOverScreen()
{
  Serial.println("Draw Endscreen");
  my_lcd.Fill_Screen(BLACK);

  //Draw frame
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Draw_Rectangle(0, 0, my_lcd.Get_Display_Width() - 1, my_lcd.Get_Display_Height() - 1);

  show_string(gameEndText, getCenterX() - (gameText.length() * 9), getCenterY() - 70, 4, WHITE, BLACK, 1);

  if (winner == 0)
  {
    show_string(drawText, getCenterX() - (gameText.length() * 4), getCenterY(), 3, YELLOW, BLACK, 1);
  }
  if (winner == 1)
  {
    show_string(humanText, getCenterX() - (gameText.length() * 5), getCenterY(), 3, BLUE, BLACK, 1);
  }

  if (winner == 2)
  {
    show_string(cpuText, getCenterX() - (gameText.length() * 5), getCenterY(), 3, RED, BLACK, 1);
  }

  my_lcd.Set_Draw_color(RED);
  my_lcd.Fill_Rectangle(getCenterX() - 100, getCenterY() + 60, getCenterX() + 100, getCenterY() + 120);
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Draw_Rectangle(getCenterX() - 100, getCenterY() + 60, getCenterX() + 100, getCenterY() + 120);
  show_string(replayText, getCenterX() - (gameText.length() * 7.5), getCenterY() + 80, 2, WHITE, RED, 1);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
}
void drawPlayerMove(int move)
{
  switch (move)
  {
  case 0:
    drawCircle((my_lcd.Get_Display_Width() / 6), (my_lcd.Get_Display_Height() / 6) * 5);
    break;
  case 1:
    drawCircle((my_lcd.Get_Display_Width() / 6) * 3, (my_lcd.Get_Display_Height() / 6) * 5);
    break;
  case 2:
    drawCircle((my_lcd.Get_Display_Width() / 6) * 5, (my_lcd.Get_Display_Height() / 6) * 5);
    break;
  case 3:
    drawCircle((my_lcd.Get_Display_Width() / 6), (my_lcd.Get_Display_Height() / 6) * 3);
    break;
  case 4:
    drawCircle((my_lcd.Get_Display_Width() / 6) * 3, (my_lcd.Get_Display_Height() / 6) * 3);
    break;
  case 5:
    drawCircle((my_lcd.Get_Display_Width() / 6) * 5, (my_lcd.Get_Display_Height() / 6) * 3);
    break;
  case 6:
    drawCircle((my_lcd.Get_Display_Width() / 6), (my_lcd.Get_Display_Height() / 6));
    break;
  case 7:
    drawCircle((my_lcd.Get_Display_Width() / 6) * 3, (my_lcd.Get_Display_Height() / 6));
    break;
  case 8:
    drawCircle((my_lcd.Get_Display_Width() / 6) * 5, (my_lcd.Get_Display_Height() / 6));
    break;
  }
}

void drawCircle(int x, int y)
{
  my_lcd.Set_Draw_color(BLUE);
  my_lcd.Draw_Circle(x, y, 10);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Circle(x, y, 8)
}

void drawCircleCPU(int x, int y)
{
  my_lcd.Set_Draw_color(RED);
  my_lcd.Draw_Circle(x, y, 10);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Circle(x, y, 8);
}

void drawCpuMove(int move)
{
  switch (move)
  {
  case 0:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6), (my_lcd.Get_Display_Height() / 6) * 5);
    break;
  case 1:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6) * 3, (my_lcd.Get_Display_Height() / 6) * 5);
    break;
  case 2:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6) * 5, (my_lcd.Get_Display_Height() / 6) * 5);
    break;
  case 3:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6), (my_lcd.Get_Display_Height() / 6) * 3);
    break;
  case 4:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6) * 3, (my_lcd.Get_Display_Height() / 6) * 3);
    break;
  case 5:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6) * 5, (my_lcd.Get_Display_Height() / 6) * 3);
    break;
  case 6:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6), (my_lcd.Get_Display_Height() / 6));
    break;
  case 7:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6) * 3, (my_lcd.Get_Display_Height() / 6));
    break;
  case 8:
    drawCircleCPU((my_lcd.Get_Display_Width() / 6) * 5, (my_lcd.Get_Display_Height() / 6));
    break;
  }
}
// game logic --------------------------------

//TODO finish player move

void playerMove()
{
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  TSPoint p;
  boolean validMove = false;
  Serial.print("\nPlayer Move:");
  do
  {
    p = ts.getPoint(); //Get touch point
    if (p.z > ts.pressureThreshhold)
    {
      int y = 0;
      int x = 0;
      //rotate touch input if needed
      if (rotated)
      {
        y = map(p.x, TS_MINX, TS_MAXX, 0, my_lcd.Get_Display_Height());
        x = map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Width(), 0);
      }
      else
      {
        y = map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(), 0);
        x = map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
      }
      Serial.println(x);
      Serial.println(y);

      if ((x < 115) && (y >= 150)) //6
      {
        if (board[6] == 0)
        {
          Serial.println("Player Move: 6");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[6] = 1;
          drawPlayerMove(6);
          Serial.println("Drawing player move");
        }
      }
      else if ((p.x > 0 && p.x < (my_lcd.Get_Display_Width() / 3)) && (p.y < (my_lcd.Get_Display_Height() / 3) * 2 && p.y > (my_lcd.Get_Display_Height() / 3))) //3
      {

        if (board[3] == 0)
        {
          Serial.println("Player Move: 3");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[3] = 1;
          drawPlayerMove(3);
          Serial.println("Drawing player move");
        }
      }
      else if ((p.x < (my_lcd.Get_Display_Width() / 3)) && (p.y < (my_lcd.Get_Display_Height() / 3))) //0
      {
        if (board[0] == 0)
        {
          Serial.println("Player Move: 0");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[0] = 1;
          drawPlayerMove(0);
        }
      }

      else if ((p.x > (my_lcd.Get_Display_Width() / 3) && p.x <= (my_lcd.Get_Display_Width() / 3) * 2) && (p.y < (my_lcd.Get_Display_Height() / 3))) //1
      {
        if (board[1] == 0)
        {
          Serial.println("Player Move: 1");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[1] = 1;
          drawPlayerMove(1);
        }
      }

      else if ((p.x > (my_lcd.Get_Display_Width() / 3) * 2) && (p.y < (my_lcd.Get_Display_Height() / 3))) //2
      {
        if (board[2] == 0)
        {
          Serial.println("Player Move: 2");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[2] = 1;
          drawPlayerMove(2);
        }
      }

      else if ((p.x > (my_lcd.Get_Display_Width() / 3) && p.x <= (my_lcd.Get_Display_Width() / 3) * 2) && (p.y < (my_lcd.Get_Display_Height() / 3) * 2 && p.y > (my_lcd.Get_Display_Height() / 3))) //4
      {
        if (board[4] == 0)
        {
          Serial.println("Player Move: 4");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[4] = 1;
          drawPlayerMove(4);
        }
      }

      else if ((p.x > (my_lcd.Get_Display_Width() / 3) * 2) && (p.y < (my_lcd.Get_Display_Height() / 3) * 2 && p.y > (my_lcd.Get_Display_Height() / 3))) //5
      {
        if (board[5] == 0)
        {
          Serial.println("Player Move: 5");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[5] = 1;
          drawPlayerMove(5);
        }
      }

      else if ((p.x > (my_lcd.Get_Display_Width() / 3) && p.x <= (my_lcd.Get_Display_Width() / 3) * 2) && (p.y > 150)) //7
      {
        if (board[7] == 0)
        {
          Serial.println("Player Move: 7");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[7] = 1;
          drawPlayerMove(7);
        }
      }

      else if ((p.x > (my_lcd.Get_Display_Width() / 3) * 2) && (p.y > (my_lcd.Get_Display_Height() / 3) * 2)) //8
      {
        if (board[8] == 0)
        {
          Serial.println("Player Move: 8");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[8] = 1;
          drawPlayerMove(8);
        }
      }
    }
  } while (p.z < ts.pressureThreshhold);
}

void playGame()
{
  do
  {
    if (moves % 2 == 1)
    {
      arduinoMove();
      printBoard();
      checkWinner();
    }
    else
    {
      playerMove();
      printBoard();
      checkWinner();
    }
    moves++;
  } while (winner == 0 && moves < 10);
  if (winner == 1)
  {
    Serial.println("HUMAN WINS");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  }
  else if (winner == 2)
  {
    Serial.println("CPU WINS");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  }
  else
  {
    Serial.println("DRAW");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  }
}

int checkOpponent()
{
  if (board[0] == 1 && board[1] == 1 && board[2] == 0)
    return 2;
  else if (board[0] == 1 && board[1] == 0 && board[2] == 1)
    return 1;
  else if (board[1] == 1 && board[2] == 1 && board[0] == 0)
    return 0;
  else if (board[3] == 1 && board[4] == 1 && board[5] == 0)
    return 5;
  else if (board[4] == 1 && board[5] == 1 && board[3] == 0)
    return 3;
  else if (board[3] == 1 && board[4] == 0 && board[5] == 1)
    return 4;
  else if (board[1] == 0 && board[4] == 1 && board[7] == 1)
    return 1;
  else
    return 100;
}

void arduinoMove()
{
  int b = 0;
  int counter = 0;
  int movesPlayed = 0;
  Serial.print("\nArduino Move:");

  int firstMoves[] = {0, 2, 6, 8}; // will use these positions first

  for (counter = 0; counter < 4; counter++) //Count first moves played
  {
    if (board[firstMoves[counter]] != 0) // First move is played by someone
    {
      movesPlayed++;
    }
  }
  do
  {
    if (moves <= 2)
    {
      int randomMove = random(4);
      int c = firstMoves[randomMove];

      if (board[c] == 0)
      {
        delay(1000);
        board[c] = 2;
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b = 1;
      }
    }
    else
    {
      int nextMove = checkOpponent();
      if (nextMove == 100)
      {
        if (movesPlayed == 4) //All first moves are played
        {
          int randomMove = random(9);
          if (board[randomMove] == 0)
          {
            delay(1000);
            board[randomMove] = 2;
            Serial.print(randomMove);
            Serial.println();
            drawCpuMove(randomMove);
            b = 1;
          }
        }
        else
        {
          int randomMove = random(4);
          int c = firstMoves[randomMove];

          if (board[c] == 0)
          {
            delay(1000);
            board[c] = 2;
            Serial.print(firstMoves[randomMove]);
            Serial.println();
            drawCpuMove(firstMoves[randomMove]);
            b = 1;
          }
        }
      }
      else
      {
        delay(1000);
        board[nextMove] = 2;
        drawCpuMove(nextMove);
        b = 1;
      }
    }
  } while (b < 1);
}

void checkWinner()
// checks board to see if there is a winner
// places result in the global variable 'winner'
{
  int qq = 0;
  // noughts win?
  if (board[0] == 1 && board[1] == 1 && board[2] == 1)
  {
    winner = 1;
  }
  if (board[3] == 1 && board[4] == 1 && board[5] == 1)
  {
    winner = 1;
  }
  if (board[6] == 1 && board[7] == 1 && board[8] == 1)
  {
    winner = 1;
  }
  if (board[0] == 1 && board[3] == 1 && board[6] == 1)
  {
    winner = 1;
  }
  if (board[1] == 1 && board[4] == 1 && board[7] == 1)
  {
    winner = 1;
  }
  if (board[2] == 1 && board[5] == 1 && board[8] == 1)
  {
    winner = 1;
  }
  if (board[0] == 1 && board[4] == 1 && board[8] == 1)
  {
    winner = 1;
  }
  if (board[2] == 1 && board[4] == 1 && board[6] == 1)
  {
    winner = 1;
  }
  // crosses win?
  if (board[0] == 2 && board[1] == 2 && board[2] == 2)
  {
    winner = 2;
  }
  if (board[3] == 2 && board[4] == 2 && board[5] == 2)
  {
    winner = 2;
  }
  if (board[6] == 2 && board[7] == 2 && board[8] == 2)
  {
    winner = 2;
  }
  if (board[0] == 2 && board[3] == 2 && board[6] == 2)
  {
    winner = 2;
  }
  if (board[1] == 2 && board[4] == 2 && board[7] == 2)
  {
    winner = 2;
  }
  if (board[2] == 2 && board[5] == 2 && board[8] == 2)
  {
    winner = 2;
  }
  if (board[0] == 2 && board[4] == 2 && board[8] == 2)
  {
    winner = 2;
  }
  if (board[2] == 2 && board[4] == 2 && board[6] == 2)
  {
    winner = 2;
  }
}
