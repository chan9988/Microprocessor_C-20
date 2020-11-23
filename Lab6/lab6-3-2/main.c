#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};
#define TIME_SEC 12.70

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

int display(int data){
	int f1,f2;
	f1=data%10;
	data/=10;
	max7219_send(1,f1);
	f2=data%10;
	data/=10;
	max7219_send(2,f2);
	max7219_send(9,0xfb);
	if(data%10==0){
		max7219_send(3,0xfe);
	}
	else if(data%10==1){
		max7219_send(3,0xb0);
	}
	else if(data%10==2){
		max7219_send(3,0xed);
	}
	else if(data%10==3){
		max7219_send(3,0xf9);
	}
	else if(data%10==4){
		max7219_send(3,0xb3);
	}
	else if(data%10==5){
		max7219_send(3,0xdb);
	}
	else if(data%10==6){
		max7219_send(3,0xdf);
	}
	else if(data%10==7){
		max7219_send(3,0xf0);
	}
	else if(data%10==8){
		max7219_send(3,0xff);
	}
	else if(data%10==9){
		max7219_send(3,0xfb);
	}
	data/=10;
	int up=4,down;
	while(data>0){
		down=(int)data%10;
		data/=10;
		max7219_send(up,down);
		up++;
	}
	while(up<=8){
		max7219_send(up,15);
		up++;
	}

	return 0;
}

void Timer_init (int target){
	// TODO : Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	SET_REG(TIM2->CR1, TIM_CR1_DIR, 0x00);
	SET_REG(TIM2->CR1, 0x8 , 0x8);
	TIM2->ARR=target;
	TIM2->PSC=39999;
	TIM2->EGR = TIM_EGR_UG;
}

void Timer_start (){
// TODO : start timer and show the time on the 7-SEG LED.
	TIM2->CR1 |= TIM_CR1_CEN;
}

int main (){
	GPIO_init();
	max7219_init();
	int target=(TIME_SEC)*100;
	Timer_init(target);
	Timer_start();
	int now=0,pre=0;
	while (1){
		// TODO : Polling the timer count and do lab requirements
		now=TIM2->CNT;
		if(now>=pre){
			pre=now;
			display(now);
		}
		else{
			display(pre);
		}

	}
	return 0;
}

