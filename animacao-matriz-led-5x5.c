#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdlib.h>
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

//arquivo .pio
#include "animacao-matriz-led-5x5.pio.h"

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// Definir número de LEDs e pino de saída
#define NUM_PIXELS 25
#define OUT_PIN 7

// Definir pinos do teclado matricial
#define ROWS 4
#define COLS 4
const uint row_pins[ROWS] = {0, 1, 2, 3};   // Linhas conectadas aos pinos 0, 1, 2, 3
const uint col_pins[COLS] = {4, 5, 6, 8};   // Colunas conectadas aos pinos 4, 5, 6, 7

// Mapeamento das teclas
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Função para inicializar o teclado
void teclado_init() {
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);  // Inicializa as linhas em nível alto
    }

    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);  // Habilita o pull-up para as colunas
    }
}

// Função para ler as teclas pressionadas
char teclado_scan() {
    for (int row = 0; row < ROWS; row++) {
        for (int i = 0; i < 4; i++) {
            gpio_put(row_pins[i], 1); // Coloca todas as linhas em nível alto
        }
        gpio_put(row_pins[row], 0);  // Coloca a linha atual em nível baixo

        for (int col = 0; col < COLS; col++) {
            if (!gpio_get(col_pins[col])) {  // Se a tecla for pressionada
                return keys[row][col];  // Retorna o caractere correspondente
            }
        }

        gpio_put(row_pins[row], 1);  // Coloca a linha de volta em nível alto
    }

    return 0;  // Nenhuma tecla pressionada
}

// Função para definir a intensidade das cores do LED
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para acionar a matriz de LEDs vermelhos (ws2812b)
void vermelho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(b = 0.0, desenho[24 - i], g =0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
}

void animacao2 ();

// Função principal
int main() {
    // Inicializar o teclado matricial
    teclado_init();

    while (true) {
        char key = teclado_scan();  // Verifica qual tecla foi pressionada

        if (key != 0) {
            printf("Tecla pressionada: %c\n", key);
            // Dependendo da tecla pressionada, aciona um desenho na matriz de LEDs
            switch (key) {
                case '2':
                    animacao2();
                    break;
                default:
                    // Caso nenhuma tecla seja pressionada, a matriz de LEDs é apagada

                    break;
            }
        }

        sleep_ms(200);  // Pausa para evitar leituras repetidas
    }

    return 0;
}

void animacao2 (){
PIO pio = pio0; 
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

// Configura o clock do sistema
    ok = set_sys_clock_khz(128000, false);
    stdio_init_all();

    if (ok) printf("Clock set to %ld\n", clock_get_hz(clk_sys));

    // Configurações do PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

//vetores para criar animação "SUBGRUPO8" //
double letraS[25] =   {0.5, 0.5, 0.5, 0.5, 0.5,
                        0.0, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.5, 0.5, 0.5, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.0,
                        0.5, 0.5, 0.5, 0.5, 0.5};
double letraU[25] =   {0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.5, 0.5, 0.5, 0.5};

double letraB[25] =   {0.5, 0.5, 0.5, 0.5, 0.0,
                        0.5, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.5, 0.5, 0.5, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.5, 0.5, 0.5, 0.0};

double letraG[25] =   {0.5, 0.5, 0.5, 0.5, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.0, 0.5, 0.5, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.5, 0.5, 0.5, 0.5};

double letraR[25] =   {0.5, 0.5, 0.5, 0.5, 0.0,
                        0.5, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.5, 0.5, 0.5, 0.0,
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5};

double letraP[25] =   {0.5, 0.5, 0.5, 0.5, 0.0,
                        0.5, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.5, 0.5, 0.5, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.0};

double letraO[25] =   {0.5, 0.5, 0.5, 0.5, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.5, 0.5, 0.5, 0.5};

double num8[25] =   {0.5, 0.5, 0.5, 0.5, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5, 
                        0.5, 0.5, 0.5, 0.5, 0.5,
                        0.5, 0.0, 0.0, 0.0, 0.5,
                        0.5, 0.5, 0.5, 0.5, 0.5};

double apagado[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0};

vermelho_pio(letraS, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(letraU, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(letraB, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(letraG, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(letraR, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(letraU, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(letraP, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(letraO, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(num8, valor_led, pio, sm, r, g, b);
sleep_ms(500);
vermelho_pio(apagado, valor_led, pio, sm, r, g, b);

}