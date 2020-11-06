//These functions inside the asm file
extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

/**
* TODO: Show data on 7-seg via max7219_send
* Input:
* data: decimal value
* num_digs: number of digits will show on 7-seg
* Return:
* 0: success
* -1: illegal data range(out of 8 digits range)
*/

int display(int data, int num_digs){
	int up=1,down;
	while(data>0){
		down=data%10;
		data/=10;
		max7219_send(up,down);
		up++;
	}
	max7219_send(up,0);
	up++;
	max7219_send(up,15);
	return 0;
}

int main(){
	int student_id = 716049;
	GPIO_init();
	max7219_init();
	display(student_id, 8);
	return 0;
}
