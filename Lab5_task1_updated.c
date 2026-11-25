#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

/* Ввод целого числа из заданного диапазона. */
int read_int(const char *prompt, int min, int max, int *value)
{
    char line[128], extra;
    int x;
    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL)
            return 0;
        if (sscanf(line, "%d %c", &x, &extra) == 1 && x >= min && x <= max) {
            *value = x;
            return 1;
        }
        printf("Введите целое число от %d до %d.\n", min, max);
    }
}

/* Динамическое выделение памяти под матрицу n x n. */
int **create_matrix(int n)
{
    int i, j;
    int **a = malloc(n * sizeof(*a));
    if (a == NULL)
        return NULL;

    for (i = 0; i < n; ++i) {
        a[i] = calloc(n, sizeof(*a[i]));
        if (a[i] == NULL) {
            for (j = 0; j < i; ++j)
                free(a[j]);
            free(a);
            return NULL;
        }
    }
    return a;
}

/* Освобождение памяти. */
void free_matrix(int **a, int n)
{
    int i;
    for (i = 0; i < n; ++i)
        free(a[i]);
    free(a);
}

/* Генерация неориентированного графа с возможными петлями. */
void generate_graph(int **a, int n, int edge_probability, int loop_probability)
{
    int i, j, edge;
    for (i = 0; i < n; ++i) {
        /* Единица на диагонали означает одну петлю. */
        a[i][i] = (rand() % 100 < loop_probability);
        for (j = i + 1; j < n; ++j) {
            edge = (rand() % 100 < edge_probability);
            a[i][j] = edge;
            a[j][i] = edge;
        }
    }
}

/* Каждая петля увеличивает степень вершины на два. */
int degree(int **a, int n, int v)
{
    int j, result = a[v][v];
    for (j = 0; j < n; ++j)
        result += a[v][j];
    return result;
}

/* Размер графа: обычные рёбра и петли, каждое по одному разу. */
int graph_size(int **a, int n)
{
    int i, j, edges = 0;
    for (i = 0; i < n; ++i)
        for (j = i; j < n; ++j)
            edges += a[i][j];
    return edges;
}

/* Проверяем связь со всеми ДРУГИМИ вершинами. */
int is_dominating(int **a, int n, int v)
{
    int j;
    for (j = 0; j < n; ++j)
        if (j != v && a[v][j] == 0)
            return 0;
    return 1;
}

void print_vertices(int **a, int n, int required_degree)
{
    int i, found = 0;
    for (i = 0; i < n; ++i) {
        if (degree(a, n, i) == required_degree) {
            printf(" v%d", i + 1);
            found = 1;
        }
    }
    if (!found)
        printf(" нет");
    printf("\n");
}

void print_dominating(int **a, int n)
{
    int i, found = 0;
    for (i = 0; i < n; ++i) {
        if (is_dominating(a, n, i)) {
            printf(" v%d", i + 1);
            found = 1;
        }
    }
    if (!found)
        printf(" нет");
    printf("\n");
}

/* После каждого изменения графа характеристики считаются заново. */
void print_graph(int **a, int n)
{
    int i, j, d, degree_sum = 0, loops = 0;
    char label[16];
    printf("\nМатрица смежности графа G (%d вершин):\n", n);
    printf("%5s", "");
    for (j = 0; j < n; ++j) {
        snprintf(label, sizeof(label), "v%d", j + 1);
        printf("%5s", label);
    }
    printf("\n");

    for (i = 0; i < n; ++i) {
        snprintf(label, sizeof(label), "v%d:", i + 1);
        printf("%5s", label);
        for (j = 0; j < n; ++j)
            printf("%5d", a[i][j]);
        printf("\n");
    }

    printf("\nСтепени вершин:\n");
    for (i = 0; i < n; ++i) {
        d = degree(a, n, i);
        printf("deg(v%d) = %d\n", i + 1, d);
        degree_sum += d;
        loops += a[i][i];
    }

    printf("\nРазмер графа |E(G)| = %d\n", graph_size(a, n));
    printf("Количество петель = %d\n", loops);
    printf("Сумма степеней = %d\n", degree_sum);
    printf("Изолированные вершины:");
    print_vertices(a, n, 0);
    printf("Концевые вершины:");
    print_vertices(a, n, 1);
    printf("Доминирующие вершины:");
    print_dominating(a, n);
}

int main(void)
{
    int **a;
    int n, edge_probability, loop_probability, choice, u, v;
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    srand((unsigned)time(NULL));

    for (;;) {
        if (!read_int("Введите количество вершин (1-100): ", 1, 100, &n) ||
            !read_int("Вероятность появления ребра (0-100 %): ", 0, 100, &edge_probability) ||
            !read_int("Вероятность появления петли (0-100 %): ", 0, 100, &loop_probability))
            return 0;

        a = create_matrix(n);
        if (a == NULL) {
            fprintf(stderr, "Ошибка выделения памяти.\n");
            return 1;
        }
        generate_graph(a, n, edge_probability, loop_probability);

        for (;;) {
            print_graph(a, n);
            printf("\n1 - Сгенерировать новый граф с этими параметрами\n");
            printf("2 - Изменить количество вершин и вероятности\n");
            printf("3 - Добавить или удалить ребро (или петлю)\n");
            printf("0 - Выход\n");
            if (!read_int("Выберите действие: ", 0, 3, &choice))
                choice = 0;

            if (choice == 0 || choice == 2)
                break;
            if (choice == 1) {
                generate_graph(a, n, edge_probability, loop_probability);
            } else {
                if (!read_int("Номер первой вершины: ", 1, n, &u) ||
                    !read_int("Номер второй вершины: ", 1, n, &v)) {
                    choice = 0;
                    break;
                }
                --u;
                --v;
                a[u][v] = 1 - a[u][v];
                a[v][u] = a[u][v];
            }
        }
        free_matrix(a, n);
        if (choice == 0)
            break;
    }
    return 0;
}
