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
    
    else{
        int hasEmpty = 0;
        for (int r = 0; r < 4; r++){
            for (int c = 0; c < 4; c++){
                if(game_matrix[r][c] == 0) hasEmpty = 1;
            }
        }

        if(hasEmpty) addItemToMatrix();
    }
}

void printMatrix()
{
    nokia_lcd_clear();
    int n;
    char text[4];
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            n = game_matrix[row][col];
            sprintf(text, "%d", n);
            if(n == 1024){
                text[0] = 'k';
                text[1] = '2';
                text[2] = '4';
                text[3] = '\0';
            }

            if(n == 0) {
                text[0] = ' ';
            }

            nokia_lcd_set_cursor(0+ (col * 22), 0 + (row * 13));
            nokia_lcd_write_string(text, 1);
        }
    }

    nokia_lcd_render();
}

int checkGameOver(){
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if(game_matrix[row][col] == 0) return 0;
            if(game_matrix[row][col] == game_matrix[row+1][col]) return 0;
            if(game_matrix[row][col] == game_matrix[row][col+1]) return 0;
        }
    }

    return 1;
}

int main(void)
{
    DDRD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3));
    PORTD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3));

    int v = time(0);
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1, glyph);
    nokia_lcd_write_string("2048", 2);
    nokia_lcd_set_cursor(0, 16);
    nokia_lcd_write_string("Pressione qualquer botao para comecar!\001", 1);
    nokia_lcd_render();

    int z = 0;
    int win = 0;
    int menu = 0;
    srand(v);   
    while (1)
    {
        _delay_ms(1);
        if(menu){
            z = 0;
            for(int row=0; row < 4; row++) {
                for(int col=0; col < 4; col++){
                    game_matrix[row][col] = 0;
                }
            }
            
            nokia_lcd_clear();
            nokia_lcd_custom(1, glyph);
            if(win) nokia_lcd_write_string("Vitoria", 2);
            else nokia_lcd_write_string("Derrota", 2);
            nokia_lcd_set_cursor(0, 16);
            nokia_lcd_write_string("Pressione qualquer botao para comecar!\001", 1);
            nokia_lcd_render();
        }
        _delay_ms(1);
        if(!z){
            if (PIND & ((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3))){
                z = 1;
                menu = 0;
                srand(v); 
                addItemToMatrix();
                addItemToMatrix();
                while (PIND & (1 << PD0))
                    _delay_ms(1);
            }

            _delay_ms(1); // debounce
            v++;
        }
        _delay_ms(1);
        if(z){
            printMatrix();
            if (PIND & (1 << PD0))
            {
                int hasMoves = 0;
                for (int col = 0; col < 4; col++){
                    int lastRow = 0;

                    for (int row = 0; row < 4; row++){
                        int value = game_matrix[row][col];  
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[lastRow][col] = value;
                            if(row != lastRow)hasMoves = 1;
                            lastRow++;
                        }
                    }
                    
                    for (int row = 0; row < 3; row++){
                        int value = game_matrix[row][col];
                        if(value != 0 && value == game_matrix[row+1][col]){
                            if (value*2 == 2048) {menu = 1; win = 1;}
                            game_matrix[row][col] = value*2;
                            game_matrix[row+1][col] = 0;
                            hasMoves = 1;
                        }
                    }

                    lastRow = 0;
                    for (int row = 0; row < 4; row++){
                        int value = game_matrix[row][col];  
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[lastRow][col] = value;
                            lastRow++; 
                        }
                    }
                }

                if (hasMoves) addItemToMatrix();

                while (PIND & (1 << PD0))
                    _delay_ms(1);
            }
            _delay_ms(1); // debounce

            if (PIND & (1 << PD2))
            {
                int hasMoves = 0;
                for (int col = 0; col < 4; col++){
                    int lastRow = 3;

                    for (int row = 3; row >= 0; row--){
                        int value = game_matrix[row][col];
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[lastRow][col] = value;
                            if(row != lastRow)hasMoves = 1;
                            lastRow--;   
                        }
                    }

                    for (int row = 3; row > 0; row--){
                        int value = game_matrix[row][col];
                        if(value != 0 && value == game_matrix[row-1][col]){
                            if (value*2 == 2048) {menu = 1; win = 1;}
                            game_matrix[row][col] = value*2;
                            game_matrix[row-1][col] = 0;
                            hasMoves = 1;
                        }
                    }

                    lastRow = 3;
                    for (int row = 3; row >= 0; row--){
                        int value = game_matrix[row][col];
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[lastRow][col] = value;
                            lastRow--;   
                        }
                    }
                }

                if (hasMoves) addItemToMatrix();

                while (PIND & (1 << PD2))
                    _delay_ms(1);
            }
            _delay_ms(1); // debounce

            if (PIND & (1 << PD3))
            {
                int hasMoves = 0;
                for (int row = 0; row < 4; row++){
                    int lastCol = 0;
                    for (int col = 0; col < 4; col++){
                        int value = game_matrix[row][col];
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[row][lastCol] = value;
                            if(col != lastCol)hasMoves = 1;
                            lastCol++;   
                        }
                    }

                    for (int col = 0; col < 3; col++){
                        int value = game_matrix[row][col];
                        if(value != 0 && value == game_matrix[row][col+1]){
                            if (value*2 == 2048) {menu = 1; win = 1;}
                            game_matrix[row][col] = value * 2;
                            game_matrix[row][col+1] = 0;
                            hasMoves = 1;
                        }
                    }

                    lastCol = 0;
                    for (int col = 0; col < 4; col++){
                        int value = game_matrix[row][col];
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[row][lastCol] = value;
                            lastCol++;   
                        }
                    }
                }

                if (hasMoves) addItemToMatrix();

                while (PIND & (1 << PD3))
                    _delay_ms(1);
            }
            _delay_ms(1); // debounce

            if (PIND & (1 << PD1))
            {
                int hasMoves = 0;
                for (int row = 0; row < 4; row++){
                    int lastCol = 3;
                    for (int col = 3; col >= 0; col--){
                        int value = game_matrix[row][col];
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[row][lastCol] = value;
                            if(col != lastCol)hasMoves = 1;
                            lastCol--;   
                        }
                    }

                    for (int col = 3; col > 0; col--){
                        int value = game_matrix[row][col];
                        if(value != 0 && value == game_matrix[row][col-1]){
                            if (value*2 == 2048) {menu = 1; win = 1;}
                            game_matrix[row][col] = value * 2;
                            game_matrix[row][col-1] = 0;
                            hasMoves = 1;
                        }
                    }

                    lastCol = 3;
                    for (int col = 3; col >= 0; col--){
                        int value = game_matrix[row][col];
                        if(value != 0){
                            game_matrix[row][col] = 0;
                            game_matrix[row][lastCol] = value;
                            lastCol--;   
                        }
                    }
                }

                if (hasMoves) addItemToMatrix();

                while (PIND & (1 << PD1))
                    _delay_ms(1);
            }
            _delay_ms(1); // debounce
            
            if(menu) continue;                              

            printMatrix();
            menu = checkGameOver();
        }
    }
}