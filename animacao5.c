#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pio_matrix_program.h"

#define OUT_PIN 2

void animacao5(); // Declaração da função antes da main

int main() {
    PIO pio = pio0;
    bool ok;

    ok = set_sys_clock_khz(128000, false);
    stdio_init_all();

    if (ok) printf("Clock set to %ld\n", clock_get_hz(clk_sys));

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    while (true) {
        animacao5();
    }
}

void animacao5() {
    double quadro1[25] = {
        0.5, 0.0, 0.0, 0.0, 0.5,
        0.0, 0.5, 0.0, 0.5, 0.0,
        0.0, 0.0, 0.5, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.5, 0.0,
        0.5, 0.0, 0.0, 0.0, 0.5
    };

    double quadro2[25] = {
        0.0, 0.5, 0.0, 0.5, 0.0,
        0.5, 0.0, 0.5, 0.0, 0.5,
        0.0, 0.5, 0.0, 0.5, 0.0,
        0.5, 0.0, 0.5, 0.0, 0.5,
        0.0, 0.5, 0.0, 0.5, 0.0
    };

    double quadro3[25] = {
        0.0, 0.0, 0.5, 0.0, 0.0,
        0.0, 0.5, 0.5, 0.5, 0.0,
        0.5, 0.5, 0.5, 0.5, 0.5,
        0.0, 0.5, 0.5, 0.5, 0.0,
        0.0, 0.0, 0.5, 0.0, 0.0
    };

    double apagado[25] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0
    };

    double* quadros[] = {quadro1, quadro2, quadro3, apagado};
    int num_quadros = sizeof(quadros) / sizeof(quadros[0]);

    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;
    PIO pio = pio0;
    uint sm = 0;

    for (int q = 0; q < num_quadros; q++) {
        for (int16_t i = 0; i < 25; i++) {
            valor_led = matrix_rgb(quadros[q][24 - i], r, g = 0.5);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        sleep_ms(500);
    }
}
