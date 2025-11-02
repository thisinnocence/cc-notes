#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void dfs(int **graph, int vertex_num, bool *visited, int max_dis, int start)
{
    if (visited[start])
        return;

    printf("%d ", start);
    visited[start] = true;
    for (int i = 0; i < vertex_num; i++) {
        if (i != start && graph[start][i] != max_dis) {
            dfs(graph, vertex_num, visited, max_dis, i);
        }
    }
}

int main()
{
    int N = 65535;
    // see graph.png at current diectory
    int graph[9][9] = {{0, 1, 5, N, N, N, N, N, N},
                       {1, 0, 3, 7, 5, N, N, N, N},
                       {5, 3, 0, N, 1, 7, N, N, N},
                       {N, 7, N, 0, 2, N, 3, N, N},
                       {N, 5, 1, 2, 0, 3, 6, 9, N},
                       {N, N, 7, N, 3, 0, N, 5, N},
                       {N, N, N, 3, 6, N, 0, 2, 7},
                       {N, N, N, N, 9, 5, 2, 0, 4},
                       {N, N, N, N, N, N, 7, 4, 0}};

    int *gra[9];
    for (int i = 0; i < 9; i++) {
        gra[i] = (int *)&graph[i];
    }
    bool visited[9];

    memset(visited, 0, sizeof(visited));
    dfs((int **)gra, 9, visited, N, 0); // 0 1 2 4 3 6 7 5 8
    printf("\n");

    memset(visited, 0, sizeof(visited));
    dfs((int **)gra, 9, visited, N, 8); // 8 6 3 1 0 2 4 5 7
    printf("\n");
    return 0;
}
