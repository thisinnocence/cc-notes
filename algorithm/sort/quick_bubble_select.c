#include <stdio.h>

// [begin, end), stable
void bubble_sort(int data[], int begin, int end)
{
    int i, j, tmp;
    for (i = begin; i < end - 1; i++) {
        for (j = begin; j < end - 1 - i; j++) {
            if (data[j] > data[j + 1]) {
                tmp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = tmp;
            }
        }
    }
}

// [begin, end), unstable
void select_sort(int data[], int begin, int end)
{
    int i, j, tmp;
    for (i = begin; i < end - 1; i++) {
        for (j = i; j < end; j++) {
            if (data[i] > data[j]) {
                tmp = data[i];
                data[i] = data[j];
                data[j] = tmp;
            }
        }
    }
}

// [begin, end), unstable
void quick_sort(int data[], int begin, int end)
{
    int i = begin, j = end - 1, tmp;
    if (begin >= end - 1)
        return;

    tmp = data[begin];
    while (i < j) {
        while (i < j && data[j] >= tmp)
            j--;
        if (i < j)
            data[i++] = data[j];
        while (i < j && data[i] < tmp)
            i++;
        if (i < j)
            data[j--] = data[i];
    }
    data[i] = tmp;

    quick_sort(data, begin, i);
    quick_sort(data, i + 1, end);
}

int main()
{
    int data[] = {1, 4, 5, 2, 3, 9, 8, 10, 12, 11, 13, 14, 0, 6, 7, 15};
    int len = sizeof(data) / sizeof(data[0]);
    int i = 0;
    // bubble_sort(data, 0, len);
    // select_sort(data, 0, len);
    quick_sort(data, 0, len);
    for (i = 0; i < len; i++)
        printf("%i ", data[i]);
    printf("\n");
    return 0;
}
