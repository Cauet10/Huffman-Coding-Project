#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node
{
    int x, y;
    double g, h, f;
    int parent;
}node;

