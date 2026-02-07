#include "./ft_nm.h"

static int ft_part(struct s_symbol *, const size_t, const size_t, int (*)(struct s_symbol, struct s_symbol));


extern struct s_symbol *ft_qsort(struct s_symbol *arr, const size_t low, const size_t high, int (*compare)(struct s_symbol, struct s_symbol)) {
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


static int ft_part(struct s_symbol *arr, const size_t low, const size_t high, int (*compare)(struct s_symbol, struct s_symbol)) {
    struct s_symbol pivot = arr[high];

    size_t i = low;
    for (size_t j = low; j < high; j++) {
        if (!compare(arr[j], pivot)) {
            struct s_symbol tmp0 = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp0;
            i++;
        }
    }
    struct s_symbol tmp0 = arr[i];
    arr[i] = arr[high];
    arr[high] = tmp0;
    return (i);
}
