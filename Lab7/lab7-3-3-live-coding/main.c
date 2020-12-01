#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};

extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

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

int cnt=0;
int c=0;  // is counting ?? 0 is not

int display(int data){
	max7219_send(0x9,0xff);  // decode mode  // decode for code B on digit 0~7
	max7219_send(1,0);
	int up=1,down;
	while(data>0){
		down=data%10;
		data/=10;
		max7219_send(up,down);
		up++;
	}
	while(up<=8){
		if(up==1){
			max7219_send(up,0);
		}
		else{
			max7219_send(up,15);
		}

		up++;
	}
	return 0;
}

void Timer_init(){
	//TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	SET_REG(TIM2->CR1, TIM_CR1_DIR, 0x00);
	//SET_REG(TIM2->CR1, 0x8 , 0x8);
	TIM2->ARR=100;
	TIM2->PSC=499;
	TIM2->EGR = TIM_EGR_UG;
}

void Timer_start (){
	// TODO : start timer and show the time on the 7-SEG LED.
	TIM2->CR1 |= TIM_CR1_CEN;
}

void GPIO_init_AF(){
	SET_REG(GPIOA->AFR[0],GPIO_AFRL_AFSEL1,0x10)
}

void PWM_channel_init(int d){
	SET_REG(TIM2->CCMR1,TIM_CCMR1_OC2M, 0x7 << 12); // set PWM mode 2  channel 2
	TIM2->CCR2 = d;
	TIM2->CCER |= 1 << 4;  // Cpature/Compare 2 output enable
}

void SystemClock_Config () {
	SET_REG(RCC->CR, 0x00000100, 0x00000100 );  // enable HSI
	while((RCC->CR & RCC_CR_HSIRDY) == 0 );     // check HSI ready
	SET_REG(RCC->CFGR, 0x00000003, 0x1 )        // change to HSI
}

void SysTick_config () {
	// SysTick
	SysTick->LOAD=1999999;              // every 1 seconds
	//SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // external clock == > HSI/8  // enable
}

void SysTick_Handler () {
	if(cnt>0){
		cnt--;
	}
}


void init_GPIO () {
	// Keypad
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN, RCC_AHB2ENR_GPIOBEN);  // enable gpio b
	SET_REG(GPIOB->MODER, 0xff , 0x00);   // pb 0 1 2 3 intput
	SET_REG(GPIOB->MODER, 0xff00 , 0x5500);   // pb 4 5 6 7 ouptput
	SET_REG(GPIOB->PUPDR, 0xff , 0xaa);   // pb 0 1 2 3 pull down
	GPIOB->BSRR=(0xf<<4);
	// pa1  buzzer
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc , 0x8);   // pa 1 af mode
	// buttom
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN);  // enable gpio c
	SET_REG(GPIOC->MODER, 0x0c000000 , 0x0);    // pc13 input // led
	// 7-seg
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN);  // enable gpio c
	SET_REG(GPIOC->MODER, 0x3f , 0x15);       // pc 0 1 2 output
	SET_REG(GPIOC->OSPEEDR, 0x3f , 0x2a);
}

void EXTI_config () {
	SET_REG(RCC->APB2ENR,RCC_APB2ENR_SYSCFGEN,RCC_APB2ENR_SYSCFGEN)
	SET_REG(SYSCFG->EXTICR[0],0x00007777, 0x00001111);  // select pb0,pb1,pb2,pb3
	SET_REG(SYSCFG->EXTICR[3],0x00000070, 0x00000020);  // select p2 13
	SET_REG(EXTI->IMR1,0x200f, 0x200f);   // interrupt mask register (enable)
	SET_REG(EXTI->RTSR1,0xf, 0xf);   // Rising trigger  // keypad
	SET_REG(EXTI->FTSR1,0x2000, 0x2000);   // Falling trigger  // button
	//SET_REG(EXTI->PR1,0xf,0xf);
}

void NVIC_config () {
	//SET_REG(NVIC->ISER[0],0x3c0,0x3c0);  // enable  6 7 8 9   EXTI 0 ,1,2,3
	NVIC_EnableIRQ(EXTI0_IRQn);  // enable exti15-10
	NVIC_EnableIRQ(EXTI1_IRQn);  // enable exti15-10
	NVIC_EnableIRQ(EXTI2_IRQn);  // enable exti15-10
	NVIC_EnableIRQ(EXTI3_IRQn);  // enable exti15-10
	NVIC_EnableIRQ(EXTI15_10_IRQn);  // enable exti15-10
	//SET_REG(NVIC->ISER[1],0x010,0x010);  // enable  40      EXTI10_15
	//SET_REG(NVIC->ISPR[0],0x3c0,0x3c0);  // pending
	//SET_REG(NVIC->IABR[0],0x3c0,0x3c0);  // active
}

void EXTI0_IRQHandler () {
	if(cnt==0&&c==0){
		int i,val;
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
		c=1;
		//SET_REG(SysTick->CTRL, 0x00000007, 0x00000002 );  // disable systick
		SysTick->LOAD=1999999;
		SysTick->VAL=0;
		SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // enable systick
		GPIOB->BSRR=(0xf<<4);
	}
		EXTI->PR1 |= 0x0F;
		NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

void EXTI1_IRQHandler () {
	if(cnt==0&&c==0){
		int i,val;
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
		c=1;
		//SET_REG(SysTick->CTRL, 0x00000007, 0x00000002 );  // disable systick
		SysTick->LOAD=1999999;
		SysTick->VAL=0;
		SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // enable systick
		GPIOB->BSRR=(0xf<<4);
    }
		EXTI->PR1 |= 0x0F;
		NVIC_ClearPendingIRQ(EXTI1_IRQn);
}

void EXTI2_IRQHandler () {
	if(cnt==0&&c==0){
		int i,val;
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
		c=1;
		//SET_REG(SysTick->CTRL, 0x00000007, 0x00000002 );  // disable systick
		SysTick->LOAD=1999999;
		SysTick->VAL=0;
		SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // enable systick
		GPIOB->BSRR=(0xf<<4);
	}
		EXTI->PR1 |= 0x0F;
		NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void EXTI3_IRQHandler () {
	if (cnt==0&&c==0){
		int i,val;
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
		c=1;
		//SET_REG(SysTick->CTRL, 0x00000007, 0x00000002 );  // disable systick
		SysTick->LOAD=1999999;
		SysTick->VAL=0;
		SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // enable systick
		GPIOB->BSRR=(0xf<<4);
	}
	EXTI->PR1 |= 0x0F;
	NVIC_ClearPendingIRQ(EXTI3_IRQn);
}

void EXTI15_10_IRQHandler () {
	cnt=0;
	c=0;
	EXTI->PR1 |= 0x2000;
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}
/*
void button(){
	int x;
	x=GPIOC->IDR;
	x&=0x00002000;
	if(x==0){
		c=0;
	}
}
*/
int main () {
	init_GPIO();
	SystemClock_Config();
	SysTick_config();
	max7219_init();
	Timer_init();
	Timer_start();
	GPIO_init_AF();
	PWM_channel_init(0);
	NVIC_config();
	EXTI_config();
	int pre;
	pre=0;
	display(pre);
	while(1){
		if(cnt!=pre){
			display(cnt);
			pre=cnt;
		}
		if(cnt==0&&c==1){
			TIM2->CCR2 = 50;
		}
		else{
			TIM2->CCR2 = 0;
			if(c==0){
				SET_REG(SysTick->CTRL, 0x00000007, 0x00000000 );  // disable systick
			}
		}
	}
	return 0;
}
