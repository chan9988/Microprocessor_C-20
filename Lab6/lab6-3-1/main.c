#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};


void GPIO_init();
void Delay1sUnder4MHz();
void Set_HCLK( int freq);
void buttom();
void delay();
void delay1();

int freq[] = {1, 6, 10, 16, 40};
int now=0;

int main(){
	// Do initializations.
	GPIO_init();
	Set_HCLK(freq[now]);
	for (;;){
	// change LED state
		GPIOA->BSRR=(1<<5);
		Delay1sUnder4MHz();
		GPIOA->BRR=(1<<5);
		Delay1sUnder4MHz();
	// change HCLK if button pressed
	}
}

void GPIO_init(){
	// led
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc00 , 0x400);    // pa5 output // led
	// buttom
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN);  // enable gpio c
	SET_REG(GPIOC->MODER, 0x0c000000 , 0x0);    // pc13 input // led
	// clock
	SET_REG(RCC->PLLCFGR, 0x0700ffff, 0x01000831 )
	SET_REG(RCC->CR, 0x03000000, 0x01000000 )
	SET_REG(RCC->CFGR, 0x00000003, 0x3 )
}

void Set_HCLK( int f){
	// 1. change to the temporary clock source if needed
	SET_REG(RCC->CFGR, 0x00000003, 0x0 )    // change to msi
	// 2. set the target clock source
	SET_REG(RCC->CR, 0x03000000, 0x00000000 )  // disable pll
	while((RCC->CR & RCC_CR_PLLRDY)==1);
	if(f==1){
		SET_REG(RCC->PLLCFGR, 0x0700ffff, 0x07000831 )
		SET_REG(FLASH->ACR,FLASH_ACR_LATENCY,FLASH_ACR_LATENCY_0WS)
	}
	else if(f==6){
		SET_REG(RCC->PLLCFGR, 0x0700ffff, 0x01000921 )
	}
	else if(f==10){
		SET_REG(RCC->PLLCFGR, 0x0700ffff, 0x01000a11 )
	}
	else if(f==16){
		SET_REG(RCC->PLLCFGR, 0x0700ffff, 0x01000801 )
	}
	else if(f==40){
		SET_REG(RCC->PLLCFGR, 0x0700ffff, 0x01002811 )
		SET_REG(FLASH->ACR,FLASH_ACR_LATENCY,FLASH_ACR_LATENCY_1WS)
	}
	SET_REG(RCC->CR, 0x03000000, 0x01000000 )  // enable pll
	while((RCC->CR & RCC_CR_PLLRDY)==0);
	// 3. change to the target clock source
	SET_REG(RCC->CFGR, 0x00000003, 0x3 )   // change to pll
}

void Delay1sUnder4MHz(){
	int i,j;
	for(i=0;i<230;i++){
		for(j=0;j<230;j++){
			buttom();
		}
	}

}

void buttom(){
	int i;
	int ch=1;
	int x;
	for(i=0;i<1000;i++){
		x=GPIOC->IDR;
		x&=0x00002000;
		if(x!=0){
			ch=0;
			break;
		}
	}
	if(ch){
		int ch1=1;
		while(ch1){
			ch1=0;
			for(i=0;i<10000;i++){
				x=GPIOC->IDR;
				x&=0x00002000;
				if(x==0){
					ch1=1;
					break;
				}
			}
		}
		int co=0;
		if(now==0){
			now=1;
			Set_HCLK(freq[now]);
			while(co<10000){
				x=GPIOC->IDR;
				x&=0x00002000;
				if(x!=0){
					co++;
				}
				else{
					co=0;
				}
			}
		}
		else if(now==1){
			now=2;
			Set_HCLK(freq[now]);
			while(co<10000){
				x=GPIOC->IDR;
				x&=0x00002000;
				if(x!=0){
					co++;
				}
				else{
					co=0;
				}
			}
		}
		else if(now==2){
			now=3;
			Set_HCLK(freq[now]);
			while(co<10000){
				x=GPIOC->IDR;
				x&=0x00002000;
				if(x!=0){
					co++;
				}
				else{
					co=0;
				}
			}
		}
		else if(now==3){
			now=4;
			Set_HCLK(freq[now]);
			while(co<10000){
				x=GPIOC->IDR;
				x&=0x00002000;
				if(x!=0){
					co++;
				}
				else{
					co=0;
				}
			}
		}
		else if(now==4){
			now=0;
			Set_HCLK(freq[now]);
			while(co<10000){
				x=GPIOC->IDR;
				x&=0x00002000;
				if(x!=0){
					co++;
				}
				else{
					co=0;
				}
			}
		}

	}

}

void delay(){
	int i,j;
	for(i=0;i<300;i++){
		for(j=0;j<300;j++){

		}
	}

}

void delay1(){
	int i,j;
	for(i=0;i<700;i++){
		for(j=0;j<700;j++){

		}
	}

}
