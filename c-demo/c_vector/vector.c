#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct vector {
    size_t item_size;
    unsigned int item_num;
    unsigned int item_max_num;
    void *head;
};

void ve_push_back(struct vector *ve, const void *item)
{
    if (ve->item_num == ve->item_max_num) {
        unsigned int old_len = ve->item_size * ve->item_num;
        ve->item_max_num += ve->item_max_num >> 1u;
        void *new_array = malloc(ve->item_size * ve->item_max_num);
        if (new_array == NULL) {
            printf("malloc failed");
            return;
        }
        memcpy(new_array, ve->head, old_len);
        free(ve->head);
        ve->head = new_array;
    }
    void *last = (char *) ve->head + ve->item_num * ve->item_size;
    memcpy(last , item, ve->item_size);
    ve->item_num++;
}

void ve_erase(struct vector *ve, void *item)
{
    if (item == NULL) {
        printf("erase nullptr\n");
        return;
    }

    char *last = (char *) ve->head + ve->item_size * (ve->item_num - 1);
    if (last < (char*)item) {
        printf("erase item out of range\n");
        return;
    }
    if (item < ve->head) {
        printf("erase item addr invalid, < first item\n");
        return;
    }

    ve->item_num--;
    if ((char*)item < last ) {
        memmove(item, (char *) item + ve->item_size, last - (char *) item);
    }
}

// return true if item1 equals item2
typedef bool (ve_comp_func)(const char *item1, const char *item2);

void *ve_find(struct vector *ve, const void *item, ve_comp_func comp)
{
    unsigned int i;
    for (i = 0; i < ve->item_num; i++) {
        char *cur = (char*)ve->head + i * ve->item_size;
        if (comp((char*)item, cur)) {
            return cur;
        }
    }
    return NULL;
}

void ve_init(struct vector *ve, size_t item_size)
{
    ve->item_max_num = 10;
    ve->head = malloc(ve->item_max_num * item_size);
    if (ve->head == NULL) {
        printf("malloc failed\n");
        return;
    }
    ve->item_size = item_size;
    ve->item_num = 0;
}

void ve_clear(struct vector *ve)
{
    free(ve->head);
    memset(ve, 0, sizeof(*ve));
}

//--------- test case ------------------
// an int vector
bool comp(const char *item1, const char *item2)
{
    return *(int*)item1 == *(int*)item2;
}

void show_re(struct vector *ve)
{
    unsigned int i;
    int *data = (int *) ve->head;
    for (i = 0; i < ve->item_num; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

int main()
{
    int n = 0;
    struct vector ve;
    ve_init(&ve, sizeof(int));

    while (n != -1) {
        scanf("%d", &n);
        ve_push_back(&ve, &n);
        show_re(&ve);
    }

    while (n != -2) {
        scanf("%d", &n);
        void *item = ve_find(&ve, &n, comp);
        ve_erase(&ve, item);
        show_re(&ve);
    }

    ve_clear(&ve);
    return 0;
}
