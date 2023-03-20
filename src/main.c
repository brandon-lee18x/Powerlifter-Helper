#include "accelerometer.h"
#include "SysClock.h"
#include <stdio.h>
#include "qpn_port.h"

void PWM_Init() {
	// Enable GPIO Port A Clock
	// [TODO]
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Enable TIM2 Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	// Configure PA5
	GPIOA->MODER ^= GPIO_MODER_MODE5_0; //0x00000400U
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5; //0x00000C00U
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5; //0x00000C00U
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5; //clear register
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0; //0x00100000U
	
	// Configure PWM Output for TIM2 CH 1
	TIM2->CR1 &= ~TIM_CR1_DIR; //set to upcounting
	TIM2->PSC = 0x0003;//0x0F9F; //prescaler value
	TIM2->ARR = 1000;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; //clear compare mode bits
	TIM2->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); //0110
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; //enable output preload
	TIM2->CCER &= ~TIM_CCER_CC1P; //set output polarity to active high
	TIM2->CCER |= TIM_CCER_CC1E; //enable channel 1 output
	TIM2->CCR1 = 500;
	TIM2->CR1 |= TIM_CR1_CEN; //enable counter
	
}

void accelerometer_fsm() {
    int state = 0;
    //0 = top, 1 = descent, 2 = pause, 3 = ascent
    switch(state) {
        case (0):
            state = 1;
            break;
        case (1):
            break;
        case (2):
            break;
        case (3):
            break;
    }
}

int main(void){
	System_Clock_Init(); // System Clock = 4 MHz
	// Initialize I2C
	PWM_Init();
	I2C_GPIO_Init();
	I2C_Initialization();
	accelerometer_init();
	int i;
	uint8_t SlaveAddress;
    float accel_x;
	uint16_t accel_x_raw;
    uint8_t accel_x_h;
    uint8_t accel_x_l;
	
    float accel_y;
    uint16_t accel_y_raw;
    uint8_t accel_y_h;
    uint8_t accel_y_l;
	
    float accel_z;
    uint16_t accel_z_raw;
    uint8_t accel_z_h;
    uint8_t accel_z_l;
	uint8_t Data_Send;
	while(1) {	
		// Slave Address: Note the "<< 1" must be present because bit 0 is treated as a don't care in 7-bit addressing mode
		SlaveAddress = 0b1101000 << 1; // address of accelerometer if AD0 = 0
		float d = 16384;

        //x_accel
	    Data_Send = 0x3B;
	    I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
	    I2C_ReceiveData(I2C1, SlaveAddress, &accel_x_h, 1);
        Data_Send = 0x3C;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_x_l, 1);
        accel_x_raw = (accel_x_h << 8) | accel_x_l;
        accel_x = accel_x_raw / d; //gives accel in g's

        //y_accel
        Data_Send = 0x3D;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_y_h, 1);
        Data_Send = 0x3E;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_y_l, 1);
        accel_y_raw = (accel_y_h << 8) | accel_y_l;
        accel_y = accel_y_raw / d; //gives accel in g's

        //z_accel
        Data_Send = 0x3F;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_z_h, 1);
        Data_Send = 0x40;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_z_l, 1);
        accel_z_raw = (accel_z_h << 8) | accel_z_l;
        accel_z = accel_z_raw / d; //gives accel in g's
        // have watch window for accel_x, accel_y, and accel_z
        for(i = 0; i < 50000; ++i); 
    }

}