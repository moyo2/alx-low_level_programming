/**
 * get_endianness - a function that check if
 *                  architecture is little Endian
 *                  or big Endian
 *
 * Return: 1 if little endian or 0 if big endian
*/
int get_endianness(void)
{
	unsigned int num = 1;
	char *c = (char *) &num;

	return (c[0]);
}
