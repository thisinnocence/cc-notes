#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_QUE_SIZE 1024
struct queue{
    int head;
    int tail;
    int num;
    int array[MAX_QUE_SIZE];
};

void push(struct queue* q, int data)
{
    if (q->num == MAX_QUE_SIZE) {
        printf("queue full, push failed\n");
        return;
    }

    if (q->num == 0) {
        // when q->num == 0, do not move tail
        q->array[q->tail] = data;
    } else {
        q->tail = (q->tail + 1) % MAX_QUE_SIZE;
        q->array[q->tail] = data;
    }
    q->num++;
}

void pop(struct queue *q)
{
    if (q->num == 0) {
        printf("queue empty, pop failed\n");
        return;
    }
    // when q->num == 1, do not move head
    if (q->num > 1) {
        q->head = (q->head + 1) % MAX_QUE_SIZE;
    }
    q->num--;
}

int front(struct queue *q)
{
    return q->array[q->head];
}

bool empty(struct queue *q)
{
    return q->num == 0;
}

void init(struct queue *q)
{
    q->num = 0;
    q->head = 0;
    q->tail = 0;
}

void bfs(int **graph, int vertex_num, int max_dis, int start)
{
    bool visited[vertex_num];
    memset(visited, false, sizeof(visited));

    struct queue que;
    init(&que);
    push(&que, start);
    while(!empty(&que)) {
        int cur = front(&que);
        if (!visited[cur]) {
            printf("%d ", cur);
        }
        visited[cur] = true;
        for (int i = 0; i < vertex_num; i++) {
            if (graph[cur][i] != max_dis && !visited[i]) {
                push(&que, i);
            }
        }
        pop(&que);
    }
    printf("\n");
}

int main()
{
    int N = 65535;
    // see graph.png at current diectory
    int graph[9][9] = {
            {0, 1, 5, N, N, N, N, N, N},
            {1, 0, 3, 7, 5, N, N, N, N},
            {5, 3, 0, N, 1, 7, N, N, N},
            {N, 7, N, 0, 2, N, 3, N, N},
            {N, 5, 1, 2, 0, 3, 6, 9, N},
            {N, N, 7, N, 3, 0, N, 5, N},
            {N, N, N, 3, 6, N, 0, 2, 7},
            {N, N, N, N, 9, 5, 2, 0, 4},
            {N, N, N, N, N, N, 7, 4, 0}
    };

    int *gra[9];
    for (int i = 0; i < 9; i ++) {
        gra[i] = (int*)&graph[i];
    }
    bfs((int**)gra, 9, N,0); // 0 1 2 3 4 5 6 7 8
    bfs((int**)gra, 9, N,8); // 8 6 7 3 4 5 1 2 0
    return 0;
}
