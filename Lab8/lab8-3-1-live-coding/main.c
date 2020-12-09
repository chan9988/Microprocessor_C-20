#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};

char *s="Hello World!";


int UART_Transmit(uint8_t *arr, uint32_t size) {
	int cnt=0;
	for(int i=0;i<size;i++){
		while(!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR = s[i];
	}
	return cnt;
}

void init_GPIO(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN;
	SET_REG(GPIOA->MODER,0x3c0000,0x280000);   // pa9,pa10 ==> af mode
	SET_REG(GPIOA->AFR[1],0xff0,0x770);       // af7

	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN);  // enable gpio c
	SET_REG(GPIOC->MODER, 0x0c000000 , 0x0);    // pc13 input

}

void HCLK(){
	SET_REG(RCC->CFGR,0xf0,0x80);  // ADC_PRESC  msi/2=2M (HCLK)
}

void init_UART(){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = 2000000/115600 ;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_UE;
}

int button(){
	int x;
	int cnt=8000;
	while(cnt--){
		x=GPIOC->IDR;
		x&=0x00002000;
		if(x!=0){
			return 0;
		}
	}
	UART_Transmit(s,12);
	cnt=10000;
	while(cnt){
		x=GPIOC->IDR;
		x&=0x00002000;
		if(x!=0) cnt--;
		else cnt=10000;
	}
	return 0;
}

int main(void) {
	init_GPIO();
	HCLK();
	init_UART();
	while(!button());
	return 0;
}
