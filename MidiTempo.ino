/*
 * Midi/USB BPM display
 * Use 7 segment LED display common Anode with 2N3906 PNP transistors and 74HC595 shift register.
 * H11L1 opto-coupler and 74HC14 inverter.
 * 
 * 2021-04-27
 * tek465b.github.io
 */


#define clockPin 5
#define dataPin 2
#define latch 4
#define OEPin 3
#define MRPin 6
#define Digit1 12
#define Digit2 7
#define Digit3 8
#define Digit4 11

byte midi = 0;
byte ccount = 0;
byte current_digit;
unsigned long mymicros;
unsigned long previousmicros = 0;
unsigned long bpm = 0;

void disp(byte number, bool dec_point = 0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(31250);
  pinMode(Digit1, OUTPUT);
  pinMode(Digit2, OUTPUT);
  pinMode(Digit3, OUTPUT);
  pinMode(Digit4, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(OEPin, OUTPUT);
  pinMode(MRPin, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(9, INPUT_PULLUP); //Set unused IO as input pullup
  pinMode(10, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT_PULLUP);
  pinMode(A7, INPUT_PULLUP);
  digitalWrite(13, HIGH);
  


  digitalWrite(OEPin, LOW);
  digitalWrite(MRPin, HIGH);

  digit_off();

  cli();
  TCCR1A = 0;   //Timer interrupt for the display
  TCCR1B = 1;
  TCNT1  = 0;
  TIMSK1 = 1;

  /*TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  OCR2A = 255;  //255 for 512us, 159 for 320us max midi rate
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS21); //cs22 and remove cs21 and cs20 for 64 prescaler, 124 ocr2a for 500us, 256 for 1024us
  TCCR2B |= (1 << CS20);
  TIMSK2 |= (1 << OCIE2A);*/
  sei();

}

ISR(TIMER1_OVF_vect)
{
  digit_off();
 
  switch (current_digit)
  {
    case 1:
    if(bpm<1000)  //if bpm is under 100 turn off the first digit instead of displaying 0.
    {
      disp(10);
      digitalWrite(Digit1, LOW);
    }
    else
    {
      disp(bpm/1000); //Else display digit and turn on corresponding transistor.
      digitalWrite(Digit1, LOW);
    }
      break;
 
    case 2:
    if(bpm<100)
    {
      disp(10);
      digitalWrite(Digit2, LOW);
    }
    else
    {
      disp((bpm/100)%10);
      digitalWrite(Digit2, LOW);
    }
      break;
 
    case 3:
      disp((bpm/10)%10, true );
      digitalWrite(Digit3, LOW);
      break;
 
    case 4:
      disp(bpm%10);
      digitalWrite(Digit4, LOW);
      break;
  }
 
  current_digit = (current_digit % 4) + 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  CheckMidi();


}

/*ISR(TIMER2_COMPA_vect) {//checks for incoming midi every 128us
    CheckMidi();
}*/

void CheckMidi(){
  if (Serial.available() > 0 ){
    midi = Serial.read();
    if (midi == 0xF8) { //If serial data is available read it and check for the midi clock byte 0xF8 or 248 in decimal, char alt+0248 = ø on serial using windows 1252 charset.
      ccount += 1;
    if (ccount == 24) { //once we get 24 clock per quarter note we get the time in microS and convert to bpm.
      digitalWrite(13, HIGH);
      ccount = 0;
      mymicros = micros() - previousmicros;
      previousmicros = micros();
      bpm = 600000000 / mymicros ; //x10 1200 = 120.0
      digitalWrite(13, LOW);
    }
    }
  }
}

void disp(byte number, bool dec_point)
{
  switch (number)
  {
    case 0:  // print 0
      shiftOut(dataPin, clockPin, MSBFIRST, 0x02 | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 1:  // print 1
      shiftOut(dataPin, clockPin, MSBFIRST, 0x9E | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 2:  // print 2
      shiftOut(dataPin, clockPin, MSBFIRST, 0x24 | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 3:  // print 3
      shiftOut(dataPin, clockPin, MSBFIRST, 0x0C | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 4:  // print 4
      shiftOut(dataPin, clockPin, MSBFIRST, 0x98 | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 5:  // print 5
      shiftOut(dataPin, clockPin, MSBFIRST, 0x48 | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 6:  // print 6
      shiftOut(dataPin, clockPin, MSBFIRST, 0x40 | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
    
    case 7:  // print 7
      shiftOut(dataPin, clockPin, MSBFIRST, 0x1E | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 8:  // print 8
      shiftOut(dataPin, clockPin, MSBFIRST, !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
 
    case 9:  // print 9
      shiftOut(dataPin, clockPin, MSBFIRST, 0x08 | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
      
    case 10:  // print Blank
      shiftOut(dataPin, clockPin, MSBFIRST, 0xFE | !dec_point);
      digitalWrite(latch, HIGH);
      digitalWrite(latch, LOW);
      break;
  }
}

void digit_off()  //Turn all digit off.
{
   digitalWrite(Digit1, HIGH);
   digitalWrite(Digit2, HIGH);
   digitalWrite(Digit3, HIGH);
   digitalWrite(Digit4, HIGH);
}
