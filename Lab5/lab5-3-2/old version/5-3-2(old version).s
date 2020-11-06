#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};

extern void max7219_send(unsigned char address, unsigned char data);

//TODO: define your gpio pin

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

/* TODO: initial keypad gpio pin, X as output and Y as input */
void keypad_init(){
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN, RCC_AHB2ENR_GPIOBEN);  // enable gpio b
	SET_REG(GPIOB->MODER, 0xff , 0x55);   // pb 0 1 2 3 output
	SET_REG(GPIOB->MODER, 0xff00 , 0x0000);   // pb 4 5 6 7 input
	SET_REG(GPIOB->PUPDR, 0xff00 , 0xaa00);   // pb 4 5 6 7 pull down
}

/* TODO: scan keypad value
return: >=0: key-value pressed，-1: keypad is free
*/

int keypad_scan(){
	int val;
	int i,j;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf , y_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf0);
		for(j=0;j<4;j++){
			if(val==x_pin[j]) return i*4+j;
		}
	}
	return 16;

/*
 	 int re=16;
	SET_REG(GPIOB->ODR, 0xf , y_pin[0]);
	val=GPIOB->IDR;
	val=val&(0xf0);
	if(val==x_pin[0]) re=1;
	else if(val==x_pin[1]) re=2;
	else if(val==x_pin[2]) re=3;
	else if(val==x_pin[3]) re=10;
	SET_REG(GPIOB->ODR, 0xf , y_pin[1]);
	val=GPIOB->IDR;
	val=val&(0xf0);
	if(val==x_pin[0]) re=4;
	else if(val==x_pin[1]) re=5;
	else if(val==x_pin[2]) re=6;
	else if(val==x_pin[3]) re=11;
	SET_REG(GPIOB->ODR, 0xf , y_pin[2]);
	val=GPIOB->IDR;
	val=val&(0xf0);
	if(val==x_pin[0]) re=7;
	else if(val==x_pin[1]) re=8;
	else if(val==x_pin[2]) re=9;
	else if(val==x_pin[3]) re=12;
	SET_REG(GPIOB->ODR, 0xf , y_pin[3]);
	val=GPIOB->IDR;
	val=val&(0xf0);
	if(val==x_pin[0]) re=15;
	else if(val==x_pin[1]) re=0;
	else if(val==x_pin[2]) re=14;
	else if(val==x_pin[3]) re=13;
	return re;
	*/

}

int value_table(int po){
	if(po==0) return 1;
	if(po==1) return 2;
	if(po==2) return 3;
	if(po==3) return 10;
	if(po==4) return 4;
	if(po==5) return 5;
	if(po==6) return 6;
	if(po==7) return 11;
	if(po==8) return 7;
	if(po==9) return 8;
	if(po==10) return 9;
	if(po==11) return 12;
	if(po==12) return 15;
	if(po==13) return 0;
	if(po==14) return 14;
	if(po==15) return 13;
	return 16;
}

void gpio_init(){
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc00 , 0x400);    // pa5 output
	SET_REG(GPIOA->OSPEEDR, 0x800 , 0x800);
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN);  // enable gpio c
	SET_REG(GPIOC->MODER, 0x3f , 0x15);       // pc 0 1 2 output
	SET_REG(GPIOC->OSPEEDR, 0x3f , 0x2a);
}

void max7219_init(){
	max7219_send(0x9,0xff);  // decode mode  // decode for code B on digit 0~7
	max7219_send(0xf,0x0);   // display test  //  off(normal display)
	max7219_send(0xa,0xa);  // intensity
	max7219_send(0xb,0x7);   // scan limit  // digit 0~7
	max7219_send(0xc,0x1);    // shutdown  // off(normal display)
}


int display(int data, int num_digs){
	max7219_send(0x9,0xff);  // decode mode  // decode for code B on digit 0~7
	int up=1,down;
	while(data>0&&num_digs!=0){
		down=data%10;
		data/=10;
		max7219_send(up,down);
		up++;
		if(up>num_digs){
			break;
		}
	}
	while(up<=num_digs){
		max7219_send(up,0);
		up++;
	}
	while(up<=8){
		max7219_send(up,15);
		up++;
	}
	return 0;
}

int main(){
	gpio_init();
	max7219_init();
	keypad_init();
	int n;
	while(1){
		n=keypad_scan();
		n=value_table(n);
		if(n>=10&&n<=15){
			display(n,2);
		}
		else if(n<10){
			display(n,1);
		}
		else{
			display(0,0);
		}
	}
	//display(59999,5);
	//GPIOA->ODR=0x20;
	//uint32_t a=GPIOA->ODR;
	//if(a==0x20){
	//	a++;
	//}
	return 0;
}
