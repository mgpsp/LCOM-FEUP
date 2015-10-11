#include <minix/drivers.h>

#include "test5.h"
#include "video_gr.h"
#include "pixmap.h"

static int proc_args(int argc, char *argv[]);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv)
{
	/* Initialize service */

	sef_startup();

	if (argc == 1)
	{
		print_usage(argv);
		return 0;
	}
	else proc_args(argc, argv);

	return 0;
}

static void print_usage(char *argv[])
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init <mode> <delay>\" \n"
			"\t service run %s -args \"square <x> <y> <size> <color>\" \n"
			"\t service run %s -args \"line <initial_x> <initial_y> <final_x> <final_y> <color>\" \n"
			"\t service run %s -args \"xpm <initial_x> <initial_y> <picture>\" \n"
			"\t service run %s -args \"move <initial_x> <initial_y> <picture> <horizontal> <distance> <time>\" \n"
			"\t service run %s -args \"controller\" \n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	unsigned short mode, delay, x, y, size, initial_x, initial_y, final_x, final_y, horizontal, time;
	unsigned long color;
	short distance;

	if (strncmp(argv[1], "init", 4) == 0)
	{

		if (argc != 4)
		{
			printf("wrong no of arguments for test_init()\n");
			return 1;
		}

		if ((mode = (unsigned short) parse_long(argv[2], 16)) == LONG_MAX) return 1;
		if ((delay = (unsigned short) parse_long(argv[3], 10)) == LONG_MAX) return 1;

		printf("test_init(0x%x, %d)\n", mode, delay);

		test_init(mode, delay);

		return 0;
	}
	else if (strncmp(argv[1], "square", 6) == 0)
	{
		if (argc != 6)
		{
			printf("wrong no of arguments for test_square()\n");
			return 1;
		}

		if ((x = (unsigned short) parse_long(argv[2], 10)) == LONG_MAX) return 1;
		if ((y = (unsigned short) parse_long(argv[3], 10)) == LONG_MAX) return 1;
		if ((size = (unsigned short) parse_long(argv[4], 10)) == LONG_MAX) return 1;
		if ((color = (unsigned long) parse_long(argv[5], 10)) == LONG_MAX) return 1;

		printf("test_square(%d, %d, %d, %d)\n", x, y, size, color);

		return test_square(x, y, size, color);
	}
	else if (strncmp(argv[1], "line", 4) == 0)
	{
		if (argc != 7)
		{
			printf("wrong no of arguments for test_line()\n");
			return 1;
		}

		if ((initial_x = (unsigned short) parse_long(argv[2], 10)) == LONG_MAX) return 1;
		if ((initial_y = (unsigned short) parse_long(argv[3], 10)) == LONG_MAX) return 1;
		if ((final_x = (unsigned short) parse_long(argv[4], 10)) == LONG_MAX) return 1;
		if ((final_y = (unsigned short) parse_long(argv[5], 10)) == LONG_MAX) return 1;
		if ((color = (unsigned long) parse_long(argv[6], 10)) == LONG_MAX) return 1;

		printf("test_linee(%d, %d, %d, %d, %d)\n", initial_x, initial_y, final_x, final_y, color);

		return test_line(initial_x, initial_y, final_x, final_y, color);
	}
	else if (strncmp(argv[1], "xpm", 3) == 0)
	{
		if (argc != 5)
		{
			printf("wrong no of arguments for test_xpm()\n");
			return 1;
		}

		if ((initial_x = (unsigned short) parse_long(argv[2], 10)) == LONG_MAX) return 1;
		if ((initial_y = (unsigned short) parse_long(argv[3], 10)) == LONG_MAX) return 1;

		if (strncmp(argv[4], "pic1", 4) == 0)
		{
			printf("test_xpm(%d, %d, %s)\n", initial_x, initial_y, "pic1");

			return test_xpm(initial_x, initial_y, pic1);
		}
		else if (strncmp(argv[4], "pic2", 4) == 0)
		{
			printf("test_xpm(%d, %d, %s)\n", initial_x, initial_y, "pic2");

			return test_xpm(initial_x, initial_y, pic2);
		}
		else if (strncmp(argv[4], "cross", 5) == 0)
		{
			printf("test_xpm(%d, %d, %s)\n", initial_x, initial_y, "cross");

			return test_xpm(initial_x, initial_y, cross);
		}
		else if (strncmp(argv[4], "pic3", 4) == 0)
		{
			printf("test_xpm(%d, %d, %s)\n", initial_x, initial_y, "pic3");

			return test_xpm(initial_x, initial_y, pic3);
		}
		else if (strncmp(argv[4], "penguin", 7) == 0)
		{
			printf("test_xpm(%d, %d, %s)\n", initial_x, initial_y, "penguin");

			return test_xpm(initial_x, initial_y, penguin);
		}
	}
	else if (strncmp(argv[1], "move", 4) == 0)
	{
		if (argc != 8)
		{
			printf("wrong no of arguments for test_move()\n");
			return 1;
		}

		if ((initial_x = (unsigned short) parse_long(argv[2], 10)) == LONG_MAX) return 1;
		if ((initial_y = (unsigned short) parse_long(argv[3], 10)) == LONG_MAX) return 1;
		if ((horizontal = (unsigned short) parse_long(argv[5], 10)) == LONG_MAX) return 1;
		if ((distance = (short) parse_long(argv[6], 10)) == LONG_MAX) return 1;
		if ((time = (unsigned short) parse_long(argv[7], 10)) == LONG_MAX) return 1;

		if (strncmp(argv[4], "pic1", 4) == 0)
		{
			printf("test_move(%d, %d, %s, %d, %d, %d)\n", initial_x, initial_y, "pic1", horizontal, distance, time);

			return test_move(initial_x, initial_y, pic1, horizontal, distance, time);
		}
		else if (strncmp(argv[4], "pic2", 4) == 0)
		{
			printf("test_move(%d, %d, %s, %d, %d, %d)\n", initial_x, initial_y, "pic2", horizontal, distance, time);

			return test_move(initial_x, initial_y, pic2, horizontal, distance, time);
		}
		else if (strncmp(argv[4], "cross", 5) == 0)
		{
			printf("test_move(%d, %d, %s, %d, %d, %d)\n", initial_x, initial_y, "cross", horizontal, distance, time);

			return test_move(initial_x, initial_y, cross, horizontal, distance, time);
		}
		else if (strncmp(argv[4], "pic3", 4) == 0)
		{
			printf("test_move(%d, %d, %s, %d, %d, %d)\n", initial_x, initial_y, "pic3", horizontal, distance, time);

			return test_move(initial_x, initial_y, pic3, horizontal, distance, time);
		}
		else if (strncmp(argv[4], "penguin", 7) == 0)
		{
			printf("test_move(%d, %d, %s, %d, %d, %d)\n", initial_x, initial_y, "penguin", horizontal, distance, time);

			return test_move(initial_x, initial_y, penguin, horizontal, distance, time);
		}

	}
	else if(strncmp(argv[1], "controller", 10) == 0)
	{
		printf("test_controller()\n");

		return test_controller();
	}
	else
	{
		printf("non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static long parse_long(char *str, int base)
{
	char *endptr;
	long val;
	val = strtol(str, &endptr, base);

	if ((errno == ERANGE && val == LONG_MAX ) || (errno != 0 && val == 0))
	{
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str)
	{
		printf("parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	//Successful conversion

	return val;
}
