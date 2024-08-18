#include "stdint.h"

// Ports
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTC_DATA_R       (*((volatile unsigned long *)0x400063FC))
#define GPIO_PORTC_DIR_R        (*((volatile unsigned long *)0x40006400))
#define GPIO_PORTC_AFSEL_R      (*((volatile unsigned long *)0x40006420))
#define GPIO_PORTC_DEN_R        (*((volatile unsigned long *)0x4000651C))
#define GPIO_PORTC_AMSEL_R      (*((volatile unsigned long *)0x40006528))
#define GPIO_PORTC_PCTL_R       (*((volatile unsigned long *)0x4000652C))
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))

#define SYSTICK_STCTRL					(*((volatile unsigned long *)0xE000E010))
#define SYSTICK_STRELOAD				(*((volatile unsigned long *)0xE000E014))			
#define SYSTICK_STCURRENT				(*((volatile unsigned long *)0xE000E018))

//Pin usage masks
#define PORTA_USING 0x1C //PA2-4 output
#define PORTC_USING 0xF0 //PC4-7 input
#define PORTE_USING 0x3E //PE1-5 output

const uint8_t fbus = 16; //MHz

// Function prototypes
void Systick_Delay_1sec(void);
void Systick_Delay(uint16_t s);
void Systick_Init(void);
void PortA_Init(void);
void PortC_Init(void);
void PortE_Init(void);
void PortA_Output(uint8_t out);
void PortE_Output(uint8_t out);

typedef struct state_struct {
	uint16_t delay;
	uint8_t outA;
	uint8_t outE;
	const struct state_struct* next[16];
} state_t;

const state_t s[7] = {
	{7,0x10,0x02,{&s[0],&s[5],&s[1],&s[1],&s[1],&s[5],&s[1],&s[1],&s[0],&s[5],&s[1],&s[1],&s[1],&s[5],&s[1],&s[1]}},
	{2,0x10,0x04,{&s[2],&s[2],&s[4],&s[4],&s[2],&s[2],&s[4],&s[4],&s[2],&s[2],&s[4],&s[4],&s[2],&s[2],&s[2],&s[4]}},
	{7,0x04,0x08,{&s[3],&s[3],&s[3],&s[3],&s[2],&s[3],&s[3],&s[3],&s[3],&s[3],&s[3],&s[3],&s[3],&s[3],&s[3],&s[3]}},
	{2,0x08,0x08,{&s[0],&s[5],&s[4],&s[4],&s[0],&s[5],&s[4],&s[4],&s[0],&s[5],&s[4],&s[0],&s[0],&s[5],&s[4],&s[5]}},
	{4,0x10,0x38,{&s[0],&s[0],&s[0],&s[0],&s[2],&s[2],&s[2],&s[2],&s[0],&s[0],&s[0],&s[0],&s[0],&s[2],&s[0],&s[2]}},
	{4,0x10,0x12,{&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6],&s[6]}},
	{3,0x10,0x02,{&s[0],&s[1],&s[1],&s[1],&s[1],&s[1],&s[1],&s[1],&s[0],&s[0],&s[1],&s[1],&s[1],&s[1],&s[1],&s[1]}}
};

#define WarnerGo      s[0]
#define WarnerWait    s[1]
#define FirstGo       s[2]
#define FirstWait     s[3]
#define WarnerWalk    s[4]
#define FirstWalk     s[5]
#define WarnerGoShort s[6]

int main(void){ 
	state_t currentState;
	uint8_t condition;
	
	Systick_Init();
	PortA_Init();
	PortC_Init();
	PortE_Init();
	
	currentState = WarnerGo;
	
	while(1){
		PortA_Output(currentState.outA);
		PortE_Output(currentState.outE);
		Systick_Delay(currentState.delay);
		condition = ((GPIO_PORTC_DATA_R & 0xF0) >> 4);
		currentState = *(currentState.next[condition]);
  }
}

void PortA_Init(void){ 
    int delay;
    SYSCTL_RCGC2_R |= 0x1;
    delay = 0;
    GPIO_PORTA_DIR_R |= PORTA_USING;
    GPIO_PORTA_AFSEL_R &= ~PORTA_USING;
    GPIO_PORTA_AMSEL_R &= ~PORTA_USING;
    GPIO_PORTA_DEN_R |= PORTA_USING;
}

void PortC_Init(void){ 
    int delay;
    SYSCTL_RCGC2_R |= 0x4;
    delay = 0;
    GPIO_PORTC_DIR_R &= ~PORTC_USING;
    GPIO_PORTC_AFSEL_R &= ~PORTC_USING;
    GPIO_PORTC_AMSEL_R &= ~PORTC_USING;
    GPIO_PORTC_DEN_R |= PORTC_USING;
}

void PortE_Init(void){ 
    int delay;
    SYSCTL_RCGC2_R |= PORTE_USING;
    delay = 0;
    GPIO_PORTE_DIR_R |= PORTE_USING;
    GPIO_PORTE_AFSEL_R &= ~PORTE_USING;
    GPIO_PORTE_AMSEL_R &= ~PORTE_USING;
    GPIO_PORTE_DEN_R |= PORTE_USING;
}

void Systick_Init(void) {
		SYSTICK_STCTRL = 0x00;
		SYSTICK_STRELOAD = 0xFFFFFF;
		SYSTICK_STCURRENT = 0;
		SYSTICK_STCTRL = 0x05;
}

void Systick_Delay(uint16_t s) {
	int i = 0;
	for (i = 0; i < s; i++)
		Systick_Delay_1sec();
}

void Systick_Delay_1sec(void) {
		uint32_t microseconds = 1000000;
		SYSTICK_STRELOAD = microseconds*fbus;
		SYSTICK_STCURRENT = 0;
		while (!(SYSTICK_STCTRL & 0x00010000));
}

void PortA_Output(uint8_t out) {
	GPIO_PORTA_DATA_R |= out;
	GPIO_PORTA_DATA_R &= (out | ~PORTA_USING);
}
	
void PortE_Output(uint8_t out) {
	GPIO_PORTE_DATA_R |= out;
	GPIO_PORTE_DATA_R &= (out | ~PORTE_USING);
}
