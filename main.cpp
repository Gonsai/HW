#include "mbed.h"
#include "uLCD_4DGL.h"
using namespace std::chrono;
// main() runs in its own thread in the OS

Thread sample,gen,t;

EventQueue q;

InterruptIn Start(D9);
InterruptIn Stop(D8);

AnalogIn Ain(A0);
AnalogOut Aout(PA_4);

uLCD_4DGL uLCD(D1,D0,D2);

bool Genwave;
int j = 0;
float ADCdata[1024];

void wave() {
    static int i = 0;
    while (true) {
        if (Genwave) {
            if (i >= 0 && i < 10) {
                Aout = i * 0.1;
                i++;
            }
            else if (i >= 10 && i <=20) { 
                Aout = 1;
                i++;
            }
            else if (i >20 && i <= 100){
                Aout = 1 - (i - 20) * 0.0125;
                i++;
            }
            if (i > 100) i = 0;
        }
        ThisThread::sleep_for(1ms);
    }
}

void ADC() {
    while (true) {
        if (Genwave) { 
            if (j < 1024) {
                ADCdata[j] = Ain;
                j++;
            }
        }
        ThisThread::sleep_for(1ms);
    }
}

void data_out() {
    for (int k = 0; k < j; k++) {
        printf("%f\r\n", ADCdata[k]);
    }
    j = 0;
}

void uLCD_start() {
    uLCD.cls();
    uLCD.printf("Start bottom\ndetected\nWaveform\ngenerating...");
}

void uLCD_stop() {
    uLCD.cls();
    uLCD.printf("Stop bottom \ndetected\nData\ntransfering...");
}

void start()
{
    Genwave = true;
    q.call_in(500ms, uLCD_start);
}

void stop()
{
    Genwave = false;
    q.call_in(500ms, uLCD_stop);
    q.call_in(1s, data_out);
}

int main()
{
    Start.rise(&start);
    Stop.rise(&stop);
    t.start(callback(&q, &EventQueue::dispatch_forever));
    sample.start(ADC);
    gen.start(wave);
}