#include <stdio.h>

void Dijkstra(const void *graphRaw, int vertexNum, int maxDistance, int start,
              int *distance)
{
    const int *graph = (const int *)graphRaw;
    int visited[vertexNum];
    int i, min, nearest;
    int visitedNum = 0;

    // start --> other, all not visited
    for (i = 0; i < vertexNum; i++) {
        distance[i] = graph[start * vertexNum + i];
        visited[i] = 0;
    }

    while (visitedNum < vertexNum) {
        // find nearest vertex to start in {notVisited}
        min = maxDistance;
        nearest = -1;
        for (i = 0; i < vertexNum; i++) {
            if (!visited[i] && distance[i] < min) {
                nearest = i;
                min = distance[i];
            }
        }
        if (nearest == -1)
            return;
        visited[nearest] = 1;
        visitedNum++;

        // update distance of vertex that is not visited
        for (i = 0; i < vertexNum; i++) {
            if (!visited[i] &&
                distance[i] >
                    distance[nearest] + graph[nearest * vertexNum + i]) {
                distance[i] =
                    distance[nearest] + graph[nearest * vertexNum + i];
            }
        }
    }
}

int main()
{
    int N = 65536;
    int graph[9][9] = {{0, 1, 5, N, N, N, N, N, N}, {1, 0, 3, 7, 5, N, N, N, N},
                       {5, 3, 0, N, 1, 7, N, N, N}, {N, 7, N, 0, 2, N, 3, N, N},
                       {N, 5, 1, 2, 0, 3, 6, 9, N}, {N, N, 7, N, 3, 0, N, 5, N},
                       {N, N, N, 3, 6, N, 0, 2, 7}, {N, N, N, N, 9, 5, 2, 0, 4},
                       {N, N, N, N, N, N, 7, 4, 0}};

    int start = 0, distance[9], i;
    Dijkstra(graph, 9, N, start, distance);
    printf("vertex %d to other vertex: \n", start);
    for (i = 0; i < 9; ++i) {
        printf("%d ", distance[i]);
    }
    printf("\n");
    return 0;
}
