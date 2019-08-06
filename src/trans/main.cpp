
#include "do_trans.h"
#include "front_desk.h"

int main()
{
    do_trans();

    front_desk desk;
    desk.start();

    return 0;
}
