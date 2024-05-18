#include <stdio.h>
#include <stdlib.h>

void
f(void)
{
    int a[4];
    int *b = malloc(16);
    int *c;
    int i;

    printf("1: a = %p, b = %p, c = %p\n", a, b, c);

    c = a;
    for (i = 0; i < 4; i++)
	a[i] = 100 + i;
    c[0] = 200;
    printf("2: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
	   a[0], a[1], a[2], a[3]);
    /* I think the answer should be "200, 101, 102, 103" */

    c[1] = 300;
    *(c + 2) = 301;
    /* Reference: https://stackoverflow.com/questions/5073350/accessing-arrays-by-indexarray-in-c-and-c */
    3[c] = 302;
    printf("3: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
	   a[0], a[1], a[2], a[3]);
    /* I think the answer should be "200, 300, 301, 302?" */

    c = c + 1;
    *c = 400;
    printf("4: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
	   a[0], a[1], a[2], a[3]);
    /* I think the answer should be "200, 400, 301, 302?" */

    c = (int *) ((char *) c + 1);
    *c = 500;
    printf("5: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
	   a[0], a[1], a[2], a[3]);
    /* I think the answer should be "200, 8000, 288, 302"
     * Wrong! I mistaken the byte-num of char and int.
     * Before this step, a[1] = 0x(00 00 01 90), a[2] = 0x(00 00 01 2D)
     * After the operation, as 500 = 0x1F4:
     * a[1] becomes 0x(00 01 F4 90) -> 128144, the top (00) goes to a[2]
     * so a[2] becomes 0x(00 00 01 00) -> 256
     */


    b = (int *) a + 1;
    c = (int *) ((char *) a + 1);
    printf("6: a = %p, b = %p, c = %p\n", a, b, c);
    /* I think the answer should be "A1, A1 + 4, A1 + 1"*/
}

int
main(int ac, char **av)
{
    f();
    return 0;
}
