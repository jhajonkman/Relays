//
//  Main file
//  ----------------------------------
//  Developed with embedXcode
//
//  Relays
//  Created by jeroenjonkman on 13-06-15
//   


#include "WProgram.h"
#include "Relays.pde"

int main(void)
{
    init();
	setup();
	for (;;) loop();
	return 0;
}
