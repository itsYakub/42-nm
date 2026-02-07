#include "./ft_nm.h"

static int ft_part(struct s_file *, const size_t, const size_t, int (*)(struct s_file, struct s_file));


extern struct s_file *ft_qsort(struct s_file *arr, const size_t low, const size_t high, int (*compare)(struct s_file, struct s_file)) {
    if (!arr)       { return (0); }
    if (!compare)   { return (0); }
    
    int part = ft_part(arr, low, high, compare);
    if ((size_t) part > low) {
        ft_qsort(arr, low, part - 1, compare);
    }
    if ((size_t) part < high) {
        ft_qsort(arr, part + 1, high, compare);
    }
    return (arr);
}


static int ft_part(struct s_file *arr, const size_t low, const size_t high, int (*compare)(struct s_file, struct s_file)) {
    struct s_file pivot = arr[high];

    size_t i = low;
    for (size_t j = low; j < high; j++) {
        if (!compare(arr[j], pivot)) {
            struct s_file tmp0 = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp0;
            i++;
        }
    }
    struct s_file tmp0 = arr[i];
    arr[i] = arr[high];
    arr[high] = tmp0;
    return (i);
}
