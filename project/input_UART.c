void init_GPIO_UART(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN;
	SET_REG(GPIOA->MODER,0x3c0000,0x280000);   // pa9,pa10 ==> af mode
	SET_REG(GPIOA->AFR[1],0xff0,0x770);       // af7
}

void init_UART(){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = 0x1a0;   // 4M/9600
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_UE;
}

void input_init(){
	init_GPIO_UART();
	init_UART();
}

int main(void) {
	input_init();
	while(1){
		char c;
		if(USART1->ISR & USART_ISR_RXNE){
			c=USART1->RDR;
			USART1->TDR=c;
		}

	}
	return 0;
}
