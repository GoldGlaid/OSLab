#include "functions.h"

int main(const int argc, char *argv[]) {
    char *input_path;
    PointMassive *massive;
    state result;

    if (argc != 2) {
        printf("ERROR: INVALID_ARGS.\n");
        exit(INVALID_ARGS);
    }

    input_path = argv[1];
    FILE *file = fopen(input_path, "r");
    if (file == NULL) {
        printf("ERROR: INVALID_FILE.\n");
        exit(INVALID_FILE);
    }

    massive = create_PointMassive(32);

    result = add_points(file, &massive);
    if (result == MEMORY_ERROR) {
        printf("ERROR: MEMORY_ERROR.\n");
        exit(MEMORY_ERROR);
    }
    if (result == UNCORECT_TYPE_OF_NUMS) {
        printf("ERROR: UNCORECT_TYPE_OF_NUMS.\n");
        exit(UNCORECT_TYPE_OF_NUMS);
    }

    double max_area = 0.0;
    Point *max_points[3];


    for (int i = 0; i < massive->size; i++) {
        for (int j = i + 1; j < massive->size; j++) {
            for (int k = j + 1; k <massive->size; k++) {
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

    // Результат
    printf("Max array: %.2f\n", max_area);
    printf("Points: (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)\n",
           max_points[0]->x, max_points[0]->y, max_points[0]->z,
           max_points[1]->x, max_points[1]->y, max_points[1]->z,
           max_points[2]->x, max_points[2]->y, max_points[2]->z);


    freePointMassive(massive);
    return 0;
}




