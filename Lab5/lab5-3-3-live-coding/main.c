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

int p1=-1,p2=-1;
int s1,s2;

/* TODO: initial keypad gpio pin, X as output and Y as input */
void keypad_init(){
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN, RCC_AHB2ENR_GPIOBEN);  // enable gpio b
	SET_REG(GPIOB->MODER, 0xff , 0x55);   // pb 0 1 2 3 output
	SET_REG(GPIOB->MODER, 0xff00 , 0x0000);   // pb 4 5 6 7 input
	SET_REG(GPIOB->PUPDR, 0xffff , 0xaa00);   // pb 4 5 6 7 pull down
}

void keypad_init2(){
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN, RCC_AHB2ENR_GPIOBEN);  // enable gpio b
		SET_REG(GPIOB->MODER, 0xff , 0x00);   // pb 0 1 2 3 input
		SET_REG(GPIOB->MODER, 0xff00 , 0x5500);   // pb 4 5 6 7 output
		SET_REG(GPIOB->PUPDR, 0xffff , 0x00aa);   // pb 4 5 6 7 pull down
}

/* TODO: scan keypad value
return: >=0: key-value pressed�A-1: keypad is free
*/
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


int keypad_scan(){
	int val;
	int i;
	int sum=0;
	int ch=0;
	int scan_p1=-1;
	int scan_p2=-1;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf , y_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf0);
		if(val==x_pin[0]) {
			sum+=value_table(i*4+0);
			if(scan_p1==-1) scan_p1=i*4+0;
			else scan_p2=i*4+0;
			ch=1;
		}
		else if(val==x_pin[1]) {
			sum+=value_table(i*4+1);
			if(scan_p1==-1) scan_p1=i*4+1;
			else scan_p2=i*4+1;
			ch=1;
		}
		else if(val==x_pin[2]) {
			sum+=value_table(i*4+2);
			if(scan_p1==-1) scan_p1=i*4+2;
			else scan_p2=i*4+2;
			ch=1;
		}
		else if(val==x_pin[3]) {
			sum+=value_table(i*4+3);
			if(scan_p1==-1) scan_p1=i*4+3;
			else scan_p2=i*4+3;
			ch=1;
		}
		else if(val==x_pin[0]+x_pin[1]) {
			sum=sum+value_table(i*4+0)+value_table(i*4+1);
			scan_p1=i*4+0;
			scan_p2=i*4+1;
			ch=2;
		}
		else if(val==x_pin[0]+x_pin[2]) {
			sum=sum+value_table(i*4+0)+value_table(i*4+2);
			scan_p1=i*4+0;
			scan_p2=i*4+2;
			ch=2;
		}
		else if(val==x_pin[0]+x_pin[3]) {
			sum=sum+value_table(i*4+0)+value_table(i*4+3);
			scan_p1=i*4+0;
			scan_p2=i*4+3;
			ch=2;
		}
		else if(val==x_pin[1]+x_pin[2]) {
			sum=sum+value_table(i*4+1)+value_table(i*4+2);
			scan_p1=i*4+1;
			scan_p2=i*4+2;
			ch=2;
		}
		else if(val==x_pin[1]+x_pin[3]) {
			sum=sum+value_table(i*4+1)+value_table(i*4+3);
			scan_p1=i*4+1;
			scan_p2=i*4+3;
			ch=2;
		}
		else if(val==x_pin[2]+x_pin[3]) {
			sum=sum+value_table(i*4+2)+value_table(i*4+3);
			scan_p1=i*4+2;
			scan_p2=i*4+3;
			ch=2;
		}
	}
	if(ch==2){
		scan_p1=value_table(scan_p1);
		scan_p2=value_table(scan_p2);
		if(scan_p1==p1){
			p2=scan_p2;
		}
		else if(scan_p2==p1){
			p2=scan_p1;
		}
		return sum;
	}
	else{
		scan_p1=-1;
		scan_p2=-1;
	}
	// change x&y's output and input
	keypad_init2();
	ch=0;
	sum=0;
	for(i=0;i<4;i++){
		SET_REG(GPIOB->ODR, 0xf0 , x_pin[i]);
		val=GPIOB->IDR;
		val=val&(0xf);
		if(val==y_pin[0]) {
			sum+=value_table(0*4+i);
			if(scan_p1==-1) scan_p1=0*4+i;
			else scan_p2=0*4+i;
			ch=1;
		}
		else if(val==y_pin[1]) {
			sum+=value_table(1*4+i);
			if(scan_p1==-1) scan_p1=1*4+i;
			else scan_p2=1*4+i;
			ch=1;
		}
		else if(val==y_pin[2]) {
			sum+=value_table(2*4+i);
			if(scan_p1==-1) scan_p1=2*4+i;
			else scan_p2=2*4+i;
			ch=1;
		}
		else if(val==y_pin[3]) {
			sum+=value_table(3*4+i);
			if(scan_p1==-1) scan_p1=3*4+i;
			else scan_p2=3*4+i;
			ch=1;
		}
		else if(val==y_pin[0]+y_pin[1]) {
			sum=sum+value_table(0*4+i)+value_table(1*4+i);
			scan_p1=0*4+i;
			scan_p2=1*4+i;
			ch=2;
		}
		else if(val==y_pin[0]+y_pin[2]) {
			sum=sum+value_table(0*4+i)+value_table(2*4+i);
			scan_p1=0*4+i;
			scan_p2=2*4+i;
			ch=2;
		}
		else if(val==y_pin[0]+y_pin[3]) {
			sum=sum+value_table(0*4+i)+value_table(3*4+i);
			scan_p1=0*4+i;
			scan_p2=3*4+i;
			ch=2;
		}
		else if(val==y_pin[1]+y_pin[2]) {
			sum=sum+value_table(1*4+i)+value_table(2*4+i);
			scan_p1=1*4+i;
			scan_p2=2*4+i;
			ch=2;
		}
		else if(val==y_pin[1]+y_pin[3]) {
			sum=sum+value_table(1*4+i)+value_table(3*4+i);
			scan_p1=1*4+i;
			scan_p2=3*4+i;
			ch=2;
		}
		else if(val==y_pin[2]+y_pin[3]) {
			sum=sum+value_table(2*4+i)+value_table(3*4+i);
			scan_p1=2*4+i;
			scan_p2=3*4+i;
			ch=2;
		}
	}
	keypad_init();
	if(ch==0){
		p1=-1;
		p2=-1;
		return 100;
	}
	else{
		scan_p1=value_table(scan_p1);
		scan_p2=value_table(scan_p2);
		if(scan_p2==16){
			p1=scan_p1;
			p2=-1;
		}
		else{
			if(scan_p1==p1){
				p2=scan_p2;
			}
			else if(scan_p2==p1){
				p2=scan_p1;
			}
		}
		return sum;
	}
}

void gpio_init(){
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc00 , 0x400);    // pa5 output // only test
	SET_REG(GPIOA->OSPEEDR, 0x800 , 0x800);   // only test
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN);  // enable gpio c
	SET_REG(GPIOC->MODER, 0x3f , 0x15);       // pc 0 1 2 output  // 7segment
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

void display1(int a,int b,int c){
	max7219_send(0x9,0xff);
	int up=1,down;
	if(c==0){
		max7219_send(up,0);
		up++;
	}
	while(c>0){
		down=c%10;
		c/=10;
		max7219_send(up,down);
		up++;
	}
	if(b>=0){
		max7219_send(up,15);
		up++;
	}
	if(b==0){
		max7219_send(up,0);
		up++;
	}
	while(b>0){
		down=b%10;
		b/=10;
		max7219_send(up,down);
		up++;
	}
	max7219_send(up,15);
	up++;
	if(a==0){
		max7219_send(up,0);
		up++;
	}
	while(a>0){
		down=a%10;
		a/=10;
		max7219_send(up,down);
		up++;
	}
	while(up<=8){
		max7219_send(up,15);
		up++;
	}
}

int main(){
	gpio_init();
	max7219_init();
	keypad_init();
	int n;
	while(1){
		n=keypad_scan();
		if(n==100){
			display(0,0);
		}
		else if(n>=10){
			display1(p1,p2,n);
			//display(n,2);
		}
		else if(n<10){
			display1(p1,p2,n);
			//display(n,1);
		}
	}
	return 0;
}
