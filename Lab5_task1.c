#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#define N 8
#define EDGE_PROBABILITY 25

/* Генерация матрицы смежности неориентированного графа. */
void generate_graph(int a[N][N])
{
    int i, j, edge;

    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
            a[i][j] = 0;

    for (i = 0; i < N; ++i)
        for (j = i + 1; j < N; ++j) {
            edge = (rand() % 100 < EDGE_PROBABILITY);
            a[i][j] = edge;
            a[j][i] = edge;
        }
}

/* Степень вершины с индексом v. */
int degree(int a[N][N], int v)
{
    int j, result = 0;
    for (j = 0; j < N; ++j)
        result += a[v][j];
    return result;
}

/* Размер графа: число неориентированных рёбер. */
int graph_size(int a[N][N])
{
    int i, j, edges = 0;
    for (i = 0; i < N; ++i)
        for (j = i + 1; j < N; ++j)
            edges += a[i][j];
    return edges;
}

void print_vertices(int a[N][N], int required_degree)
{
    int i, found = 0;
    for (i = 0; i < N; ++i)
        if (degree(a, i) == required_degree) {
            printf(" v%d", i + 1);
            found = 1;
        }
    if (!found)
        printf(" нет");
    printf("\n");
}

int main(void)
{
    int a[N][N];
    int i, j, degree_sum = 0;

    setlocale(LC_ALL, "");
    srand(2026u); /* Фиксированное зерно для проверки. */
    generate_graph(a);

    printf("Матрица смежности графа G (%d вершин):\n", N);
    printf("     ");
    for (j = 0; j < N; ++j)
        printf(" v%d", j + 1);
    printf("\n");

    for (i = 0; i < N; ++i) {
        printf(" v%d: ", i + 1);
        for (j = 0; j < N; ++j)
            printf("  %d", a[i][j]);
        printf("\n");
    }

    printf("\nСтепени вершин:\n");
    for (i = 0; i < N; ++i) {
        int d = degree(a, i);
        printf("deg(v%d) = %d\n", i + 1, d);
        degree_sum += d;
    }

    printf("\nРазмер графа |E(G)| = %d\n", graph_size(a));
    printf("Сумма степеней = %d\n", degree_sum);
    printf("Изолированные вершины:");
    print_vertices(a, 0);
    printf("Концевые вершины:");
    print_vertices(a, 1);
    printf("Доминирующие вершины:");
    print_vertices(a, N - 1);

    return 0;
}
