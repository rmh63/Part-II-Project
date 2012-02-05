/*
 * main.h
 */

void output(); //A function prototype

typedef struct {
    double x;
    double y;
    double z;
} Vector;

typedef struct {
    Vector position;
    Vector velocity;
    Vector force;
    double mass;
} DataItem;
