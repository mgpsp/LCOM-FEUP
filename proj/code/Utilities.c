#include "Utilities.h"

int drawNumber(int number, int x, int y)
{
	int i, length, width;
	int d[5];

	i = 0;

	if (number == 0)
	{
		d[i] = 0;
		i++;
	}

	while (number > 0)
	{
		d[i] = number % 10;
		number = number / 10;
		i++;
	}

	length = i;

	for (i = 0; i < length; i++)
	{
		width = digits[d[i]]->bitmapInfoHeader.width;
		x -= width;
		if (i != 0)
			x -= 2;
		drawBitmap(digits[d[i]], x, y, ALIGN_LEFT);
	}

	return x;
}
