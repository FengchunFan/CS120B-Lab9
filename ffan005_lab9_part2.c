/*      Author: ffan005
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo: 
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start, Init, Power, Pow, Increment, Incre, Decrement, Decre} state;
unsigned char power = 0x00; //power starts with off

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

double FRE[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char i = 0x00;

void Tick(){
        switch(state){
                case Start:
                        state = Init;
                        break;

                case Init:
                        if((~PINA & 0x07) == 0x01) {
                        state = Power;
                        } else if ((~PINA & 0x07) == 0x02) {
                        state = Increment;
                        } else if ((~PINA & 0x07) == 0x04) {
                        state = Decrement;
                        } else {
                        state = Init;
                        }
                        break;

                case Power:
                        state = Pow;
                        break;

                case Pow:
                        if ((~PINA & 0x07) == 0x00) {
                        state = Init;
                        }else{
                        state = Pow;
                        }
                        break;

                case Increment:
                        state = Incre;
                        break;

                case Incre:
                        if ((~PINA & 0x07) == 0x00) {
                        state = Init;
                        }else{
                        state = Incre;
                        }
                        break;

                case Decrement:
                        state = Decre;
                        break;

                case Decre:
                        if ((~PINA & 0x07) == 0x00) {
                        state = Init;
                        }else{
                        state = Decre;
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
                        if(power == 0x00){
                                power = 0x01;
                                set_PWM(FRE[i]); //messed up thought it was reset
                        }else if (power == 0x01) { 
                                power = 0x00;
                                set_PWM(0);
                        }
                        break;

                case Pow:
                        break;

                case Increment:
                        if(i < 0x07){
                        i = i + 1;
                        }
                        if(power == 0x01){
                        set_PWM(FRE[i]);
                        }
                        break;

                case Incre:
                        break;

                case Decrement:
                        if(i > 0x00){
                        i = i - 1;
                        }
                        if(power == 0x01){
                        set_PWM(FRE[i]);
                        }
                        break;

                case Decre:
                        break;

                default:
                        break;
        }

}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    PWM_on();

    while (1) {
        Tick();
    }
    return 0;
}
