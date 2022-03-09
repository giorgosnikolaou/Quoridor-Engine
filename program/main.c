#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quoridor.h"
#include "moves.h"


int main()
{
    
   	Engine en = engine_create("George");


	while(1)
    	read_command(en);


    return 0;
}

