#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};

void GPIO_init () {
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc00 , 0x400);    // pa5 output // led
}

void SystemClock_Config () {
	SET_REG(RCC->CR, 0x00000100, 0x00000100 );  // enable HSI
	while((RCC->CR & RCC_CR_HSIRDY) == 0 );     // check HSI ready
	SET_REG(RCC->CFGR, 0x00000003, 0x1 )        // change to HSI
}

void SysTick_config () {
	// SysTick
	SysTick->LOAD=5999999;
	SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // external clock == > HSI/8
}

void SysTick_Handler () {
	int x;
	x=(GPIOA->ODR&(1<<5));
	if(x>0){
		GPIOA->BRR=(1<<5);
	}
	else{
		GPIOA->BSRR=(1<<5);
	}
}

int main (){
	GPIO_init();
	SystemClock_Config();
	SysTick_config();
	while (1);
	GPIOA->BSRR=(1<<5);
	return 0;
}
