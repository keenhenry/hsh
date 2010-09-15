#include "hsh.h"

int do_main(void)
{
	/* initialize shell */
	init_shell();
	
	/* execute commands from command line */
	execute_line();

	/* clean up hsh memory*/
	clean_shell();

	return 0;
}

int main(void)
{
	return do_main();
}
