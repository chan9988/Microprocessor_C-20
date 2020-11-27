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

//extern void GPIO_init();
int scan=0;

void keypad_init(){
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN, RCC_AHB2ENR_GPIOBEN);  // enable gpio b
	SET_REG(GPIOA->MODER, 0xc , 0x8);   // pa 1 af mode
	SET_REG(GPIOB->MODER, 0xff , 0x55);   // pb 0 1 2 3 output
	SET_REG(GPIOB->MODER, 0xff00 , 0x0000);   // pb 4 5 6 7 input
	SET_REG(GPIOB->PUPDR, 0xff00 , 0xaa00);   // pb 4 5 6 7 pull down
}

int keypad_scan(){
	int val;
	int i,j;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf , y_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf0);
		for(j=0;j<4;j++){
			if(val==x_pin[j]){
				if(scan==0){
					scan=3000;
					return i*4+j;
				}
			}
		}
	}
	if(scan>0){
		scan--;
	}

	return 16;
}


void Timer_init(){
//TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	SET_REG(TIM2->CR1, TIM_CR1_DIR, 0x00);
	//SET_REG(TIM2->CR1, 0x8 , 0x8);
	TIM2->ARR=100;
	TIM2->PSC=119;
	TIM2->EGR = TIM_EGR_UG;
}

void Timer_start (int d){
// TODO : start timer and show the time on the 7-SEG LED.
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->CCR2 = d;
}

void GPIO_init_AF(){
	SET_REG(GPIOA->AFR[0],GPIO_AFRL_AFSEL1,0x10)
}

void PWM_channel_init(int d){
	SET_REG(TIM2->CCMR1,TIM_CCMR1_OC2M, 0x7 << 12); // set PWM mode 2  channel 2
	TIM2->CCR2 = d;
	TIM2->CCER |= 1 << 4;  // Cpature/Compare 2 output enable
}


int main (){
	keypad_init();
	GPIO_init_AF();
	Timer_init();
	int f=10;
	Timer_start(f);
	PWM_channel_init(f);
	int b;
	while (1){
		// TODO : Polling the timer count and do lab requirements
		//GPIOA->BRR=(1<<1);
		b=keypad_scan();
		if(b==14&&f<90){
			f+=5;
			TIM2->CCR2 = f;
		}
		else if(b==12&&f>10){
			f-=5;
			TIM2->CCR2 = f;
		}
	}
	return 0;
}

