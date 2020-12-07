#include "stm32l476xx.h"
#define SET_REG(REG, MASK, VAL) {((REG)=((REG) & (~(MASK))) | (VAL));};

#define ADC_CCR_CKMODE_Pos 16
#define ADC_CCR_DELAY_Pos 8
#define ADC_SQR1_SQ1_Pos 6
#define ADC_SMPR1_SMP1_Pos 3

int state=0;

void SysTick_config () {
	// SysTick
	SysTick->LOAD=249999;
	SET_REG(SysTick->CTRL, 0x00000007, 0x00000003 );  // external clock == > msi/8 // enable
}

void startADC();
void printint();

void SysTick_Handler () {
	if(state==1){
		startADC();
		printint();
	}
}

void configureADC() {
// TODO setting ADC register
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;  // enable
	ADC1->CFGR &= ~ADC_CFGR_RES;                    // resolution 12-bit
	ADC1->CFGR &= ~ADC_CFGR_CONT;                   // single conversion mode
	ADC1->CFGR &= ~ADC_CFGR_ALIGN;                  // right alignment

	ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;            // independent mode
	ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;          // HCLK / 1
	ADC123_COMMON->CCR |= 1 << ADC_CCR_CKMODE_Pos;
	ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;           // prescaler: div 1
	ADC123_COMMON->CCR &= ~ADC_CCR_MDMA;            // disable DMA
	ADC123_COMMON->CCR &= ~ADC_CCR_DELAY;
	ADC123_COMMON->CCR |= 4 << ADC_CCR_DELAY_Pos;

	ADC1->SQR1 &= ~(ADC_SQR1_SQ1);                  // channel: 1, rank: 1
	ADC1->SQR1 |= 1 << ADC_SQR1_SQ1_Pos;

	ADC1->SMPR1 &= ~ADC_SMPR1_SMP1;                 // ADC sample  cycle  // pre 2.5 ADC clock cycle
//  ADC1->SMPR1 |= 2 << ADC_SMPR1_SMP1_Pos;

	ADC1->CR &= ~ADC_CR_DEEPPWD;                    // disable deeppwd
	ADC1->CR |= ADC_CR_ADVREGEN;                    // enable ADC voltage regulator
//  ADC1->IER |= ADC_IER_EOCIE;                     // enable end of conversion interrupt
//  NVIC_EnableIRQ(ADC1_2_IRQn);
	ADC1->CR |= ADC_CR_ADEN;                        // enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY));           // wait for ADC startup

}

void startADC() {
	// TODO enable ADC
	ADC1->CR |= ADC_CR_ADSTART;  // start adc

}

int UART_Transmit(char *s) {
	int cnt=0;
	for(int i=0;s[i]!='\0';i++){
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

	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN);  // enable gpio a
	SET_REG(GPIOA->MODER, 0xc00 , 0x400);    // pa5 output // led

	SET_REG(GPIOC->MODER, 0x3, 0x3 );  // pc0 Analog mode
	SET_REG(GPIOC->ASCR, 0x1, 0x1 );  // connect to ADC input

}

void init_UART(){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = 0x1a0;   // 4M/9600
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_UE;
}

void printint(){
	int a=(int) ADC1->DR;
	char c[100];
	int i=0;
	if(a==0){
		char s[4];
		s[0]='0';
		s[1]='\r';
		s[2]='\r';
		s[3]='\0';
		UART_Transmit(s);
	}
	while(a>0){
		c[i]=a%10+'0';
		i++;
		a/=10;
	}
	i--;
	int j=0;
	char s[100];
	while(i>=0){
		s[j]=c[i];
		j++;
		i--;
	}
	s[j]='\r';
	j++;
	s[j]='\n';
	j++;
	s[j]='\0';
	UART_Transmit(s);
}

int button(){
	int x;
	int cnt=8000;
	x=ADC1->DR;
	while(cnt--){
		x=GPIOC->IDR;
		x&=0x00002000;
		if(x!=0){
			return 0;
		}
	}
	startADC();
	printint();
	cnt=10000;
	while(cnt){
		x=GPIOC->IDR;
		x&=0x00002000;
		if(x!=0) cnt--;
		else cnt=10000;
	}
	return 0;
}

int cm(char *c1,char *c2){
	int i=0;
	while(c1[i]==c2[i]){
		i++;
		if(c1[i]=='\0'&&c2[i]=='\0'){
			return 1;
		}
		if(c1[i]=='\0'||c2[i]=='\0'){
			return 0;
		}
	}
	return 0;
}

void command(char *co){
	if(cm(co,"showid")){
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\r';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\n';
		while (!(USART1->ISR & USART_ISR_TXE));
		UART_Transmit("0716049\0");
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\r';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\n';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='>';
		while (!(USART1->ISR & USART_ISR_TXE));
	}
	else if(cm(co,"light")){
		state=1;
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\r';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\n';
		while (!(USART1->ISR & USART_ISR_TXE));
	}
	else if(cm(co,"led on")){
		GPIOA->BSRR=(1<<5);
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\r';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\n';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='>';
		while (!(USART1->ISR & USART_ISR_TXE));
	}
	else if(cm(co,"led off")){
		GPIOA->BRR=(1<<5);
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\r';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\n';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='>';
		while (!(USART1->ISR & USART_ISR_TXE));
	}
	else{
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\r';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\n';
		while (!(USART1->ISR & USART_ISR_TXE));
		UART_Transmit("Unknown Command\0");
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\r';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='\n';
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR='>';
		while (!(USART1->ISR & USART_ISR_TXE));
	}
}


int main(void) {
	init_GPIO();
	init_UART();
	SysTick_config();
	configureADC();
	startADC();
	USART1->TDR='>';
	char co[100];
	int i=0;
	char c;
	while(1){
		c=ADC1->DR;
		if(USART1->ISR & USART_ISR_RXNE){
			c=USART1->RDR;
			if(state==0&&(c=='\n'||c=='\r')){
				co[i]='\0';
				command(co);
				i=0;
			}
			else if(state==1&&c=='q'){
				state=0;
				while (!(USART1->ISR & USART_ISR_TXE));
				USART1->TDR='\r';
				while (!(USART1->ISR & USART_ISR_TXE));
				USART1->TDR='\n';
				while (!(USART1->ISR & USART_ISR_TXE));
				USART1->TDR='>';
				while (!(USART1->ISR & USART_ISR_TXE));
			}
			else if(state==0){
				if((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c==' '){
					USART1->TDR=c;
					co[i]=c;
					i++;
				}
				else if(c==0x7f){  // backspace
					USART1->TDR=c;
					if(i>=1){
						i--;
					}

				}
			}
		}

	}
	return 0;
}
