#include "ws2812b.h"
#include "tim.h"

#define BIT_0_TIME		32
#define BIT_1_TIME		64

#define RESET_LEN		40
#define LED_N			7

static uint8_t led_buffer[RESET_LEN + 24 * LED_N + 1];

//Funkcja pomocnicza kodujaca 8-bitowa liczbe w postaci 8 bajtow aby bylo odpowiednie sterowanie
static void set_byte(uint32_t pos, uint8_t value) {

	int i;
	for(i = 0; i < 8; i++) {

		if(value & 0x80) {

			led_buffer[pos + i] = BIT_1_TIME;

		}
		else {

			led_buffer[pos + i] = BIT_0_TIME;

		}
		value <<= 1;
	}

}

//Funkcja inicjalizujaca dane do wyslania przy uzyciu licznika TIM3:
void ws2812b_init(void) {

	for(int i = 0; i < RESET_LEN; i++) {

		led_buffer[i] = 0;

	}

	for(int i = 0; i < 24 * LED_N; i++) {

		led_buffer[RESET_LEN + i] = BIT_0_TIME;

	}

	led_buffer[RESET_LEN + 24 * LED_N] = 100;

	HAL_TIM_Base_Start(&htim3);

	ws2812b_update();

}

//Funkcja wysylajaca dane przez PWM za pomoca DMA do modulu WS2812B:
void ws2812b_update(void) {

	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)led_buffer, sizeof(led_buffer));

}

//Funkcja czekajaca na koniec transmisji przez DMA:
void ws2812b_wait(void) {

	while(HAL_TIM_GetChannelState(&htim3, TIM_CHANNEL_1) == HAL_TIM_CHANNEL_STATE_BUSY) {

	}

}

//Funkcja ustawiajaca kolor diod:
void ws2812b_set_color(uint32_t led, uint8_t red, uint8_t green, uint8_t blue) {

	if(led < LED_N) {

		set_byte(RESET_LEN + 24 * led, green);
		set_byte(RESET_LEN + 24 * led + 8, red);
		set_byte(RESET_LEN + 24 * led + 16, blue);

	}

}
