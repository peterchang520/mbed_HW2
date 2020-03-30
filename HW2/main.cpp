#include "mbed.h"
#include "math.h"
Serial pc(USBTX, USBRX);
AnalogOut Aout(DAC0_OUT);
AnalogIn  Ain(A0);

int sample = 800;
int count_wave;
int fre;
int fre_2, fre_1, fre_0;
float t = 0;

float ADCdata[800];

//switch & LED
DigitalIn Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);

//7-seg
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);

char seg_display(int num, int last);

int main(){
    //smapling
    for(int i=0; i<sample; i++){
        ADCdata[i] = Ain;
        wait(1./sample);
    }

    //calculate frequency
    for(int i=0; i<sample; i++){
        if(i!=0){
            if(ADCdata[i]>=ADCdata[i-1] && ADCdata[i+1]<=ADCdata[i]){
                count_wave++;
            }
        }
    }
    fre = count_wave;
    fre_2 = fre/100;
    fre_1 = (fre - 100*fre_2)/10;
    fre_0 = fre - 100*fre_2 - 10*fre_1;

    //send to pc
    for(int i=0; i<sample; i++){
        pc.printf("%1.3f\r\n", ADCdata[i]);
        wait(0.01);
    }


    //display frequency at 7-seg & output sine wave
    while(1){
        //display fre
        for(int i=0; i<3; i++){
            if(Switch==0){  //push
                redLED = 0;  // 0 is light
                greenLED = 1;
                if(i==0)
                    {display = seg_display(fre_2, 0);}
                else if(i==1)
                    {display = seg_display(fre_1, 0);}
                else if(i==2)
                    {display = seg_display(fre_0, 1);}
                wait(1.0);
            }
            else if(Switch==1){
                i = 3;
                redLED = 1;
                greenLED = 0;
                display = 0;
            }
        
        //output sine wave
        Aout = 0.5*sin(2*3.14*fre*t)+0.5;
        t = t + 0.0001;
        wait(0.0001);
        }
    }

}

char seg_display(int num, int last){
    char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; //0-9(no dot)
    char table_dot[10] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF}; //0-9(dot)
    if(last==0){
        return table[num];
    }
    else if(last==1){
        return table_dot[num];
    }
}
