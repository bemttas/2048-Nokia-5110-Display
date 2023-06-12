#include <avr/io.h>
#include <time.h>
#include <stdio.h>
#include <util/delay.h>

#include "nokia5110.h"

uint8_t glyph[] = {0b00010000, 0b00100100, 0b11100000, 0b00100100, 0b00010000};
int game_matrix[4][4] = {0};

void addItemToMatrix(){
    int row = rand() % 4;
    int col = rand() % 4;

    if (game_matrix[row][col] == 0)
        game_matrix[row][col] = 2;
}

void printMatrix()
{
    nokia_lcd_clear();
    int n;
    char text[3];
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            n = game_matrix[row][col];
            sprintf(text, "%d", n);

            nokia_lcd_set_cursor(16 + (col * 20), 7 + (row * 11));
            nokia_lcd_write_string(text, 1);
        }
    }

    nokia_lcd_render();
}

int main(void)
{
    DDRD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3));
    PORTD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3));
    DDRB &= ~(1 << PB0);  // seta PB0 como entrada
    PORTB &= ~(1 << PB0); // desabilita pull-up

    int v = time(0);
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1, glyph);
    nokia_lcd_write_string("20488", 2);
    nokia_lcd_set_cursor(0, 16);
    nokia_lcd_write_string("Pressione qualquer botao para comecar!\001", 1);
    nokia_lcd_render();

    int z = 0;
    srand(v);
    while (1)
    {
        _delay_ms(1);
        if(!z){
            if (PIND & (1 << PD0)){
                z = 1;
                game_matrix[3][3] = 2;
                game_matrix[1][3] = 2;
                while (PIND & (1 << PD0))
                    _delay_ms(1);
            }

            _delay_ms(1); // debounce
            v++;
        }
        srand(v);
        if(z){
            printMatrix();
            if (PIND & (1 << PD0))
            {
                for (int col = 0; col < 4; col++){
                    int lastRow = 0;
                    for (int row = 0; row < 4; row++){
                        int value = game_matrix[row][col];
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[lastRow][col] = value;
                            lastRow++;   
                        }
                    }

                    for (int row = 0; row < 3; row++){
                        if(game_matrix[row][col] == game_matrix[row+1][col]){
                            game_matrix[row][col] *= 2;
                            game_matrix[row+1][col] = 0;
                        }
                    }
                }
                addItemToMatrix();
                while (PIND & (1 << PD0))
                    _delay_ms(1);
            }
            _delay_ms(1); // debounce

            printMatrix();
        }
    }
}