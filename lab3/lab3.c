#include <minix/drivers.h>

#include "test3.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv)
{
	/* Initialize service */
	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

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
			"\t service run %s -args \"test_scan <ass>\" \n"
			"\t service run %s -args \"test_leds <size> <array>\" \n"
			"\t service run %s -args \"test_timed_scan <time>\" \n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	unsigned short ass, size, array, time, i, elm, k;

	if (strncmp(argv[1], "test_scan", 9) == 0)
	{
		if (argc != 3)
		{
			printf("test3: wrong no of arguments for kbd_test_scan()");
			return 1;
		}

		if ((ass = parse_ulong(argv[2], 10)) == ULONG_MAX) return 1;

		printf("test3:: kbd_test_scan(%d)\n", ass);
		return kbd_test_scan(ass);
	}
	else if (strncmp(argv[1], "test_leds", 9) == 0)
	{
		if (argc < 4)
		{
			printf("test3: wrong no of arguments for kbd_test_leds()");
			return 1;
		}

		if ((size = parse_ulong(argv[2], 10)) == ULONG_MAX) return 1;

		if (argc != (3 + size))
		{
			printf("Array size doesn't match the number of elements\n");
			return 1;
		}

		unsigned short c[size];
		for (i = 0, k = 3; i < size; i++, k++)
		{
			if ((elm = parse_ulong(argv[k], 10)) == ULONG_MAX) return 1;
			c[i] = elm;
		}

		printf("test3:: kbd_test_leds(%d, {", size);
		for (i = 0; i < size; i++)
		{
			if ((i + 1) != size) printf("%d, ", c[i]);
			else printf("%d})\n", c[i]);
		}
		return kbd_test_leds(size, c);
	}
	else if (strncmp(argv[1], "test_timed_scan", 15) == 0)
	{
		if (argc != 3)
		{
			printf("test3: wrong no of arguments for kbd_test_timed_scan()");
			return 1;
		}

		if ((time = parse_ulong(argv[2], 10)) == ULONG_MAX) return 1;

		printf("test3:: kbd_test_timed_scan(%d)\n", time);
		return kbd_test_timed_scan(time);
	}
	else
	{
		printf("test3: non valid function \"%s\" to test\n", argv[1]);
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
	  printf("test3: parse_ulong: no digits were found in %s \n", str);
	  return ULONG_MAX;
  }

  //Successful conversion
  return val;
}
