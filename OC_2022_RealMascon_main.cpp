#include "mbed.h"
#include <math.h>

#define MAX_SPEED 110
#define MAX_ACCELERATION 4.5
#define MAX_DECELERATION 10

void InitStatus(void);

void ReadBrake(void);
void ReadPower(void);
void CheckBrake(void);
void CheckPower(void);
void CalcBrake(void);
void CalcPower(void);
void CalcSpeed(void);
void MoveTrain(void);

void PrintDisplay(void);
void RefleshDisplay(void);


DigitalIn Power[3] = {PB_15, PB_1, PB_2};
DigitalIn Brake[4] = {PB_5, PB_4, PB_10, PA_8};
DigitalOut Direction(PA_14);
PwmOut PWM_speed(PA_15);
Serial PC(USBTX, USBRX);

int Power_oct = 0;
int Brake_hex = 0;
int Power_notch = 0;
int Brake_notch = 10;
double RealSpeed = 0.0;
double ScaleSpeed = 0.0;
double Acceleration = 0.0;
double Deceleration = 0.0;
double LimitSpeed = 0.0;

int main()
{
    InitStatus();
    while(1){
        RefleshDisplay();
        ReadBrake();
        ReadPower();
        CheckBrake();
        PrintDisplay();
        CalcSpeed();
        MoveTrain();
        wait_ms(100);
    }
}


void InitStatus(void){
    PC.printf("System Start\n\r");
    Direction = 0;
    RealSpeed = 0.0;
    ScaleSpeed = 0.0;
    PC.printf("Press any key to continue...\n\r");
    PC.getc();
    PC.printf("\e[0J");
}

void ReadBrake(void){
    Brake_hex = 0;
    for(int i = 0; i < 4; i++){
        Brake_hex += Brake[i] * (int)pow((double)2, (double)i);
    }
}

void ReadPower(void){
    Power_oct = 0;
    for(int i = 0; i < 3; i++){
        Power_oct += Power[i] * (int)pow((double)2, (double)i);
    }
}

void CheckBrake(void){
    Power_notch = 0;
    switch(Brake_hex){
        case 0x2: Brake_notch = 0; CheckPower(); break;
        case 0x8: Brake_notch = 1; break;
        case 0xA: Brake_notch = 2; break;
        case 0x1: Brake_notch = 3; break;
        case 0x3: Brake_notch = 4; break;
        case 0x9: Brake_notch = 5; break;
        case 0xB: Brake_notch = 6; break;
        case 0x4: Brake_notch = 7; break;
        case 0x6: Brake_notch = 8; break;
        case 0xF: Brake_notch = 10; break;
        default: Brake_notch = 9;
    }
}

void CheckPower(void){
    switch(Power_oct){
        case 04: Power_notch = 1; break;
        case 05: Power_notch = 2; break;
        case 02: Power_notch = 3; break;
        case 03: Power_notch = 4; break;
        case 06: Power_notch = 5; break;
        default: Power_notch = 0;
    }
}

void CalcBrake(void){
    Acceleration = 0;
    switch(Brake_notch){
        case 0: Deceleration = 0; CalcPower(); break;
        case 1: Deceleration = MAX_DECELERATION / 8.0; break; 
        case 2: Deceleration = MAX_DECELERATION / 5.0; break;
        case 3: Deceleration = MAX_DECELERATION / 3.0; break;
        case 4: Deceleration = MAX_DECELERATION / 2.0; break;
        case 5: Deceleration = MAX_DECELERATION / 1.8; break; 
        case 6: Deceleration = MAX_DECELERATION / 1.5; break;
        case 7: Deceleration = MAX_DECELERATION / 1.2; break;
        case 8: Deceleration = MAX_DECELERATION / 1.1; break;
        case 9: Deceleration = MAX_DECELERATION; break;
        default: ScaleSpeed = 0;
    }
}

void CalcPower(void){
    switch(Power_notch){
        case 1: Acceleration = MAX_ACCELERATION / 5.0; LimitSpeed = MAX_SPEED / 4.0; break; 
        case 2: Acceleration = MAX_ACCELERATION / 3.0; LimitSpeed = MAX_SPEED / 2.0; break;
        case 3: Acceleration = MAX_ACCELERATION / 2.0; LimitSpeed = MAX_SPEED / 1.5; break;
        case 4: Acceleration = MAX_ACCELERATION / 1.3; LimitSpeed = MAX_SPEED; break;
        case 5: Acceleration = MAX_ACCELERATION; LimitSpeed = MAX_SPEED; break;
        default: Acceleration = 0; Deceleration = MAX_DECELERATION / 20.0;
    }
    if(ScaleSpeed >= LimitSpeed)
        Acceleration = 0;
}

void CalcSpeed(void){
    CalcBrake();
    ScaleSpeed = ScaleSpeed + Acceleration/3 - Deceleration/3;
    if(ScaleSpeed <= 0)
        ScaleSpeed = 0;
    RealSpeed = ScaleSpeed / MAX_SPEED;
}

void MoveTrain(void){
    Direction = 0;
    PWM_speed = RealSpeed;
}



void PrintDisplay(void){
    PC.printf("\n\r\n\r");
    PC.printf("ScaleSpeed:    \e[1m%d\e[m km/h\n\r",(int)ScaleSpeed);
    PC.printf("RealSpeed:     \e[1m%lf\e[m percent\n\r",RealSpeed * 100);
    PC.printf("\tNotch: %dstep  Brake: %dstep",Power_notch,Brake_notch);
}

void RefleshDisplay(void){
    PC.printf("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
}