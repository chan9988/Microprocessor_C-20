#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};

#define Y0 0x1     // pb0
#define Y1 0x2     // pb1
#define Y2 0x4     // pb2
#define Y3 0x8     // pb3
#define X0 0x10    // pb4
#define X1 0x20    // pb5
#define X2 0x40    // pb6
#define X3 0x80    // pb7

unsigned int x_pin[4] = {X0, X1, X2, X3};
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3};

void delay(){
	int i,j;
	for(i=0;i<400;i++){
		for(j=0;j<400;j++){

		}
	}
}

void init_GPIO () {
	// LED
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc00 , 0x400);    // pa5 output // led
	GPIOA->BSRR=(1<<5);
	// Keypad
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN, RCC_AHB2ENR_GPIOBEN);  // enable gpio b
	SET_REG(GPIOB->MODER, 0xff , 0x00);   // pb 0 1 2 3 intput
	SET_REG(GPIOB->MODER, 0xff00 , 0x5500);   // pb 4 5 6 7 ouptput
	SET_REG(GPIOB->PUPDR, 0xff , 0xaa);   // pb 0 1 2 3 pull down
	GPIOB->BSRR=(0xf<<4);
}

void EXTI_config () {
	SET_REG(RCC->APB2ENR,RCC_APB2ENR_SYSCFGEN,RCC_APB2ENR_SYSCFGEN)
	SET_REG(SYSCFG->EXTICR[0],0x00007777, 0x00001111);  // select pb0,pb1,pb2,pb3
	SET_REG(EXTI->IMR1,0xf, 0xf);   // interrupt mask register (enable)
	SET_REG(EXTI->RTSR1,0xf, 0xf);   // Rising trigger
	//SET_REG(EXTI->PR1,0xf,0xf);
}

void NVIC_config () {
	SET_REG(NVIC->ISER[0],0x3c0,0x3c0);  // enable
	//SET_REG(NVIC->ISPR[0],0x3c0,0x3c0);  // pending
	//SET_REG(NVIC->IABR[0],0x3c0,0x3c0);  // active
}

void EXTI0_IRQHandler () {
	int i,val,cnt=0;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf0 , x_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf);
		if(val==y_pin[0]) {
			if(i==0){
				cnt=1;
			}
			else if(i==1){
				cnt=2;
			}
			else if(i==2){
				cnt=3;
			}
			else if(i==3){
				cnt=10;
			}
		}
	}
	while(cnt--){
		GPIOA->BRR=(1<<5);
		delay();
		GPIOA->BSRR=(1<<5);
		delay();
	}
	GPIOB->BSRR=(0xf<<4);
	EXTI->PR1 |= 0x0F;
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

void EXTI1_IRQHandler () {
	int i,val,cnt=0;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf0 , x_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf);
		if(val==y_pin[1]) {
			if(i==0){
				cnt=4;
			}
			else if(i==1){
				cnt=5;
			}
			else if(i==2){
				cnt=6;
			}
			else if(i==3){
				cnt=11;
			}
		}
	}
	while(cnt--){
		GPIOA->BRR=(1<<5);
		delay();
		GPIOA->BSRR=(1<<5);
		delay();
	}
	GPIOB->BSRR=(0xf<<4);
	EXTI->PR1 |= 0x0F;
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
}

void EXTI2_IRQHandler () {
	int i,val,cnt=0;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf0 , x_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf);
		if(val==y_pin[2]) {
			if(i==0){
				cnt=7;
			}
			else if(i==1){
				cnt=8;
			}
			else if(i==2){
				cnt=9;
			}
			else if(i==3){
				cnt=12;
			}
		}
	}
	while(cnt--){
		GPIOA->BRR=(1<<5);
		delay();
		GPIOA->BSRR=(1<<5);
		delay();
	}
	GPIOB->BSRR=(0xf<<4);
	EXTI->PR1 |= 0x0F;
	NVIC_ClearPendingIRQ(EXTI2_IRQn);

}

void EXTI3_IRQHandler () {
	int i,val,cnt=0;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf0 , x_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf);
		if(val==y_pin[3]) {
			if(i==0){
				cnt=15;
			}
			else if(i==1){
				cnt=0;
			}
			else if(i==2){
				cnt=14;
			}
			else if(i==3){
				cnt=13;
			}
		}
	}
	while(cnt--){
		GPIOA->BRR=(1<<5);
		delay();
		GPIOA->BSRR=(1<<5);
		delay();
	}
	GPIOB->BSRR=(0xf<<4);
	EXTI->PR1 |= 0x0F;
	NVIC_ClearPendingIRQ(EXTI3_IRQn);

}

int main () {
	init_GPIO();
	NVIC_config();
	EXTI_config();
	while(1);
	return 0;
}
