#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdlib.h>
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"

//arquivo .pio
#include "animacao-matriz-led-5x5.pio.h"
#include "ws2818b.pio.h"

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

//CÓDIGO REFERENTE A ANIMAÇÃO 3
// Biblioteca gerada pelo arquivo .pio durante compilação.


#define BUZZER 21 //PINO 21 DEFINIDO COMO BUZZER.
#define DO 261.63 //Nota Dó
#define DOh 277.18 //Dó#
#define RE 293.66 //Nota Ré
#define REh 311.13//Ré#
#define MI 329.63 //Nota Mi
#define FA 349.23 //Nota Fá
#define FAh 369.99//Fá#0,,,,,,,
#define SOL 392 //Nota Sol
#define SOLh 415.30 //Sol#
#define LA 440 //Nota Lá
#define LAh 466.16 //Lá#
#define SI 493.88 //Nota Si
#define SIh 493.88 //Sí#

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

int matriz1[5][5][3] = {
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}},
    {{255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{0, 0, 0}, {0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {250, 249, 241}, {255, 242, 0}},
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}}
};

int matriz2[5][5][3] = {
    {{255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}},
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{33, 54, 255}, {0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {250, 249, 241}, {255, 242, 0}},
    {{255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}}
};

int matriz3[5][5][3] = {
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}},
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{255, 242, 0}, {33, 54, 255}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {250, 249, 241}, {255, 242, 0}},
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}}
};

int matriz4[5][5][3] = {
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}},
    {{255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}},
    {{255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {250, 249, 241}, {255, 242, 0}},
    {{0, 0, 0}, {255, 242, 0}, {255, 242, 0}, {255, 242, 0}, {0, 0, 0}}
};

int matriz5[5][5][3] = {
    {{33, 54, 255}, {0, 0, 0}, {33, 54, 255}, {0, 0, 0}, {33, 54, 255}},
    {{33, 54, 255}, {33, 54, 255}, {33, 54, 255}, {33, 54, 255}, {33, 54, 255}},
    {{33, 54, 255}, {33, 54, 255}, {33, 54, 255}, {33, 54, 255}, {33, 54, 255}},
    {{33, 54, 255}, {0, 0, 0}, {33, 54, 255}, {0, 0, 0}, {33, 54, 255}},
    {{0, 0, 0}, {33, 54, 255}, {33, 54, 255}, {33, 54, 255}, {0, 0, 0}}
};

int matriz6[5][5][3] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
};

int linha;

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
    };
    typedef struct pixel_t pixel_t;
    typedef pixel_t npLEED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

    // Declaração do buffer de pixels que formam a matriz.
    npLEED_t leeds[LED_COUNT];

    // Variáveis para uso da máquina PIO.
    PIO np_pio;
    uint sm;

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit1(uint pin) {

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED1(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear1() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED1(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite1() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Função para converter a posição do matriz para uma posição do vetor.
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return y * 5 + x; // Linha par (esquerda para direita).
    } else {
        return y * 5 + (4 - x); // Linha ímpar (direita para esquerda).
    }
}

void BUZZER_CONFIG (uint gpio, float frequencia, float duty_cycle){
    gpio_set_function(gpio, GPIO_FUNC_PWM); //DEFINE O PINO COMO SAÍDA PWM
    uint slice_num = pwm_gpio_to_slice_num(gpio); //OBTEM O NUMERO DA SLICE ASSOCIADA AO PINO
    uint channel = pwm_gpio_to_channel(gpio); //OBETEM O NÚMERO DO CANAL

    float clk_div = 8.0f;  // DIVISOR DE CLOCK
    uint32_t wrap = (uint32_t)((125e6 / (frequencia * clk_div)) - 1);
   
    // CONFIGURAÇÃO PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clk_div);
    pwm_config_set_wrap(&config, wrap);
    pwm_init(slice_num, &config, true);
    
    // CONFIGURAÇÃO DUTY_CICLE
     uint32_t level = (uint32_t)(wrap * duty_cycle);
    pwm_set_chan_level(slice_num, channel, level);
    pwm_set_enabled(slice_num, true);

}

void CONTROLE_BUZZER(){

    BUZZER_CONFIG(BUZZER, DO, 0.5f); //CONFIGURA FREQUENCIA E DUTY_CYCLE
        sleep_ms(500);
        
        
        uint slice_num = pwm_gpio_to_slice_num(BUZZER);
        uint channel = pwm_gpio_to_channel(BUZZER);
        pwm_set_chan_level(slice_num, channel, 0);  //DESLIGAR BUZZER

}
// Função para desligar o buzzer
void DESLIGAR_BUZZER() {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER);
    uint channel = pwm_gpio_to_channel(BUZZER);
    pwm_set_chan_level(slice_num, channel, 0);  // Desliga o buzzer
    pwm_set_enabled(slice_num, false);
}

// Função para tocar várias notas
void TOCAR_NOTAS() {
    float notas[] = {SI, SI, FAh, REh, SI, FAh, REh,
DO, DO, SOL, MI, DO, SOL, MI,
SI, SI, FAh, DOh, SI, FAh, REh,
RE, REh, MI, FA, SOL, SOLh, LA, LAh, SI}; // Array com as notas a serem tocadas
    int num_notas = sizeof(notas) / sizeof(notas[0]);

    for (int i = 0; i < num_notas; i++) {
        BUZZER_CONFIG(BUZZER, notas[i], 0.5f); // Toca cada nota com duty cycle de 50%
        sleep_ms(200);  // Intervalo de 200ms entre as notas
        DESLIGAR_BUZZER();  // Desliga o buzzer após tocar a nota
        
    }
}

// Função para tocar várias notas
void TOCAR_SOM() {
    float notas[] = {600}; // Array com as notas a serem tocadas
    int num_notas = sizeof(notas) / sizeof(notas[0]);

    for (int i = 0; i < num_notas; i++) {
        BUZZER_CONFIG(BUZZER, notas[i], 0.5f); // Toca cada nota com duty cycle de 50%
        sleep_ms(50);  // Intervalo de 150ms entre as notas
    float notas[] = {0}; // Array com as notas a serem tocadas
    int num_notas = sizeof(notas) / sizeof(notas[0]);

    for (int i = 0; i < num_notas; i++) {
        BUZZER_CONFIG(BUZZER, notas[i], 0.5f); // Toca cada nota com duty cycle de 50%
        sleep_ms(20);  // Intervalo de 150ms entre as notas        
        DESLIGAR_BUZZER();  // Desliga o buzzer após tocar a nota
    }
    }
}

void TOCAR_SOM_FANTASMA() {
    float notas[] = {MI}; // Array com as notas a serem tocadas
    int num_notas = sizeof(notas) / sizeof(notas[0]);

    for (int i = 0; i < num_notas; i++) {
        BUZZER_CONFIG(BUZZER, notas[i], 0.5f); // Toca cada nota com duty cycle de 50%
        sleep_ms(120);  // Intervalo de 150ms entre as notas
    float notas[] = {0}; // Array com as notas a serem tocadas
    int num_notas = sizeof(notas) / sizeof(notas[0]);

    for (int i = 0; i < num_notas; i++) {
        BUZZER_CONFIG(BUZZER, notas[i], 0.5f); // Toca cada nota com duty cycle de 50%
        sleep_ms(10);  // Intervalo de 150ms entre as notas        
        DESLIGAR_BUZZER();  // Desliga o buzzer após tocar a nota

        float notas[] = {DO}; // Array com as notas a serem tocadas
    int num_notas = sizeof(notas) / sizeof(notas[0]);

    for (int i = 0; i < num_notas; i++) {
        BUZZER_CONFIG(BUZZER, notas[i], 0.5f); // Toca cada nota com duty cycle de 50%
        sleep_ms(120);  // Intervalo de 150ms entre as notas        
        DESLIGAR_BUZZER();  // Desliga o buzzer após tocar a nota

        float notas[] = {0}; // Array com as notas a serem tocadas
    int num_notas = sizeof(notas) / sizeof(notas[0]);

    for (int i = 0; i < num_notas; i++) {
        BUZZER_CONFIG(BUZZER, notas[i], 0.5f); // Toca cada nota com duty cycle de 50%
        sleep_ms(10);  // Intervalo de 150ms entre as notas        
        DESLIGAR_BUZZER();  // Desliga o buzzer após tocar a nota
    }
    }
    }
    }
}

void FRAMEINICIAL(){
      for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
          int index = getIndex(x, y);
          npSetLED1(index, matriz1[y][x][0], matriz1[y][x][1], matriz1[y][x][2]);
      }
    }
    npWrite1(); // Escreve os dados nos LEDs.
    TOCAR_NOTAS();
}


void FRAME1(){
      for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
          int index = getIndex(x, y);
          npSetLED1(index, matriz1[y][x][0], matriz1[y][x][1], matriz1[y][x][2]);
      }
    }
}

void FRAME2(){
      for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
          int index = getIndex(x, y);
          npSetLED1(index, matriz2[y][x][0], matriz2[y][x][1], matriz2[y][x][2]);
      }
    }
}

void FRAME3(){
    for (int y = 0; y < 5; y++) {
      for (int x = 0; x < 5; x++) {
        int index = getIndex(x, y);
        npSetLED1(index, matriz3[y][x][0], matriz3[y][x][1], matriz3[y][x][2]);
      }
    }
}

void FRAME4(){
      for (int y = 0; y < 5; y++) {
      for (int x = 0; x < 5; x++) {
        int index = getIndex(x, y);
        npSetLED1(index, matriz4[y][x][0], matriz4[y][x][1], matriz4[y][x][2]);
      }
    }
}

void FRAME5(){
      for (int y = 0; y < 5; y++) {
      for (int x = 0; x < 5; x++) {
        int index = getIndex(x, y);
        npSetLED1(index, matriz5[y][x][0], matriz5[y][x][1], matriz5[y][x][2]);
      }
    }
}

void FRAME6(){
      for (int y = 0; y < 5; y++) {
      for (int x = 0; x < 5; x++) {
        int index = getIndex(x, y);
        npSetLED1(index, matriz6[y][x][0], matriz6[y][x][1], matriz6[y][x][2]);
      }
    }
}

void LEDS() {

  // Inicializa entradas e saídas.
  stdio_init_all();

  // Inicializa matriz de LEDs NeoPixel.
  npInit1(LED_PIN);
  npClear1();

    // Atualiza os LEDs com o padrão da matriz.
    FRAMEINICIAL();
    npWrite1(); // Escreve os dados nos LEDs.
    sleep_ms(1500); // Delay de 1 segundo antes de repetir.
   for (int i=0; i<5; i++){
    TOCAR_SOM();
    FRAME1();
    npWrite1();
    sleep_ms(100);
    FRAME2();
    npWrite1();
    sleep_ms(100);
    FRAME3();
    npWrite1();
    sleep_ms(100);
    FRAME4();
    npWrite1();
    sleep_ms(100);
   }
  for (int v=0; v<5; v++){
    TOCAR_SOM_FANTASMA();
    FRAME5();
    npWrite1();
    sleep_ms(500);
    FRAME6();
    npWrite1();
    sleep_ms(500);
  }
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

                case '3':
                    LEDS();
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