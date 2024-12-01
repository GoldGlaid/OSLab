#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POINTS_COUNT 30 // Количество точек в массиве
#define RANGE 100       // Диапазон значений координат

typedef struct {
    int x, y, z;
} Point;

void generate_points(Point points[], int count, int range) {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    for (int i = 0; i < count; i++) {
        points[i].x = rand() % (range * 2 + 1) - range;
        points[i].y = rand() % (range * 2 + 1) - range;
        points[i].z = rand() % (range * 2 + 1) - range;
    }
}

void print_points(Point points[], int count) {
    for (int i = 0; i < count; i++) {
        printf("%d, %d, %d\n", points[i].x, points[i].y, points[i].z);
    }
}

int main() {
    Point points[POINTS_COUNT];

    generate_points(points, POINTS_COUNT, RANGE);
    print_points(points, POINTS_COUNT);

    return 0;
}