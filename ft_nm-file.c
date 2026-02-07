#include "./ft_nm.h"

static int ft_comparea(struct s_symbol, struct s_symbol);

static int ft_compared(struct s_symbol, struct s_symbol);

extern struct s_file *ft_file(const char *path) {
    /* setup... */
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        g_errno = 1;
        ft_perror(path);
        return (0);
    }

    struct stat stat = { 0 };
    if (fstat(fd, &stat) == -1) {
        return (0);
    }
    
    if (S_ISDIR(stat.st_mode)) {
        g_errno = 2;
        ft_perror(path);
        return (0);
    }

    /* read file to a buffer... */
    char *buffer = mmap(0, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!buffer) {
        close(fd), fd = 0;
        return (0);
    }
    
    /* execution... */
    struct s_file *result = 0;

    /* check if the file is ELF file... */
    if (ft_elf_getMagic(buffer)) {
        switch (ft_elf_getArch(buffer)) {
            case (ELFCLASS32): { result = ft_elf32(path, buffer); } break;
            case (ELFCLASS64): { result = ft_elf64(path, buffer); } break;
        }
    }
    else {
        /* check if file is "ar" file... */
        if (ft_ar_getMagic(buffer)) {
            result = ft_ar(path, buffer, stat.st_size);
        }
        else {
            g_errno = 3;
            ft_perror(path);
            return (0);
        }
    }
    
    /* cleanup... */
    munmap(buffer, stat.st_size), buffer = 0;
    close(fd), fd = 0;

    return (result);
}

extern struct s_symbol *ft_sort(struct s_symbol *arr, const size_t size) {
    /* safety-check... */
    if (!arr)  { return (0); }
    if (!size) { return (0); }
    
    int (*compare)(struct s_symbol, struct s_symbol);
    switch (g_opt_sort) {
        case (1): { compare = ft_comparea; } break;
        case (2): { compare = ft_compared; } break;
        default:  { return (arr); }
    }

    return (ft_qsort(arr, 0, size - 1, compare));
}

static int ft_comparea(struct s_symbol f0, struct s_symbol f1) {
    const char *n0, *name0 = n0 = f0.s_name;
    const char *n1, *name1 = n1 = f1.s_name;

    while (*n0 || *n1) {
        while (*n0 && !ft_isalnum(*n0)) { n0++; }
        while (*n1 && !ft_isalnum(*n1)) { n1++; }
        if (ft_tolower(*n0) != ft_tolower(*n1)) {
            return (ft_tolower(*n0) > ft_tolower(*n1));
        }

        if (*n0) { n0++; }
        if (*n1) { n1++; }
    }

    return (ft_strcmp(name0, name1) > 0);
}

static int ft_compared(struct s_symbol f0, struct s_symbol f1) {
    const char *n0, *name0 = n0 = f0.s_name;
    const char *n1, *name1 = n1 = f1.s_name;

    while (*n0 || *n1) {
        while (*n0 && !ft_isalnum(*n0)) { n0++; }
        while (*n1 && !ft_isalnum(*n1)) { n1++; }
        if (ft_tolower(*n0) != ft_tolower(*n1)) {
            return (ft_tolower(*n0) < ft_tolower(*n1));
        }

        if (*n0) { n0++; }
        if (*n1) { n1++; }
    }

    return (ft_strcmp(name0, name1) < 0);
}
