#include <minix/drivers.h>

#include "timer.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
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
			"\t service run %s -args \"test_square <freq>\" \n"
			"\t service run %s -args \"test_int <time>\" \n"
			"\t service run %s -args \"test_config <timer>\" \n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	unsigned long timer, freq, time;

	if (strncmp(argv[1], "test_square", 11) == 0)
	{
		if (argc != 3)
		{
			printf("timer: wrong no of arguments for timer_test_square()");
			return 1;
		}

		if ((freq = parse_ulong(argv[2], 10)) == ULONG_MAX) return 1;

		printf("timer:: timer_test_square(%lu)\n", freq);
		return timer_test_square(freq);
	}
	else if (strncmp(argv[1], "test_int", 8) == 0)
	{
		if (argc != 3)
		{
			printf("timer: wrong no of arguments for timer_test_int()");
			return 1;
		}

		if ((time = parse_ulong(argv[2], 10)) == ULONG_MAX) return 1;

		printf("timer:: timer_test_int(%lu)\n", time);
		return timer_test_int(time);
	}
	else if (strncmp(argv[1], "test_config", 11) == 0)
	{
		if (argc != 3)
		{
			printf("timer: wrong no of arguments for timer_test_config()");
			return 1;
		}

		if ((timer = parse_ulong(argv[2], 10)) == ULONG_MAX) return 1;

		printf("timer:: timer_test_config(%lu)\n", timer);
		return timer_test_config(timer);
	}
	else
	{
		printf("timer: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
  char *endptr;
  unsigned long val;

  val = strtoul(str, &endptr, base);

  if ((errno == ERANGE && val == ULONG_MAX ) || (errno != 0 && val == 0))
  {
	  perror("strtol");
	  return ULONG_MAX;
  }

  if (endptr == str)
  {
	  printf("timer: parse_ulong: no digits were found in %s \n", str);
	  return ULONG_MAX;
  }

  //Successful conversion
  return val;
}
