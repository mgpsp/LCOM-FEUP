#include <minix/drivers.h>

#include "test4.h"

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
			"\t service run %s -args \"packet <count>\" \n"
			"\t service run %s -args \"async <idle_time>\" \n"
			"\t service run %s -args \"config\" \n"
			"\t service run %s -args \"gesture <length> <tolerance>\" \n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	unsigned short count, idle_time, tolerance;
	short length;

	if (strncmp(argv[1], "packet", 6) == 0)
	{
		if (argc != 3)
		{
			printf("mouse: wrong no of arguments for test_packet()\n");
			return 1;
		}

		if ((count = (unsigned short) parse_long(argv[2], 10)) == LONG_MAX) return 1;

		printf("mouse:: test_packet(%d)\n", count);
		return test_packet(count);
	}
	else if (strncmp(argv[1], "async", 5) == 0)
	{
		if (argc != 3)
		{
			printf("mouse: wrong no of arguments for test_async()\n");
			return 1;
		}

		if ((idle_time = (unsigned short) parse_long(argv[2], 10)) == LONG_MAX) return 1;

		printf("mouse:: timer_test_int(%d)\n", idle_time);
		return test_async(idle_time);
	}
	else if (strncmp(argv[1], "config", 5) == 0)
	{
		if (argc != 2)
		{
			printf("mouse: wrong no of arguments for test_config()\n");
			return 1;
		}

		printf("mouse:: test_config()\n");
		return test_config();
	}
	else if (strncmp(argv[1], "gesture", 7) == 0)
	{
		if (argc != 4)
		{
			printf("mouse: wrong no of arguments for test_gesture()\n");
			return 1;
		}

		if ((length = (short) parse_long(argv[2], 10)) == LONG_MAX) return 1;
		if ((tolerance = (unsigned short) parse_long(argv[3], 10)) == LONG_MAX) return 1;

		printf("mouse:: test_gesture(%d, %d)\n", length, tolerance);
		return test_gesture(length, tolerance);
	}
	else
	{
		printf("mouse: non valid function \"%s\" to test\n", argv[1]);
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
	  printf("mouse: parse_long: no digits were found in %s \n", str);
	  return LONG_MAX;
  }

  //Successful conversion
  return val;
}
