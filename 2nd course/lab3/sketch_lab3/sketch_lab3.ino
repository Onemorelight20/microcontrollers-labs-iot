#include <LiquidCrystal.h>

#define DDR_KEYPAD  DDRA
#define PORT_KEYPAD PORTA
#define PIN_KEYPAD  PINA
#include "keypad4x4.h"

const int buzzerPin = 21;
int buzzerCount = 10;
bool currentSecondBuzzer = true;
bool buzzerTurnedOn = false;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to. 8-bit LCD => 8 data pins
const int rs = 11, rw = 12, en = 13, d0 = 37, d1 = 36, d2 = 35, d3 = 34, d4 = 33, d5 = 32, d6 = 31, d7 = 30;
LiquidCrystal lcd(rs, rw, en, d0, d1, d2, d3, d4, d5, d6, d7);

const char* TIMER_OVERFLOW = {"EE"};
char* MSG = {""};

const PROGMEM char sixty[60][3] = {
  {"00"}, {"01"}, {"02"}, {"03"}, {"04"}, {"05"}, {"06"}, {"07"}, {"08"}, {"09"},
  {"10"}, {"11"}, {"12"}, {"13"}, {"14"}, {"15"}, {"16"}, {"17"}, {"18"}, {"19"},
  {"20"}, {"21"}, {"22"}, {"23"}, {"24"}, {"25"}, {"26"}, {"27"}, {"28"}, {"29"},
  {"30"}, {"31"}, {"32"}, {"33"}, {"34"}, {"35"}, {"36"}, {"37"}, {"38"}, {"39"},
  {"40"}, {"41"}, {"42"}, {"43"}, {"44"}, {"45"}, {"46"}, {"47"}, {"48"}, {"49"},
  {"50"}, {"51"}, {"52"}, {"53"}, {"54"}, {"55"}, {"56"}, {"57"}, {"58"}, {"59"}
};

bool settingTimer1 = false, settingTimer2 = false;
bool activeTimer1 = false, activeTimer2 = false;
unsigned char currentTimerBlock = "";
long bufferTimer = 0;
int currentBlockPosition = 0;

struct Time
{
  char second, minute, hour;
};

Time timer1 = {0, 0, 0};
Time timer2 = {0, 0, 0};

void LCD_WriteStrPROGMEM(char *str, int n)  //вивід масиву символів,
{ //записаних у флеші
  for (int i = 0; i < n; i++)
    lcd.print( (char)pgm_read_byte( &(str[i]) ) );
}

ISR(TIMER4_COMPA_vect)  // Таймер Т1 по співпадінню А, кожної 1 сек.
{
  if (activeTimer1 && !settingTimer1) {
    if (--timer1.second == -1) {
      timer1.second = 59;
      if (--timer1.minute == -1) {
        timer1.minute = 59;
        if (--timer1.hour == -1) {
          buzzerTurnedOn = true;
          reset_timer(&timer1);
          activeTimer1 = false;      
          MSG = {"T1!!"};    
        }
      }
    }
  }  

  if (activeTimer2 && !settingTimer2) {
    if (--timer2.second == -1) {
      timer2.second = 59;
      if (--timer2.minute == -1) {
        timer2.minute = 59;
        if (--timer2.hour == -1) {
          buzzerTurnedOn = true;     
          reset_timer(&timer2);
          activeTimer2 = false;      
          MSG = {"T2!!"};         
        }
      }
    }
  }  

  lcd.clear();

  lcd.setCursor(0, 0);
  print_hour_on_lcd(timer1);
  lcd.write(':');
  print_minute_on_lcd(timer1);
  lcd.write(':');
  print_second_on_lcd(timer1);

  lcd.setCursor(0, 1);
  print_hour_on_lcd(timer2);
  lcd.write(':');
  print_minute_on_lcd(timer2);
  lcd.write(':');
  print_second_on_lcd(timer2);

  buzz();
  display_msg();
}

void setup() {
  noInterrupts(); // disable all interrupts

  // Таймер#1: Скид при співпадінні OCR4A (1sec) + дільник=256
  TCCR4A = 0x00;
  TCCR4B = (1 << WGM12); // sets CTC mode 
  /*(In CTC mode, the timer counts up to a specified value, 
  and then resets to 0, triggering an interrupt when it 
  reaches the specified value.)*/
  TCCR4B |= (1 << CS12) | (1 << CS10); // set prescaler to divide the system clock by 1024
  OCR4A = 0x3D08;// compare value = 1 sec (16MHz AVR) This value determines when the timer will reset in CTC mode.
  TIMSK4 = (1 << OCIE1A); // enables the interrupt will be triggered when the timer reaches the value in OCR4A.

  //KeyPad 4x4
  initKeyPad();

  //LCD 16x2
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //Buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  interrupts();  // Enable global interrupts
}

void loop() {
    if (isButtonPressed()) {
      char pressedButton = readKeyFromPad4x4();
      char pressedNum = pressedButton - '0';
      lcd.setCursor(15, 1);
      lcd.write(pressedButton);
      short_buzz_on_func_call(1);

      if(pressedButton == 'A' && (!settingTimer2)) {
        settingTimer1 = true;
        reset_timer(&timer1);
        MSG = {"SET1"};
      } else if (pressedButton == 'B' && (!settingTimer1)) {
        settingTimer2 = true;
        reset_timer(&timer2);
        MSG = {"SET2"};
      } else if (pressedButton == 'C') {
        if (settingTimer1) {
          if (check_time_bounds_else_turn_buzzer(&timer1)) {
            activeTimer1 = true;
            MSG = {"T1ON"};
          } else {
            activeTimer1 = false;
            reset_timer(&timer1);
            MSG = {"OFLW"};
          }
          settingTimer1 = false;
          bufferTimer = 0;
        } else {
          activeTimer1 = false;
          reset_timer(&timer1);
          MSG = {"T1OF"};
        }
      } else if (pressedButton == 'D') {
        if (settingTimer2) {
          if (check_time_bounds_else_turn_buzzer(&timer2)) {
            activeTimer2 = true;
            MSG = {"T2ON"};
          } else {
            activeTimer2 = false;
            reset_timer(&timer2);
            MSG = {"OFLW"};
          }
          settingTimer2 = false;
          bufferTimer = 0;
        } else {
          activeTimer2 = false;
          reset_timer(&timer2);
          MSG = {"T2OF"};
        }
      } else if (pressedButton == 'F') {
        if (settingTimer1) {
          settingTimer1 = false;
          activeTimer1 = false;
          reset_timer(&timer1);
          MSG = {"T1OF"};
        } else if (settingTimer2) {
          settingTimer2 = false;
          activeTimer2 = false;
          reset_timer(&timer2);
          MSG = {"T2OF"};
        } 
        one_buzz_init();
        bufferTimer = 0;
      } else if (pressedButton == 'E') {
        if (settingTimer2) {
          if (check_time_bounds_else_turn_buzzer(&timer2)) {
            MSG = {"T2OK"};
          } else {
            MSG = {"OFLW"};
          }
        } else if (settingTimer1) {
          if (check_time_bounds_else_turn_buzzer(&timer1)) {
            MSG = {"T1OK"};
          } else {
            MSG = {"OFLW"};
          }
        }
      } else if (pressedNum >= 0 && pressedNum <= 9) {
          if (settingTimer1)
            process_num_click(&timer1, pressedNum);
          else if (settingTimer2)
            process_num_click(&timer2, pressedNum);  
      }
    }

}
void short_buzz_on_func_call(int times) {
  for (int i = 0; i < times; i++) {  
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
  }
}

void one_buzz_init() {
  buzzerTurnedOn = true;
  buzzerCount = 2;
}

void buzz() {
  if (buzzerTurnedOn) {
    if (currentSecondBuzzer) {
      digitalWrite(buzzerPin, HIGH);
      currentSecondBuzzer = false;
    } else {
      digitalWrite(buzzerPin, LOW);
      currentSecondBuzzer = true;
    }

    buzzerCount -= 1;
    if (buzzerCount == 0) {
      buzzerTurnedOn = false;
      // by defauld the buzzer will sound 5 times, if another value needed should be set along with buzzerTurnedOn
      buzzerCount = 10;
      digitalWrite(buzzerPin, LOW);
    }
  }
}

void display_msg(){
  lcd.setCursor(12, 0);
  lcd.print(MSG);
  MSG = {""};
}

bool check_time_bounds_else_turn_buzzer(Time *timer){
  if (timer->hour > 23 || timer->minute > 59 || timer->second > 59) {
    buzzerTurnedOn = true;
    return false;
  }
  return true;
}

void print_hour_on_lcd(Time timer) {
if (timer.hour > 23) {
  lcd.print(TIMER_OVERFLOW);
} else {
  LCD_WriteStrPROGMEM(sixty[timer.hour], 2);
}
}

void print_minute_on_lcd(Time timer) {
if (timer.minute > 59) {
  lcd.print(TIMER_OVERFLOW);
} else {
  LCD_WriteStrPROGMEM(sixty[timer.minute], 2);
}
}

void print_second_on_lcd(Time timer) {
if (timer.second > 59) {
  lcd.print(TIMER_OVERFLOW);
} else {
  LCD_WriteStrPROGMEM(sixty[timer.second], 2);
}
}

void process_num_click(Time *timer, char pressedNum) {
  bufferTimer = bufferTimer * 10 + pressedNum;
  timer->second = bufferTimer % 100;
  timer->minute = bufferTimer / 100 % 100;
  timer->hour = bufferTimer / 10000 % 100;
}

void reset_timer(Time *timer) {
  timer->second = 0;
  timer->minute = 0;
  timer->hour = 0;
}
