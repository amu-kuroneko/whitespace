//
//  main.c
//  whitespace
//
//  Created by kuroneko on 2015/01/18.
//  Copyright (c) 2015年 kuroneko. All rights reserved.
//

#include "whitespace.h"

#define LINE_LENGTH 30

char buffer[BUFFER_SIZE];

void line( int length );

int main(int argc, const char * argv[])
{

	FILE *file = stdin;
	
	if( 2 < argc && strcmp( argv[1] , FILE_OPTION ) == 0 ){
		if( ( file = fopen( argv[2] , "r" ) ) == NULL ){
			fputs( "open file error.\n" , stderr );
			return EXIT_FAILURE;
		}
		fputs( "source loading" , stdout );
	}

	size_t count;
	int index = 0;
	while( ( count = fread( buffer , sizeof( char ) , BUFFER_SIZE , file ) ) != 0 ){
		buffer[count] = '\0';
		setProgram( buffer , BUFFER_SIZE );
		if( file != stdin && index++ % 10 == 0 ){
			fputc( '.' , stdout );
		}
	}
	fputc( '\n' , stdout );
	if( feof( file ) == 0 ){
		fputs( "read error!\n" , stderr );
	}
	fputs( "load finished\n" , stdout );
	
	fputs( "initialize instruction\n" , stdout );
	Instruction *instruction = getInstruction();
	
	programClear();
	fputs( "initialize finished\n\n" , stdout );
    
    fputs( "disassemble start\n" , stdout );
    line( LINE_LENGTH );
	disassemble( instruction );
    line( LINE_LENGTH );
    fputs( "disassemble finished\n\n" , stdout );
	
	fputs( "program start\n" , stdout );
    line( LINE_LENGTH );
	execute( instruction );
	
    line( LINE_LENGTH );
	fputs( "program finish\n" , stdout );

    fputc( '\n' , stdout );
	fputs( "end process\n" , stdout );
	freeInstruction( instruction );
	stackClear();
    heapClear();
	fputs( "all finished\n" , stdout );
	
    return EXIT_SUCCESS;
}


void line( int length ){
    while( length-- ){
        fputc( '-' , stdout );
    }
    fputc( '\n' , stdout );
    return;
}
