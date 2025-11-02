#include <stdio.h>
#include <stdlib.h>

typedef int hash_key_t;

typedef struct data_s {
    hash_key_t key;
    char name[100];
    struct data_s *next;
} data_t;

#define TBL_SIZE 7
data_t *g_hash_tbl[TBL_SIZE];

int hash(int n)
{
    return n % TBL_SIZE;
}

data_t *find(hash_key_t key)
{
    int i = hash(key);
    data_t *p = g_hash_tbl[i];
    while (p) {
        if (p->key == key)
            return p;
        else
            p = p->next;
    }
    return NULL;
}

int insert(data_t *data)
{
    if (find(data->key))
        return -1;
    int i = hash(data->key);
    data->next = g_hash_tbl[i];
    g_hash_tbl[i] = data;
    return 0;
}

int delete(hash_key_t key)
{
    int i = hash(key);
    data_t **head = &g_hash_tbl[i];
    while (*head) {
        data_t *curr = *head;
        if (curr->key == key) {
            *head = curr->next;
            free(curr);
            return 0;
        } else {
            head = &curr->next;
        }
    }
    return -1;
}

data_t *new_data(int n)
{
    data_t *d = malloc(sizeof(data_t));
    if (d == NULL)
        return NULL;
    d->next = NULL;
    d->key = n;
    sprintf(d->name, "Number(%d)", n);
    return d;
}

int main()
{
    insert(new_data(7));
    printf("%s\n", find(7)->name);
    insert(new_data(8));
    printf("%s\n", find(8)->name);
    insert(new_data(9));
    printf("%s\n", find(9)->name);
    insert(new_data(14));
    printf("%s\n", find(14)->name);
    insert(new_data(21));
    printf("%s\n", find(21)->name);
    insert(new_data(28));
    printf("%s\n", find(28)->name);
    delete (14);
    printf("delete 14, find 14 ret %p\n", find(14));
    return 0;
}
