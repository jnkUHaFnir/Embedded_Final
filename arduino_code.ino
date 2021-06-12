#define C 262 // 도 
#define D 294 // 레 
#define E 330 // 미 
#define F 349 // 파 
#define G 392 // 솔 
#define A 440 // 라 
#define B 494 // 시

char Cur_tone;

int dataPin = 13;
int latchPin = 12;
int clockPin = 11;

String songstr;
int notecount = 0;

int button_C = 9;
int button_D = 8;
int button_E = 7;
int button_F = 6;
int button_G = 5;
int button_A = 4;
int button_B = 3;

int piezo = 10;
int reset_switch = A0;

int noteToBin(char note){ //song 배열 속 주파수 값을 피아노 led불 위치 bit로 변환
  switch(note)
    {
      case 'C':
        return 0b00000001;
      case 'D':
        return 0b00000010;
      case 'E':
        return 0b00000100;
      case 'F':
        return 0b00001000;
      case 'G':
        return 0b00010000;
      case 'A':
        return 0b00100000;
      case 'B':
        return 0b01000000;
    }
}

char playNote(){//버튼 입력 감지 후 음 출력, 출력한 음 반환
  
    //초기화, 안하면 계속 같은음 출력해버림
  bool buttonstate_C = 0;
  bool buttonstate_D = 0;
  bool buttonstate_E = 0;
  bool buttonstate_F = 0;
  bool buttonstate_G = 0;
  bool buttonstate_A = 0;
  bool buttonstate_B = 0;
  int buttonstate_reset = 0;
  
  Cur_tone = 0; 
  
  buttonstate_C = !digitalRead(button_C);
  buttonstate_D = !digitalRead(button_D);
  buttonstate_E = !digitalRead(button_E);
  buttonstate_F = !digitalRead(button_F);
  buttonstate_G = !digitalRead(button_G);
  buttonstate_A = !digitalRead(button_A);
  buttonstate_B = !digitalRead(button_B);
  buttonstate_reset = digitalRead(reset_switch); 
  // 건반 눌린게 있으면 음 출력하고 음 return (X = reset)
  // 건반 눌린게 없으면 빠르게 return 0
    if (buttonstate_C)
  {
    Cur_tone = 'C';
    tone(piezo, C, 100);
  } 
  else if (buttonstate_D)
  {
    Cur_tone = 'D';
    tone(piezo, D, 100);
  }
  else if (buttonstate_E)
  {
    Cur_tone = 'E';
    tone(piezo, E, 100);
  }
  else if (buttonstate_F)
  {
    Cur_tone = 'F';
    tone(piezo, F, 100);
  }
  else if (buttonstate_G)
  {
    Cur_tone = 'G';
    tone(piezo, G, 100);
  }
  else if (buttonstate_A)
  {
    Cur_tone = 'A';
    tone(piezo, A, 100);
  }
  else if (buttonstate_B)
  {
    Cur_tone = 'B';
    tone(piezo, B, 100);
  }
   else if (buttonstate_reset)
  {
    Cur_tone = 'X';
    return Cur_tone;
  }
  else
  {
    return 0;
  }

  delay(300);
  return Cur_tone;
}


void setup()
{
  pinMode(button_C, INPUT_PULLUP);
  pinMode(button_D, INPUT_PULLUP);
  pinMode(button_E, INPUT_PULLUP);
  pinMode(button_F, INPUT_PULLUP);
  pinMode(button_G, INPUT_PULLUP);
  pinMode(button_A, INPUT_PULLUP);
  pinMode(button_B, INPUT_PULLUP);
  pinMode(piezo, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(reset_switch, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop()
{ 
  /**********song[] 문자열 관련 코드**********/

  while(Serial.available()) {
    char wait = Serial.read();
    songstr.concat(wait);
    notecount++;

    // Serial.read()로 문자열을 받아서 song_str 스트링에 concatenate시킴.
  }
   
  /**************본체 관련 코드**************/
  
  int reset_data = 0b00000000;
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, reset_data);
  digitalWrite(latchPin, HIGH);

  if (digitalRead(reset_switch) == HIGH)
  { 
    for(int i = 0; i < notecount; i++)
    {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, noteToBin(songstr.charAt(i)));
      digitalWrite(latchPin, HIGH);//LED점등
      
      while(1)
      {
        char pressedNote = playNote();

        //누른 건반이 led가 표시한 건반이면 다음 음으로
        if(pressedNote == songstr.charAt(i))
        {
         
          break;
        }
        else if(pressedNote == 'X'){  
            i=-1;
      
            break;
          }
      }
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, reset_data);
    digitalWrite(latchPin, HIGH);
    delay(100);
    }
  }
}
