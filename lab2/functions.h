#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>


typedef enum {
    OK,
    INVALID_ARGS,
    MEMORY_ERROR,
    INVALID_FILE,
    UNCORECT_TYPE_OF_NUMS,
    NULL_PTR,
} state;


typedef struct {
    double x, y, z;
} Point;

typedef struct {
    size_t size;
    size_t capacity;
    Point **array;
} PointMassive;

// === Потоки ====
typedef struct {
    // Массив точек
    PointMassive *massive;
    int num_threads; // Общее количество потоков
    int thread_id; // Уникальный ID потока
    double max_area; // Максимальная площадь, найденная этим потоком
    Point *max_points[3]; // Точки, образующие треугольник с максимальной площадью
} ThreadData;

// === Потоки ====

state is_digital(const char *str, double *x);

state get_word(FILE *input, char **buffer, char *c);

state get_point(FILE *file, double *x, double *y, double *z, char *symbol);

state add_points(FILE *file, PointMassive **massive);

state insert(PointMassive **massive, double x, double y, double z);

void freePointMassive(PointMassive *massive);

PointMassive *create_PointMassive(int capacity);

double get_area(double x, double y, double z);

double vector_length(double x, double y, double z);

double triangle_area(Point *a, Point *b, Point *c);

void *find_max_area_thread(void *arg);


state is_digital(const char *str, double *x) {
    char *endptr;
    double temp = strtod(str, &endptr);

    if (*endptr != '\0') {
        return UNCORECT_TYPE_OF_NUMS;
    }

    *x = temp;
    return OK;
}

state get_word(FILE *input, char **buffer, char *c) {
    int capacity = 2;

    if (input == NULL || *buffer == NULL)
        return NULL_PTR;

    int index = 0;
    *c = (char) fgetc(input);

    while (*c != ' ' && *c != '\n' && *c != '\t' && *c != EOF) {
        if (capacity == index) {
            capacity *= 2;
            char *temp_buffer = (char *) realloc(*buffer, capacity);
            if (temp_buffer == NULL) {
                free(*buffer);
                return MEMORY_ERROR;
            }
            *buffer = temp_buffer;
        }

        (*buffer)[index++] = *c;
        *c = (char) fgetc(input);
    }

    (*buffer)[index] = '\0';
    return OK;
}

state get_point(FILE *file, double *x, double *y, double *z, char *symbol) {
    char *str_x, *str_y, *str_z;

    str_x = (char *) malloc(sizeof(char) * 128);
    str_y = (char *) malloc(sizeof(char) * 128);
    str_z = (char *) malloc(sizeof(char) * 128);

    if (get_word(file, &str_x, symbol) == MEMORY_ERROR ||
        get_word(file, &str_y, symbol) == MEMORY_ERROR ||
        get_word(file, &str_z, symbol) == MEMORY_ERROR) {
        free(str_x);
        free(str_y);
        free(str_z);
        return MEMORY_ERROR;
    }

    if (is_digital(str_x, x) == UNCORECT_TYPE_OF_NUMS ||
        is_digital(str_y, y) == UNCORECT_TYPE_OF_NUMS ||
        is_digital(str_z, z) == UNCORECT_TYPE_OF_NUMS) {
        free(str_x);
        free(str_y);
        free(str_z);

        return UNCORECT_TYPE_OF_NUMS;
    }

    free(str_x);
    free(str_y);
    free(str_z);
    return OK;
}

PointMassive *create_PointMassive(int capacity) {
    PointMassive *massive = (PointMassive *) malloc(sizeof(PointMassive));
    if (massive == NULL) {
        return NULL;
    }

    massive->size = 0;
    massive->capacity = capacity;
    massive->array = (Point **) malloc(sizeof(Point *) * capacity);

    if (massive->array == NULL) {
        free(massive);
        return NULL;
    }

    for (int i = 0; i < capacity; i++) {
        massive->array[i] = NULL;
    }

    return massive;
}

state insert(PointMassive **massive, double x, double y, double z) {
    Point *element = (Point *) malloc(sizeof(Point));

    PointMassive *masive_p = (*massive);
    if (element == NULL) {
        return MEMORY_ERROR;
    }

    if (masive_p->size == masive_p->capacity) {
        masive_p->capacity *= 2;
        Point **temp_buffer = (Point **) realloc(masive_p->array, masive_p->capacity * sizeof(Point *));
        if (temp_buffer == NULL) {
            free(element);
            return MEMORY_ERROR;
        }
        masive_p->array = temp_buffer;
    }

    element->x = x;
    element->y = y;
    element->z = z;

    const size_t index = masive_p->size;
    masive_p->array[index] = element;

    masive_p->size++;
    return OK;
}

void freePointMassive(PointMassive *massive) {
    if (massive == NULL) return;

    for (int i = 0; i < massive->size; i++) {
        free(massive->array[i]);
    }
    free(massive->array);
    free(massive);
}


state add_points(FILE *file, PointMassive **massive) {
    while (!feof(file)) {
        double x, y, z;
        char symbol;

        state result = get_point(file, &x, &y, &z, &symbol);
        if (result == MEMORY_ERROR) {
            freePointMassive(*massive);
            return MEMORY_ERROR;
        }
        if (result == UNCORECT_TYPE_OF_NUMS) {
            freePointMassive(*massive);
            return UNCORECT_TYPE_OF_NUMS;
        }

        result = insert(massive, x, y, z);
        if (result == MEMORY_ERROR) {
            freePointMassive(*massive);
            return MEMORY_ERROR;
        }
    }

    return OK;
}

// Вычисление длины вектора
double vector_length(double x, double y, double z) {
    return sqrt(x * x + y * y + z * z);
}

// Вычисление площади треугольника, заданного тремя точками
double triangle_area(Point *a, Point *b, Point *c) {
    // Векторы AB и AC
    double ab_x = b->x - a->x, ab_y = b->y - a->y, ab_z = b->z - a->z;
    double ac_x = c->x - a->x, ac_y = c->y - a->y, ac_z = c->z - a->z;

    // Векторное произведение AB × AC
    double cross_x = ab_y * ac_z - ab_z * ac_y;
    double cross_y = ab_z * ac_x - ab_x * ac_z;
    double cross_z = ab_x * ac_y - ab_y * ac_x;

    // Длина вектора (модуль векторного произведения)
    double cross_length = vector_length(cross_x, cross_y, cross_z);

    // Площадь треугольника
    return 0.5 * cross_length;
}


// === Потоки ====
void *find_max_area_thread(void *arg) {
    ThreadData *data = (ThreadData *) arg;
    PointMassive *massive = data->massive;
    int num_points = massive->size;

    double max_area = 0.0;
    Point *max_points[3] = {NULL, NULL, NULL};

    // Циклическое распределение работы
    for (int i = 0; i < num_points - 2; i++) {
        for (int j = i + 1; j < num_points - 1; j++) {
            for (int k = j + 1; k < num_points; k++) {
                if ((i + j + k) % data->num_threads != data->thread_id) {
                    continue; // Эта комбинация не для данного потока
                }
                double area = triangle_area(massive->array[i], massive->array[j], massive->array[k]);
                if (area > max_area) {
                    max_area = area;
                    max_points[0] = massive->array[i];
                    max_points[1] = massive->array[j];
                    max_points[2] = massive->array[k];
                }
            }
        }
    }

    data->max_area = max_area;
    memcpy(data->max_points, max_points, sizeof(max_points));
    pthread_exit(NULL);
}
// === Потоки ====

#endif //FUNCTIONS_H
