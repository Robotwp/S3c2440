#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

void print_usage(char *file)
{
	printf("Usage:\n");
	printf("%s <dev> <on|off>\n", file);
	printf("eg. \n");
	printf("%s /dev/leds on\n", file);
	printf("%s /dev/leds off\n", file);
	printf("%s /dev/led1 on\n", file);
	printf("%s /dev/led1 off\n", file);
}

int main(int argc, char **argv)
{
	int fd;
	char* filename;
	int val = 0;
	if (argc != 3)
	{
		print_usage(argv[0]);
		return 0;
	}

	filename = argv[1];
	fd = open(filename, O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	
	val = strcmp(argv[2], "on")?0:1;
	printf("val=%d\n", val);

	write(fd, &val, 4);
	return 0;
}
