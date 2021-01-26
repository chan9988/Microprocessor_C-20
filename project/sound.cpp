#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};

void Sound_init(){
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN, RCC_AHB2ENR_GPIOBEN);  // enable gpio b
	SET_REG(GPIOA->MODER, 0xc , 0x8);   // pa 1 af mode
	SET_REG(GPIOB->MODER, 0xc00 , 0x800);   // pb 5 af mode
	SET_REG(GPIOB->MODER, 0xc000 , 0x8000);   // pb 7 af mode

}


void Timer_init(){
//TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	SET_REG(TIM5->CR1, TIM_CR1_DIR, 0x00);
	SET_REG(TIM3->CR1, TIM_CR1_DIR, 0x00);
	SET_REG(TIM4->CR1, TIM_CR1_DIR, 0x00);
	//SET_REG(TIM2->CR1, 0x8 , 0x8);
	TIM5->ARR=10;
	TIM5->PSC=119;
	TIM5->EGR = TIM_EGR_UG;
	TIM3->ARR=10;
	TIM3->PSC=2427;
	TIM3->EGR = TIM_EGR_UG;
	TIM4->ARR=10;
	TIM4->PSC=3058;
	TIM4->EGR = TIM_EGR_UG;
}


void Timer_start (int d){
// TODO : start timer and show the time on the 7-SEG LED.
	TIM5->CR1 |= TIM_CR1_CEN;
	TIM5->CCR2 = d;
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM3->CCR2 = d;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM4->CCR2 = d;
}

void GPIO_init_AF(){
	SET_REG(GPIOA->AFR[0],GPIO_AFRL_AFSEL1,0x20);
	SET_REG(GPIOB->AFR[0],GPIO_AFRL_AFSEL5,0x200000);
	SET_REG(GPIOB->AFR[0],GPIO_AFRL_AFSEL7,0x20000000);
}

void PWM_channel_init(int d){
	SET_REG(TIM5->CCMR1,TIM_CCMR1_OC2M, 0x7 << 12); // set PWM mode 2  channel 2
	//SET_REG(TIM3->CCMR2,TIM_CCMR2_OC3M, 0x7 << 4); // set PWM mode 2  channel 3
	SET_REG(TIM3->CCMR1,TIM_CCMR1_OC2M, 0x7 << 12);
	SET_REG(TIM4->CCMR1,TIM_CCMR1_OC2M, 0x7 << 12); // set PWM mode 2  channel 2
	TIM5->CCR2 = d;
	TIM5->CCER |= 1 << 4;  // Cpature/Compare 2 output enable
	TIM3->CCR2 = d;
	TIM3->CCER |= 1 << 4;  // Cpature/Compare 2 output enable
	TIM4->CCR2 = d;
	TIM4->CCER |= 1 << 4;  // Cpature/Compare 2 output enable
}

void GPIO_init_sound () {
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc00 , 0x400);    // pa5 output // led
}

void SysTick_config_sound () {
	// SysTick
	SysTick->LOAD=62499;
	SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // external clock  // enable
}

void Sound(){
	Sound_init();
	GPIO_init_AF();
	Timer_init();
	Timer_start(0);
	PWM_channel_init(0);
	GPIO_init_sound();
	SysTick_config_sound();

}

void change_s(int d){
	if(d==0){
		TIM5->CCR2 = 0;
	}
	else{
		TIM5->CCR2 = 5;
	}
	switch(d){
	case 1: TIM5->PSC=1529; break;
	case 2: TIM5->PSC=1362; break;
	case 3: TIM5->PSC=1213; break;
	case 4: TIM5->PSC=1145; break;
	case 5: TIM5->PSC=1020; break;
	case 6: TIM5->PSC=909; break;
	case 7: TIM5->PSC=809; break;
	case 11: TIM5->PSC=764; break;
	case 12: TIM5->PSC=681; break;
	case 13: TIM5->PSC=606; break;
	case 14: TIM5->PSC=572; break;
	case 15: TIM5->PSC=510; break;
	case 16: TIM5->PSC=454; break;
	case 17: TIM5->PSC=405; break;
	case 21: TIM5->PSC=382; break;
	case 22: TIM5->PSC=340; break;
	case 23: TIM5->PSC=303; break;
	case 24: TIM5->PSC=286; break;
	case 25: TIM5->PSC=255; break;
	case 26: TIM5->PSC=227; break;
	case 27: TIM5->PSC=202; break;
	case 31: TIM5->PSC=191; break;
	case 111: TIM5->PSC=3058; break;
	case 112: TIM5->PSC=2724; break;
	case 113: TIM5->PSC=2427; break;
	case 114: TIM5->PSC=2290; break;
	case 115: TIM5->PSC=2040; break;
	case 116: TIM5->PSC=1818; break;
	case 117: TIM5->PSC=1619; break;
	case 121: TIM5->PSC=6116; break;
	case 122: TIM5->PSC=5448; break;
	case 123: TIM5->PSC=4854; break;
	case 124: TIM5->PSC=4582; break;
	case 125: TIM5->PSC=4081; break;
	case 126: TIM5->PSC=3636; break;
	case 127: TIM5->PSC=3239; break;
	}
}

void change_a(int d){
	if(d==0){
		TIM3->CCR2 = 0;
	}
	else{
		TIM3->CCR2 = 5;
	}
	switch(d){
	case 1: TIM3->PSC=1529; break;
	case 2: TIM3->PSC=1362; break;
	case 3: TIM3->PSC=1213; break;
	case 4: TIM3->PSC=1145; break;
	case 5: TIM3->PSC=1020; break;
	case 6: TIM3->PSC=909; break;
	case 7: TIM3->PSC=809; break;
	case 11: TIM3->PSC=764; break;
	case 12: TIM3->PSC=681; break;
	case 13: TIM3->PSC=606; break;
	case 14: TIM3->PSC=572; break;
	case 15: TIM3->PSC=510; break;
	case 16: TIM3->PSC=454; break;
	case 17: TIM3->PSC=405; break;
	case 21: TIM3->PSC=382; break;
	case 22: TIM3->PSC=340; break;
	case 23: TIM3->PSC=303; break;
	case 24: TIM3->PSC=286; break;
	case 25: TIM3->PSC=255; break;
	case 26: TIM3->PSC=227; break;
	case 27: TIM3->PSC=202; break;
	case 31: TIM3->PSC=191; break;
	case 111: TIM3->PSC=3058; break;
	case 112: TIM3->PSC=2724; break;
	case 113: TIM3->PSC=2427; break;
	case 114: TIM3->PSC=2290; break;
	case 115: TIM3->PSC=2040; break;
	case 116: TIM3->PSC=1818; break;
	case 117: TIM3->PSC=1619; break;
	case 121: TIM3->PSC=6116; break;
	case 122: TIM3->PSC=5448; break;
	case 123: TIM3->PSC=4854; break;
	case 124: TIM3->PSC=4582; break;
	case 125: TIM3->PSC=4081; break;
	case 126: TIM3->PSC=3636; break;
	case 127: TIM3->PSC=3239; break;
	}
}

void change_b(int d){
	if(d==0){
		TIM4->CCR2 = 0;
	}
	else{
		TIM4->CCR2 = 5;
	}
	switch(d){
	case 1: TIM4->PSC=1529; break;
	case 2: TIM4->PSC=1362; break;
	case 3: TIM4->PSC=1213; break;
	case 4: TIM4->PSC=1145; break;
	case 5: TIM4->PSC=1020; break;
	case 6: TIM4->PSC=909; break;
	case 7: TIM4->PSC=809; break;
	case 11: TIM4->PSC=764; break;
	case 12: TIM4->PSC=681; break;
	case 13: TIM4->PSC=606; break;
	case 14: TIM4->PSC=572; break;
	case 15: TIM4->PSC=510; break;
	case 16: TIM4->PSC=454; break;
	case 17: TIM4->PSC=405; break;
	case 21: TIM4->PSC=382; break;
	case 22: TIM4->PSC=340; break;
	case 23: TIM4->PSC=303; break;
	case 24: TIM4->PSC=286; break;
	case 25: TIM4->PSC=255; break;
	case 26: TIM4->PSC=227; break;
	case 27: TIM4->PSC=202; break;
	case 31: TIM4->PSC=191; break;
	case 111: TIM4->PSC=3058; break;
	case 112: TIM4->PSC=2724; break;
	case 113: TIM4->PSC=2427; break;
	case 114: TIM4->PSC=2290; break;
	case 115: TIM4->PSC=2040; break;
	case 116: TIM4->PSC=1818; break;
	case 117: TIM4->PSC=1619; break;
	case 121: TIM4->PSC=6116; break;
	case 122: TIM4->PSC=5448; break;
	case 123: TIM4->PSC=4854; break;
	case 124: TIM4->PSC=4582; break;
	case 125: TIM4->PSC=4081; break;
	case 126: TIM4->PSC=3636; break;
	case 127: TIM4->PSC=3239; break;
	}
}


int x=0;
int y=20;
int z=40;

int up[]={3125,2915,2720,2538,2368,2209,2061,1923,1794,1674,1562,1457,1360,1269,1184,1104,1030,961,897,837,781,
		728,680,634,592,552,515,481,448,418,390,364,340,317,296,
		276,257,240,224,209,195,182,170,158,148,138,128,120,112,
		104,97,91,85,79,74,69,64,60,56,52};

int c=0,c1=0;

void SysTick_Handler () {
	if(c==0){
		TIM3->CCR2 = 5;
		TIM4->CCR2 = 5;
		TIM5->CCR2 = 5;
		if(x>=0&&x<60) TIM5->PSC=up[x];
		if(y>=0&&y<60) TIM3->PSC=up[y];
		if(z>=0&&z<60) TIM4->PSC=up[z];
		x++;
		y++;
		z++;
		if(x>60) x=0;
		if(y>60) y=0;
		if(z>60) z=0;
	}
	else if(c==2){
		TIM3->CCR2 = 5;
		TIM4->CCR2 = 5;
		TIM5->CCR2 = 5;
		if(x>0) TIM5->PSC=up[x];
		if(y>0) TIM3->PSC=up[y];
		if(z>0) TIM4->PSC=up[z];

	}
	else{
		TIM3->CCR2 = 0;
		TIM4->CCR2 = 0;
		TIM5->CCR2 = 0;
	}
	c++;
	if(c>6){
		c=0;
	}
}


