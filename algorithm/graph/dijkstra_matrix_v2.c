#include <stdio.h>
#include <stdlib.h>

struct GraphMatrix {
    int *graph; // [vertexNum][vertexNum]
    int vertexNum;
    int maxDistance; // max distance means unreachable
};

void InitGraph(struct GraphMatrix *graphMatrix, int vertexNum, int maxDistance)
{
    graphMatrix->vertexNum = vertexNum;
    graphMatrix->maxDistance = maxDistance;
    graphMatrix->graph = (int *)malloc(sizeof(int) * vertexNum * vertexNum);
    if (graphMatrix->graph == NULL) {
        printf("malloc error\n");
        return;
    }
    for (int i = 0; i < vertexNum; i++) {
        for (int j = 0; j < vertexNum; j++) {
            if (i == j) {
                graphMatrix->graph[i * vertexNum + j] = 0;
            } else {
                graphMatrix->graph[i * vertexNum + j] = maxDistance;
            }
        }
    }
}

void SetEdge(struct GraphMatrix *graphMatrix, int src, int dst, int len)
{
    graphMatrix->graph[src * graphMatrix->vertexNum + dst] = len;
    graphMatrix->graph[dst * graphMatrix->vertexNum + src] = len;
}

void FreeGraph(struct GraphMatrix *graphMatrix)
{
    free(graphMatrix->graph);
}

void Dijkstra(const struct GraphMatrix *graphMatrix, int start, int *distance)
{
    const int *graph = graphMatrix->graph;
    int vertexNum = graphMatrix->vertexNum;
    int maxDistance = graphMatrix->maxDistance;
    int visited[vertexNum];

    // start --> other, all not visited
    for (int i = 0; i < vertexNum; i++) {
        distance[i] = graph[start * vertexNum + i];
        visited[i] = 0;
    }

    int visitedNum = 0;
    while (visitedNum < vertexNum) {
        // find nearest vertex to start in {notVisited}
        int min = maxDistance;
        int nearest = -1;
        for (int i = 0; i < vertexNum; i++) {
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
        for (int i = 0; i < vertexNum; i++) {
            if (!visited[i] &&
                distance[i] >
                    distance[nearest] + graph[nearest * vertexNum + i]) {
                distance[i] =
                    distance[nearest] + graph[nearest * vertexNum + i];
            }
        }
    }
}

int GetShortDistance(const struct GraphMatrix *graph, int src, int dst)
{
    int distances[graph->vertexNum];
    Dijkstra(graph, src, distances);
    return distances[dst];
}

int main()
{
    int N = 65536;
    int graph[9][9] = {{0, 1, 5, N, N, N, N, N, N}, {1, 0, 3, 7, 5, N, N, N, N},
                       {5, 3, 0, N, 1, 7, N, N, N}, {N, 7, N, 0, 2, N, 3, N, N},
                       {N, 5, 1, 2, 0, 3, 6, 9, N}, {N, N, 7, N, 3, 0, N, 5, N},
                       {N, N, N, 3, 6, N, 0, 2, 7}, {N, N, N, N, 9, 5, 2, 0, 4},
                       {N, N, N, N, N, N, 7, 4, 0}};

    // init map
    struct GraphMatrix graphMatrix;
    InitGraph(&graphMatrix, 9, N);

    // set edge
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 9; j++) {
            if (graph[i][j] != N) {
                SetEdge(&graphMatrix, i, j, graph[i][j]);
            }
        }
    }

    // calculate shortest distance
    int start = 0, distance[9];
    Dijkstra(&graphMatrix, start, distance);

    // print path
    printf("vertex %d to other vertex: \n", start);
    for (int i = 0; i < 9; ++i) {
        printf("%d ", distance[i]);
    } // 0 1 4 7 5 8 10 12 16
    printf("\n");

    int dis = GetShortDistance(&graphMatrix, 0, 3);
    printf("0-->3 distance %d\n", dis);

    // free map
    FreeGraph(&graphMatrix);
    return 0;
}
