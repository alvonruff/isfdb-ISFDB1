#include <stdio.h>
#ifdef AIX
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <stdlib.h>
#include <unistd.h>


int
parseline(int fd)
{
	unsigned char	input;
	int		result;

	while(1) {
		result = read(fd, &input, 1);
		if (result <= 0) {
			return(1);
		} else if (input == '\n') {
			return(0);
		} else {
			printf("%c", input);
		}
	}
}


int
main(int argc, char *argv[])
{
	int fd, loop;

	if (argc != 2) {
		fprintf(stderr, "tileawards <file>\n");
		exit(1);
	}

#ifdef CYGWIN
	fd = open(argv[1], O_RDONLY|O_BINARY);
#else
	fd = open(argv[1], O_RDONLY);
#endif
	if (fd == -1) {
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		exit(1);
	}
	
	printf("<pre>\n");
	while(1) {
		for(loop=0; loop<5; loop++) {
			if (parseline(fd))
				goto done;
		}
		printf("\n<br>\n");
	}
done:
	printf("\n</pre>\n");
}

