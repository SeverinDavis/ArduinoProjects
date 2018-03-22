typedef enum{
  idle,
  start,
  data_h,
  data_l,
  done,
} state_t;

typedef enum{
  on    = 0b1100000000111111,
  off   = 0b0100000010111111,
  up    = 0b0000000011111111,
  down  = 0b1000000001111111,
  white = 0b1110000000011111,
  red   = 0b0010000011011111,
  blue  = 0b0110000010011111,
  green = 0b1010000001011111,
  ro    = 0b0001000011101111,
  o     = 0b0011000011001111,
  yo    = 0b0000100011110111,
  y     = 0b0010100011010111,
  lg    = 0b1001000001101111,
  trq   = 0b1011000001001111,
  aqua  = 0b1000100001110111,
  lb    = 0b1010100001010111,
  purp  = 0b0101000010101111,
  mag   = 0b0111000010001111,
  rose  = 0b0100100010110111,
  pink  = 0b0110100010010111,
  flash = 0b1101000000101111,
  strob = 0b1111000000001111,
  fade  = 0b1100100000110111,
  smoot = 0b1110100000010111
} cmd_t;

int pstate = 0;
volatile state_t state = idle;

volatile unsigned int counter = 0;
volatile unsigned int data_counter = 0;

unsigned int data_lo = 0b0100000010111111;
unsigned int data_hi = 0b0000000011110111;


ISR(TIMER2_COMPB_vect){  // Interrupt service routine to pulse the modulated pin 3
  switch(state)
  {
    case idle:
    {
      TCCR2A &= ~_BV(COM2B1);
      digitalWrite(4, 0);
      break;
    }
    case start:
    {
      if(counter < 340)
      {
        TCCR2A |= _BV(COM2B1);
        digitalWrite(4, 1);
        counter++;
      }
      else if(counter < 340 + 170)
      {
        TCCR2A &= ~_BV(COM2B1);
        digitalWrite(4, 0);
        counter++;
        if(!(counter < 340 + 170))
        {
          state = data_h;
          counter = 0;
        }
      }
      break;
    }
    case data_h:
    {
      int current_bit = (data_hi >> (15 - data_counter)) & 0x01;

      if(counter < 22)
      {
        TCCR2A |= _BV(COM2B1);
        digitalWrite(4, 1);
        counter++;
      }
      else if(counter < 44 + current_bit*44)
      {
        TCCR2A &= ~_BV(COM2B1);
        digitalWrite(4, 0);
        counter++;
        if(!(counter < 44+current_bit*44))
        {
          counter = 0;
          data_counter++;
          if(data_counter == 16)
          {
            state = data_l;
            counter = 0;
            data_counter = 0;
          }
        }
      }
        break;
    }
    case data_l:
    {
      int current_bit = (data_lo >> (15 - data_counter)) & 0x01;

      if(counter < 22)
      {
        TCCR2A |= _BV(COM2B1);
        digitalWrite(4, 1);
        counter++;
      }
      else if(counter < 44 + current_bit*44)
      {
        TCCR2A &= ~_BV(COM2B1);
        digitalWrite(4, 0);
        counter++;
        if(!(counter < 44+current_bit*44))
        {
          counter = 0;
          data_counter++;
          if(data_counter == 16)
          {
            state = done;
            counter = 0;
            data_counter = 0;
          }
        }
      }
        break;
    }
    case done:
    {
      if(counter < 22)
      {
        TCCR2A |= _BV(COM2B1);
        digitalWrite(4, 1);
        counter++;
      }
      else if(counter < 44 + 15437)
      {
        TCCR2A &= ~_BV(COM2B1);
        digitalWrite(4, 0);
        counter++;
        if(!(counter < 44 + 1537))
        {
          state = idle;
          counter = 0;
          data_counter = 0;
        }
      }
      break;
    }
  }
}


void setIrModOutput()
{
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, 0);
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS22);
  OCR2A = 52; // defines the frequency 51 = 38.4 KHz, 54 = 36.2 KHz, 58 = 34 KHz, 62 = 32 KHz
  OCR2B = 21;  // deines the duty cycle - Half the OCR2A value for 50%
  TCCR2B = TCCR2B & 0b00111000 | 0x2; // select a prescale value of 8:1 of the system clock
  TCCR2A &= ~_BV(COM2B1);
}

void setup(){
  setIrModOutput();
  TIMSK2 = _BV(OCIE2B);
  Serial.begin(9600); 
}

void loop(){
  if (Serial.available() > 0) 
  {
    char incomingByte = Serial.read();

    switch(incomingByte)
    {
      case 'a':
      {
        data_lo = on;
        state = start;
        break;
      }  
      case 'z':
      {
        data_lo = off;
        state = start;
        break;
      }  
      case 's':
      {
        data_lo = up;
        state = start;
        break;
      } 
      case 'x':
      {
        data_lo = down;
        state = start;
        break;
      } 
      case 'w':
      {
        data_lo = white;
        state = start;
        break;
      } 
      case 'r':
      {
        data_lo = red;
        state = start;
        break;
      } 
      case 't':
      {
        data_lo = ro;
        state = start;
        break;
      } 
      case 'y':
      {
        data_lo = o;
        state = start;
        break;
      } 
      case 'u':
      {
        data_lo = yo;
        state = start;
        break;
      } 
      case 'i':
      {
        data_lo = y;
        state = start;
        break;
      } 
      case 'g':
      {
        data_lo = green;
        state = start;
        break;
      }
      case 'h':
      {
        data_lo = lg;
        state = start;
        break;
      }
      case 'j':
      {
        data_lo = trq;
        state = start;
        break;
      }
      case 'k':
      {
        data_lo = aqua;
        state = start;
        break;
      }
      case 'l':
      {
        data_lo = lb;
        state = start;
        break;
      }
      case 'b':
      {
        data_lo = blue;
        state = start;
        break;
      }
      case 'n':
      {
        data_lo = purp;
        state = start;
        break;
      }
      case 'm':
      {
        data_lo = mag;
        state = start;
        break;
      }
      case ',':
      {
        data_lo = rose;
        state = start;
        break;
      }
      case '.':
      {
        data_lo = pink;
        state = start;
        break;
      }
      case '1':
      {
        data_lo = flash;
        state = start;
        break;
      }
      case '2':
      {
        data_lo = strob;
        state = start;
        break;
      }
      case '3':
      {
        data_lo = fade;
        state = start;
        break;
      }
      case '4':
      {
        data_lo = smoot;
        state = start;
        break;
      }
    }
  }
}

