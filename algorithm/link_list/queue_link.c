#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

struct data {
    item_t val;
    struct data *next;
};

struct queue {
    struct data *head;
    struct data *tail;
    int num;
};

void init(struct queue *q)
{
    q->head = NULL;
    q->tail = NULL;
    q->num = 0;
}

item_t *front(struct queue *q)
{
    if (q->head) {
        return &q->head->val;
    } else {
        return NULL;
    }
}

void push(struct queue *q, const item_t *it)
{
    struct data *dt = malloc(sizeof(struct data));
    dt->val = *it;
    dt->next = NULL;
    if (q->head == NULL) {
        q->head = dt;
    } else {
        q->tail->next = dt;
    }
    q->tail = dt;
    q->num++;
}

void pop(struct queue *q)
{
    if (q->head == NULL)
        return;

    if (q->head == q->tail) {
        q->head = NULL;
        q->tail = NULL;
    }

    struct data *tmp = q->head;
    q->head = q->head->next;
    free(tmp);
    q->num--;
}

void show(struct queue *q)
{
    struct data *head = q->head;
    while (head) {
        printf("%d ", head->val);
        head = head->next;
    }
    printf("\n");
}

int main()
{
    struct queue q;
    init(&q);
    int val[] = {1, 2, 3};
    push(&q, &val[0]);
    push(&q, &val[1]);
    push(&q, &val[2]);
    show(&q);
    pop(&q);
    show(&q);
    push(&q, &val[0]);
    show(&q);
    printf("%d\n", *front(&q));
    return 0;
}
