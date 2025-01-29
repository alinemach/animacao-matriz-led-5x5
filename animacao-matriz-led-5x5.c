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
// By default the stdout UART is uart0, so we will use the second one
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

// Definição de pixel GRB
struct npLED_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct npLED_t npLED_t;

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

npLED_t leds[NUM_PIXELS];

// Inicializa a máquina PIO para controle da matriz de LEDs.
void npInit(uint pin) {

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &pio_matrix_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }
  // Inicia programa na máquina PIO obtida.
  pio_matrix_program_init(np_pio, sm, offset, pin);

  // Limpa buffer de pixels.
  for (uint i = 0; i < NUM_PIXELS; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

// Atribui uma cor RGB a um LED. Index possui range (0, 25)
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

// Limpa o buffer de pixels.
void npClear() {
  for (uint i = 0; i < NUM_PIXELS; ++i)
    npSetLED(i, 0, 0, 0);
}

// Escreve os dados do buffer nos LEDs.
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < NUM_PIXELS; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

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

void animacao1();

void animacao2 ();

void leds_all_blue_max() {
    int position = 0;
    int R = 0;
    int G = 0;
    int B = 255;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            npSetLED(position, R, G, B);
            position += 1;
        }
    }
    npWrite();
}

void leds_all_red_80() {
    int position = 0;
    int R = (int)255*0.8;
    int G = 0;
    int B = 0;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            npSetLED(position, R, G, B);
            position += 1;
        }
    }
    npWrite();
}

void leds_all_green_50() {
    int position = 0;
    int R = 0;
    int G = (int)255*0.5;
    int B = 0;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            npSetLED(position, R, G, B);
            position += 1;
        }
    }
    npWrite();
}

void leds_all_white_20() {
    int position = 0;
    int val = (int)255*0.2;
    int R = val;
    int G = val;
    int B = val;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            npSetLED(position, R, G, B);
            position += 1;
        }
    }
    npWrite();
}

// Função principal
int main() {
    // Inicializar o teclado matricial
    stdio_init_all();
    npInit(OUT_PIN);

    while (true) {
        teclado_init();
        char key = teclado_scan();  // Verifica qual tecla foi pressionada
        if (key != 0) {
            printf("Tecla pressionada: %c\n", key);
            // Dependendo da tecla pressionada, aciona um desenho na matriz de LEDs
            switch (key) {
                case 'A':
                    npClear();
                    npWrite();
                    break;
                case 'B':
                    leds_all_blue_max();
                    break;
                case 'C':
                    leds_all_red_80();
                    break;
                case 'D':
                    leds_all_green_50();
                    break;
                case '#':
                    leds_all_white_20();
                    break;
                case '1':
                    animacao1();
                    break;
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

void animacao1() {
    PIO pio = pio0;
    bool ok;
    uint32_t valor_led;
    double r = 1.0, g = 0.0, b = 0.0; // Vermelho
    double letraE[25] = {0.5, 0.5, 0.5, 0.5, 0.5, 
                         0.0, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.0, 
                         0.0, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.5};
    double letraM[25] = {0.5, 0.5, 0.0, 0.5, 0.5, 
                         0.5, 0.0, 0.5, 0.0, 0.5, 
                         0.5, 0.0, 0.5, 0.0, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5};
    double letraB[25] = {0.5, 0.5, 0.5, 0.5, 0.0, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.0};
    double letraA[25] = {0.0, 0.5, 0.5, 0.5, 0.0, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5};
    double letraR[25] = {0.5, 0.5, 0.5, 0.5, 0.0, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.0, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5};
    double letraC[25] = {0.5, 0.5, 0.5, 0.5, 0.5, 
                         0.0, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.0, 
                         0.0, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.5};
    double letraT[25] = {0.5, 0.5, 0.5, 0.5, 0.5, 
                         0.0, 0.0, 0.5, 0.0, 0.0, 
                         0.0, 0.0, 0.5, 0.0, 0.0, 
                         0.0, 0.0, 0.5, 0.0, 0.0, 
                         0.0, 0.0, 0.5, 0.0, 0.0};
    double letraH[25] = {0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.5, 0.5, 0.5, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5, 
                         0.5, 0.0, 0.0, 0.0, 0.5};
    double apagado[25] = {0.0, 0.0, 0.0, 0.0, 0.0, 
                          0.0, 0.0, 0.0, 0.0, 0.0, 
                          0.0, 0.0, 0.0, 0.0, 0.0, 
                          0.0, 0.0, 0.0, 0.0, 0.0, 
                          0.0, 0.0, 0.0, 0.0, 0.0};

    ok = set_sys_clock_khz(128000, false); // Configura o clock
    stdio_init_all();
    if (ok) printf("Clock set to %ld\n", clock_get_hz(clk_sys));

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Vetor de letras para "EMBARCATECH"
    double *letras[] = {letraE, letraM, letraB, letraA, letraR, letraC, letraA, letraT, letraE, letraC, letraH};
    size_t total_letras = sizeof(letras) / sizeof(letras[0]);

    // Exibição das letras
    for (size_t i = 0; i < total_letras; i++) {
        vermelho_pio(letras[i], valor_led, pio, sm, r, g, b);
        sleep_ms(2000); // Pausa entre letras
        vermelho_pio(apagado, valor_led, pio, sm, r, g, b); // Apaga entre letras
        sleep_ms(200); // Pausa para apagar
    }
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