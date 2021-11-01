/*      Author: ffan005
 *      Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *      sound track from squid game
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 
 *      Demo: https://www.youtube.com/shorts/vX6E7UwvoEw
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start, Init, Power, Pow} state;

//double FRE[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
double FRE[18] = {329.63, 329.63, 0, 440.00, 0, 440.00, 440.00, 392.00, 0, 440.00, 0, 440.00, 0, 329.63, 0, 329.63, 0, 392.00};
unsigned char i = 0x00;

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
TCCR1B = 0x0B;
OCR1A = 125;
TIMSK1 = 0x02;
TCNT1 = 0;
_avr_timer_cntcurr = _avr_timer_M;
SREG |= 0x80;
}


void TimerOff(){
TCCR1B = 0x00;
}

void TimerISR(){
TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
_avr_timer_cntcurr--;
if(_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
  }
}

void TimerSet(unsigned long M){
_avr_timer_M = M;
_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
  static double current_frequency;
  if (frequency != current_frequency) {
    if (!frequency) { TCCR3B &= 0x08; }
    else { TCCR3B |= 0x03; }
    if (frequency < 0.954) { OCR3A = 0xFFFF; }
    else if (frequency > 31250) { OCR3A = 0x0000; }
    else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
    TCNT3 = 0;
    current_frequency = frequency;
  }
}

void PWM_on() {
  TCCR3A = (1 << COM3A0);
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
  set_PWM(0);
}

void PWM_off() {
  TCCR3A = 0x00;
  TCCR3B = 0x00;
}

void Tick(){
        switch(state){
                case Start:
                        state = Init;
                        break;

                case Init:
                        if((~PINA & 0x01) == 0x01){
                                state = Power;
                        }else{
                                state = Init;
                        }
                        break;

                case Power:
                        if((i >= 18)){ //wont get interrupted during music
                                state = Pow;
                        }else{
                                state = Power;
                        }
                        break;

                case Pow:
                        if((~PINA & 0x01) == 0x00){
                                state = Init;
                        }else{
                                state = Pow;
                        }
                        break;

                default:
                        state = Start;
                        break;
        }

      switch(state){
                case Start:
                        break;

                case Init:
                        break;

                case Power:
                        set_PWM(FRE[i]);
                        i = i + 1;
                        break;

                case Pow:
                        set_PWM(0);
                        i = 0;
                        break;

                default:
                        break;

        }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(35);

    TimerOn();

    PWM_on();
    while (1) {
        Tick();
        while(!TimerFlag) {};
        TimerFlag = 0;
    }
    return 0;
}
