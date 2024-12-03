#include "functions.h"
#include "sys/time.h"

int main(const int argc, char *argv[]) {
    char *input_path;
    PointMassive *massive;
    state result;

    //Засекаем время выполнения
    struct timeval start, end;
    gettimeofday(&start, NULL);



    if (argc != 3) {
        printf("ERROR: INVALID_ARGS.\n");
        exit(INVALID_ARGS);
    }

    input_path = argv[1];
    FILE *file = fopen(input_path, "r");
    if (file == NULL) {
        printf("ERROR: INVALID_FILE.\n");
        exit(INVALID_FILE);
    }

    massive = create_PointMassive(128);

    result = add_points(file, &massive);
    if (result == MEMORY_ERROR) {
        fclose(file);
        printf("ERROR: MEMORY_ERROR.\n");
        exit(MEMORY_ERROR);
    }
    if (result == UNCORECT_TYPE_OF_NUMS) {
        fclose(file);
        printf("ERROR: UNCORECT_TYPE_OF_NUMS.\n");
        exit(UNCORECT_TYPE_OF_NUMS);
    }
    fclose(file);


    // === Потоки ====
    double arv_threads;
    is_digital(argv[2], &arv_threads);


    int num_threads = (int) arv_threads; // Число потоков

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    for (int t = 0; t < num_threads; t++) {
        thread_data[t].massive = massive;
        thread_data[t].num_threads = num_threads;
        thread_data[t].thread_id = t;

        // printf("Thread %d started\n", t + 1);
        pthread_create(&threads[t], NULL, find_max_area_thread, &thread_data[t]);
    }

    // Объединение результатов
    double max_area = 0.0;
    Point *max_points[3] = {NULL, NULL, NULL};
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
        if (thread_data[t].max_area > max_area) {
            max_area = thread_data[t].max_area;
            memcpy(max_points, thread_data[t].max_points, sizeof(max_points));
        }
    }
    // === Потоки ====


    // Окончание подсчёта времени выполнения
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
       end.tv_usec - start.tv_usec) / 1.e6;

    // Вывод результата
    printf("Max area: %.2f\n", max_area);
    printf("Points: (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)\n",
           max_points[0]->x, max_points[0]->y, max_points[0]->z,
           max_points[1]->x, max_points[1]->y, max_points[1]->z,
           max_points[2]->x, max_points[2]->y, max_points[2]->z);

    freePointMassive(massive);


    printf("Time take: %0.6f seconds\n", delta);

    return 0;
}

