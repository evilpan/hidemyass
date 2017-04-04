#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "../core.h"

void test_time()
{
    time_t seconds = 1491220836;

    const char *ptime = seconds2str(seconds);
    printf("%s\n", ptime);
    assert( strcmp("2017:04:03:20:00:36", ptime) == 0 );
    assert(seconds == str2seconds(ptime));


    ptime = seconds2str(0);
    printf("%s\n", ptime);
    assert( strcmp("1970:01:01:08:00:00", ptime) == 0 );
    assert(0 == str2seconds(ptime));

    ptime = seconds2str(66666666666);
    printf("%s\n", ptime);
    assert ( strcmp("4082:08:01:06:31:06", ptime) == 0 );
    assert ( str2seconds("4082:08:01:06:31:06") == 66666666666 );

    ptime = seconds2str(-10);
    printf("%s\n", ptime);
    assert( strcmp("1970:01:01:07:59:50", ptime) == 0 );
    assert ( str2seconds("1970:01:01:07:59:50") == -10 );

    assert( -1 == str2seconds("0000:00:00:00:00:00") );
    assert( -1 == str2seconds("abcd:ef:gh:ij:kh:lm") );
    assert( -1 == str2seconds("xxoo") );
    assert( -1 == str2seconds("2017:04:03") );


    printf("test time pass\n");
}

int main()
{
    test_time();
    return 0;
}
