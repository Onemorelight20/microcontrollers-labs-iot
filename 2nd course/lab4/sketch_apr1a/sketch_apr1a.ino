const int buzzerPin = 34;
const int button1pin = 52, button2pin = 51, button3pin = 50, button4pin = 10;

unsigned char number[10]  = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90}; // 0 - 9
unsigned char time_tbd_7segment[6] = {number[0], number[0], number[0], number[0], number[0], number[0]};

struct Time
{
  unsigned char second, minute, hour;
};

Time stopwatch = {0, 0, 0};
unsigned char isStopwatchTurnedOn = 1;
unsigned char isMainStopwatchReset = 0;

Time MEMORIZED_TIME[10];
unsigned char memory_pos_write_to = 0;
unsigned char isOverflowInMemorized = 0;

unsigned char key_cursor = 0b00000100; // position of number to refresh
unsigned char fig_number = 0; // position in time_tbd_7segment array
unsigned char timer2_interrupt_flag = 1;

void convert_decimal_to_binary(unsigned char number_to_convert, unsigned char position);

ISR(TIMER1_COMPA_vect)  // Таймер Т1 по співпадінню А, кожної 1 сек. (для відліку часу)
{
  if(isStopwatchTurnedOn) {
    if (++stopwatch.second == 60) {
      stopwatch.second = 0;
      if (++stopwatch.minute == 60) {
        stopwatch.minute = 0;
        if (++stopwatch.hour == 24)
          stopwatch.hour = 0; 
      } 
    }
    refresh_7segm_array_nums(&stopwatch);
  }
}

ISR(TIMER2_OVF_vect){  // Таймер Т2 по переповненню. (для оновлення 7-сегментника)
  if (timer2_interrupt_flag) {
    key_cursor <<= 1;
    key_cursor &= 0b11111100;
    if(key_cursor==0) key_cursor = 0b00000100;

    // змінюємо число на відображення, відповідно до позиції key_cursor time_tbd_7segment[0_1_.2_3_.4_5_]
    fig_number++;
    if (fig_number == 6) fig_number = 0;

    PORTD = key_cursor; // змінюємо пін для подачі високого рівня (+5V) у конкретний момент часу
    PORTA = time_tbd_7segment[fig_number];
    timer2_interrupt_flag = 0;
  } else {
    timer2_interrupt_flag = 1;
  }
}

void setup() {
  pinMode(button1pin, INPUT);
  digitalWrite(button1pin, HIGH);
  pinMode(button2pin, INPUT);
  digitalWrite(button2pin, HIGH);
  pinMode(button3pin, INPUT);
  digitalWrite(button3pin, HIGH);
  pinMode(button4pin, INPUT);
  digitalWrite(button4pin, HIGH);

  noInterrupts();  // disable all interrupts

  // Порт D - керування живленням (анод)
	DDRD = 0xFF;  		// порт працює на вихід 
	PORTD = key_cursor; // високий рівень на піні key_cursor у конкретний момент часу

  // Порт А - сегменти (катод)
	DDRA = 0xFF;  		// порт працює на вихід 
	PORTA = 0xFF; 	  // +5V (семисегм. не світять)  

  // TIMER#1: Скид при співпадінні OCR1A (1sec) 
  TCCR1A = 0x00;
  TCCR1B = (1 << WGM12) | (1 << CS12); // | (1 << CS10) CTC mode & Prescaler CURRENTLY 256 (@ 1024)
  OCR1A = 0x3D08;// compare value = 1 sec (16MHz AVR)
  TIMSK1 |= (1 << OCIE1A); // Включаємо переривання по співпадінню з OCR1A

  // TIMER#2 Used for 7-segment display refresh
  TCNT2 = 0; // current count value of the timer set to 0
  TCCR2A = 0x00;
  TCCR2B = (1 << CS01);  // Prescaler CLKio / 8 (interrupt each 2MHz)
  TIMSK2 |= (1 << TOIE2);  // Enable timer2 overflow interrupt(TOIE2)

  interrupts();  // Enable global interrupts

  //Buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}


void loop() {
  if(digitalRead(button1pin) == LOW) {
    if (isStopwatchTurnedOn) {
      isStopwatchTurnedOn = 0;
    } else {
      if (isMainStopwatchReset) {
        isStopwatchTurnedOn = 1;
        isMainStopwatchReset = 0;
      } else {
        reset_time(&stopwatch);
        isStopwatchTurnedOn = 0;
        isMainStopwatchReset = 1;
        refresh_7segm_array_nums(&stopwatch);
      }
    }

    delay(200);
  } else if(digitalRead(button2pin) == LOW) {
    if (isStopwatchTurnedOn) {
      if (memory_pos_write_to >= 10) {
        memory_pos_write_to = 0;
        isOverflowInMemorized = 1;
      }
      Time timeToMemorize = {stopwatch.second, stopwatch.minute, stopwatch.hour};
      MEMORIZED_TIME[memory_pos_write_to++] = timeToMemorize;
    }

    delay(200);
  } else if(digitalRead(button3pin) == LOW) {
    for(int i = 0; i < 10; i++){ 
      reset_time(&MEMORIZED_TIME[i]);
    }
    memory_pos_write_to = 0;

    delay(200);
  } else if(digitalRead(button4pin) == LOW) {
    if (!isStopwatchTurnedOn) {
      unsigned char memorizedValues = isOverflowInMemorized ? 10 : memory_pos_write_to;
      for(int i = 0; i < memorizedValues; i++){ 
      refresh_7segm_array_nums(&MEMORIZED_TIME[i]);
      delay(300);
      }

      refresh_7segm_array_nums(&stopwatch);
    }

    delay(200);
  }

  if ((stopwatch.second + 1 == 60) && (isStopwatchTurnedOn))
    buzz_for(200);
}

void refresh_7segm_array_nums(Time *time) {
  convert_decimal_to_binary(time->hour, 0);
  convert_decimal_to_binary(time->minute, 2);
  convert_decimal_to_binary(time->second, 4);
}

// updates time to be displayed on 7-segment
// INPUT: 10-based number and position in time_tbd_7segment array
void convert_decimal_to_binary(unsigned char number_to_convert, unsigned char position){
  unsigned char bcdL = number_to_convert;
  unsigned char bcdH = 0;
  while(bcdL>=10){
    bcdL -= 10;
    bcdH++;
  }
  time_tbd_7segment[position] = number[bcdH];
  time_tbd_7segment[position+1] = number[bcdL];

  // to display dot separators (HH.MM.SS)
  if ((position + 1 == 1) || (position + 1 == 3)) {
    time_tbd_7segment[position+1] &= ~(1 << 7);
  }
}


void reset_time(Time *time){
  time->second = 0;
  time->minute = 0;
  time->hour = 0;
}

void buzz_for(int ms_duration) {
  digitalWrite(buzzerPin, HIGH);
  delay(ms_duration);
  digitalWrite(buzzerPin, LOW);
}
