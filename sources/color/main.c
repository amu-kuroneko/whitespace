//
//  main.c
//  whitespace
//
//  Created by kuroneko on 2015/01/19.
//  Copyright (c) 2015年 kuroneko. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB_COLOR "\x1b[43m"
#define SPACE_COLOR "\x1b[46m"
#define DEFAULT_COLOR "\x1b[0m"

enum { TAB , SPACE , DEFAULT } typedef color;

int main( void ){
    char buffer[1024] , *position , current;
    color previous = DEFAULT;
    while( fgets( buffer , 1024 , stdin ) != NULL ){
        position = buffer;
        while( ( current = *position++ ) != '\0' ){
            if( current == '\t' ){
                if( previous != TAB ){
                    printf( TAB_COLOR );
                    previous = TAB;
                }
                current = 'T';
            }
            else if( current == ' ' ){
                if( previous != SPACE ){
                    printf( SPACE_COLOR );
                    previous = SPACE;
                }
                current = 'S';
            }
            else{
                if( previous != DEFAULT ){
                    printf( DEFAULT_COLOR );
                    previous = DEFAULT;
                }
            }
            fputc( current , stdout );
        }
    }
    fputs( DEFAULT_COLOR , stdout );
    return EXIT_SUCCESS;
}
