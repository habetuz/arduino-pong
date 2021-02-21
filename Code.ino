int score[2];

class Screen //Klasse für den Bilschirm
{
  public: boolean state[14][8]; //Boolean Array mit Status der LED's
  public: int width; //* Breite und Höhe des Bildschirms (wird in setup() berechnet)
  public: int height;//**
  public: void setup(int pinX[], int pinY[], int pinBarLeft[], int pinBarRight[]) //Setup funktion des Bilschirms. Bekommt Int Arrays mit allen nötigen Pins.
  {
    width = static_cast<int>(sizeof(state) / sizeof(state[0])); 	  //* Berechnet Breite und Höhe des Bildschirms.
    height = static_cast<int>(sizeof(state[0]) / sizeof(state[0][0]));//*
  }
};

Screen screen; //Ein Bilschrim wird erstellt.

class Bar //Klasse für die Balken an den Seiten des Bilschirms
{
  char inputPin; // Input Pin, an den der Drehknopf, mit dem man den Balken steuert angeschlossen ist.
  int column; // Spalte, auf der der Balken auf dem Bildschirm gezeigt wird.
  public: int pos; // Position des Balkens
  public: void setup(int mColumn, char mInputPin) // Setup Funktion der Balken. Bekommt Spalte und Input Pin.
  {
    inputPin = mInputPin;
    column = mColumn;
  }
  
  public: void show() // Loop Funktion für den Balken
  {
    pos = analogRead(inputPin)/171 + 1;
    /*
	* analogRead(inputPin): Bekommt den Wert des Drehknopfes (zwischen 0 und 1023).
    * .../171: Rechnet den Drehknopf Wert durch 171, um einen Wert zwischen 0 und 6 zu bekommen (6 wird niemals erreicht, da immer abgerundet wird).
    * ...+1: Wie nachfolgend erklärt darf die Position nicht auf 0 sein. Durch das +1 wird eine 0 verhindert.
    *		  	*******************************************************************************************
    *		  	* Warum 6 Werte?																		                                        *
    *		  	* 	XOOOOOOX																			                                          *
    *    	  	* Die Position darf nur auf den O's sein, da sonst eine LED vom Bildschirm verschwindet.  *
    *		  	*******************************************************************************************
    */
    
    for(int i = 0; i < screen.height; i++) // Schaltet alle LED's des Balkens aus.
    {
      screen.state[column][i] = false;
    }
    screen.state[column][pos -1] = true; // Schaltet die LED unter, über und an der Position des Balkens an.
    screen.state[column][pos] = true;
    screen.state[column][pos +1] = true;
  }
};

Bar barLeft;
Bar barRight;

class Pong // Klasse für den Ball
{
  const int LEFT = 0;
  const int RIGHT = 1;
  
  int prePosX; //* Position vom vorigen Frame
  int prePosY; //**
  float posX; //* Position des Balls
  float posY; //**
  float velocity; // Insgesamte Geschwindigkeit des Balls
  float vX; //* X und Y Geschwindigkeit des Balls
  float vY; //**
  
  public: void setup() // Setup Funktion des Balls
  {
	reset();
  }
  
  public: void show()
  {
    posX = posX + vX;
    posY = posY + vY;

    if(posY > (screen.height - 1))
    {
      posY = (screen.height - 1) - (posY - (screen.height - 1));
      vY *= -1;
    } else if(posY < 0) 
    {
      posY = - posY;
      vY *= -1;
    }
    
    if(posX > screen.width - 2)
    {
      if(barCatches(RIGHT))
      {
        posX = (screen.width - 2) - (posX - (screen.width - 2));
        vX *= -1;
      } else
      {
        score[1] ++;
        screen.state[prePosX][prePosY] = false;
        screen.state[screen.width - 1][(int)posY] = true;
        showLCD();
        showScreen();
        delay(500);
        screen.state[screen.width - 1][(int)posY] = false;
        showLCD();
        showScreen();
        delay(500);
        screen.state[screen.width - 1][(int)posY] = true;
        showLCD();
        showScreen();
        delay(500);
        screen.state[(int)posX][(int)posY] = false;
        showLCD();
        showScreen();
        reset();
      }
    } else if(posX < 1) 
    {
      if(barCatches(LEFT))
      {
        posX = 1 + (1 - posX);
        vX *= -1;
      } else
      {
        score[0] ++;
        screen.state[prePosX][prePosY] = false;
        screen.state[0][(int)posY] = true;
        showLCD();
        showScreen();
        delay(500);
        screen.state[0][(int)posY] = false;
        showLCD();
        showScreen();
        delay(500);
        screen.state[0][(int)posY] = true;
        showLCD();
        showScreen();
        delay(500);
        screen.state[0][(int)posY] = false;
        showLCD();
        showScreen();
        reset();
      }
    }

    screen.state[prePosX][prePosY] = false;
    screen.state[(int)(posX + 0.5)][(int)(posY+0.5)] = true; 
    prePosX = (int)(posX + 0.5);
    prePosY = (int)(posY+0.5);
  }
  
  private: void reset(){
	posX = screen.width /2.0;   //* Bildschirm Höhe und Breite wird durch 2 geteilt, um den Ball möglichst mittig zu plazieren.
    posY = screen.height / 2.0; //**
    velocity = 1; // Startgeschwindigkeit entspricht 1.
    vY = (velocity * (random(20,81) / 100.0));
    /*
    * random(20,81)/100: Eine zufällige Zahl zwischen 20 und 80 wird durch 100 gerechnet, um als Prozent genutzt zu werden.
    * velocity*...: 20 bis 80 Prozent der Gesamtgeschwindigkeit wird vY zugewiesen.
    */
    
    vX = pow(pow(velocity, 2) - pow(vY, 2), 0.5);
    /*
    * pow(velocity, 2) - pow(vY, 2): velocity²(c²) - vY²(a²) = vX²(b²)
    * pow(..., 0.5): √vX²(√b²)
    * 	*****************************
    *	* Warum Satz des Pythagoras?*
    *	* vY|\velocity				*
    *	*   |_\						*
    *	*	vX						*
    *	*****************************
    */
    
    switch(random(4)) // Da bis jetzt nur die Geschwindigkeit ausgerechnet wurde, müssen wir noch die Richtung auswählen.
    {
      case 0:
      	vX *= -1;
      	vY *= -1;
      	break;
      case 1:
      	vY *= -1;
      	break;
      case 2:
      	break;
      case 3:
      	vX *= -1;
      break;
      /*
      *********************
      * <-- 0		1 --> *
      *					  *
      *					  *
      * <-- 3		2 --> *
      *********************
      */
    }
  }
  
  private: boolean barCatches(int bar)
  {
    if(bar == RIGHT) 
    {
      int y = ((vY / vX)*((screen.width - 1) - posX) + posY) + 0.5;
      return y == barRight.pos - 1 || y == barRight.pos || y == barRight.pos + 1; 
    } else if(bar == LEFT)
    {
      int y = ((vY / vX)*((screen.width - 1) - posX) + posY) + 0.5;
      return y == barLeft.pos - 1 || y == barLeft.pos || y == barLeft.pos + 1; 
    }
  }
};

Pong pong;

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(A5));
  int screenPinsX[] = {0,1,2,3,4,5,6,7,8};
  int screenPinsY[] = {9,10,11,12,13,14,15};
  screen.setup(screenPinsX,screenPinsY, 0, 0);
  barLeft.setup(0, A1);
  barRight.setup(13, A0);
  pong.setup();
}

void loop()
{
  barLeft.show();
  barRight.show();
  pong.show();
  showLCD();
  showScreen();
  delay(500);
}

void showScreen(){
  for(int row = 0; row < screen.height; row++) {
    for(int column = 0; column < screen.width; column++) {
      if(screen.state[column][row]) {
        Serial.print("O");
      } else {
        Serial.print(".");
      }
    }
    Serial.print("\n");
  }
  Serial.print("--------------\n");
}

void showLCD(){
  Serial.print(score[0]);
  Serial.print("            ");
  Serial.print(score[1]);
  Serial.print("\n");
}
