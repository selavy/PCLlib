#include <stdio.h>
#include <assert.h>

#define NAME  pcl
#define KEY   int
#define VALUE int
#include "PCLlib/loa_table.h"

int main(int argc, char** argv)
{
    int rc;
    pcltable table;

    table = pcl_create();
    pcl_init(table);

    rc = pcl_resize_unsafe(table, 32);
    assert(rc == 0);

    pcl_destroy(table);

    return 0;
}
