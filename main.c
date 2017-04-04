#include <stdio.h>

#include "option_parser.h"
#include "core.h"


int main(int argc, char *argv[])
{
    hma_option main_opt;

    init_option(&main_opt);
    if (0 != parse_option(argc, argv, &main_opt)) {
        return 1;
    }
    list_option(&main_opt);
    switch(main_opt.action) {
        case ACTION_EMPTY:
            printf("you must specify an action, see %s -h\n", argv[0]);
            break;
        case ACTION_PRINT:
            print_byopt(&main_opt);
            break;
        case ACTION_CLEAR:
            clear_byopt(&main_opt);
            break;
        default:
            break;
    }
    return 0;
}
