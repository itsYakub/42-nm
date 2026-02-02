#include "./ft_nm.h"

extern int ft_elf_getMagic(const char *buffer) {
    /* null-check... */
    if (!buffer) { return (0); }

    if (buffer[EI_MAG0] != ELFMAG0 ||
        buffer[EI_MAG1] != ELFMAG1 ||
        buffer[EI_MAG2] != ELFMAG2 ||
        buffer[EI_MAG3] != ELFMAG3
    ) {
        return (0);
    }
    return (1);
}

extern int ft_ar_getMagic(const char *buffer) {
    /* null-check... */
    if (!buffer) { return (0); }

    return (ft_strcmp(ARMAG, buffer) == 0);
}

extern int ft_elf_getArch(const char *buffer) {
    /* null-check... */
    if (!buffer) { return (0); }

    return (buffer[EI_CLASS]);
}

extern void *ft_elf_extract(const char *buffer, const size_t size, const size_t offset) {
    /* null-check... */
    if (!buffer) { return (0); }

    /* ...alloc... */
    void *result = malloc(size);
    if (!result) {
        return (0);
    }

    /* ...copy... */
    if (!memcpy(result, buffer + offset, size)) {
        free(result);
        return (0);
    }

    /* ...return */
    return (result);
}

extern void *ft_memdup(void *src, const size_t size) {
    /* null-check... */
    if (!src) { return (0); }

    /* ...alloc... */
    void *dst = malloc(size);
    if (!dst) {
        return (0);
    }

    /* ...copy */
    return (ft_memcpy(dst, src, size));
}

extern void *ft_memjoin(void *s1, void *s2, const size_t ss1, const size_t ss2) {
    /* null-check... */
    if (!s1) { return (0); }
    if (!s2) { return (0); }

    /* ...alloc... */
    void *dst = malloc(ss1 + ss2);
    if (!dst) {
        return (0);
    }

    /* ...copy */
    uint8_t *dst1 = (uint8_t *)dst;
    dst = ft_memcpy(dst1, s1, ss1);
    dst1 += ss1;
    dst1 = ft_memcpy(dst1, s2, ss2);
    return (dst);
}

extern int ft_strcmp(const char *s0, const char *s1) {
    while (*s0 && *s1) {
        if (*s0 != *s1) { return (*s0 - *s1); }
    
        s0++;
        s1++;
    }
    return (0);
}

extern int ft_islower(int c) {
    return (c >= 'a' && c <= 'z');
}

extern size_t ft_numlen(long long n, int base) {
	int	result = 0;
	while (n != 0) {
		result++;
		n /= base;
	}

	return (result);
}

extern char *ft_utoa_hex(unsigned long long n) {
    const char base[] = "0123456789abcdef";

    /* length of the number in base16... */
    size_t n_l = ft_numlen(n, 16);

    /* output string... */
    char *output = ft_calloc(n_l + 1, sizeof(char));
    if (!output) { return (0); }

    /* append characters to array... */
    for (size_t i = 0; n; i++, n /= 16) {
        output[i] = base[n % 16];
    }

    /* reverse the array... */
    for (size_t i = 0; i < n_l / 2; i++) {
        char tmp = output[i];
        output[i] = output[n_l - 1 - i];
        output[n_l - 1 - i] = tmp;
    }

    return (output);
}

/* NOTE:
 *  dst pointer is going to be freed during the execution...
 * */
extern char *ft_strjoin_free(char *dst, const char *src) {
    char *tmp = ft_strjoin(dst, src);
    if (!tmp) {
        return (0);
    }

    free(dst), dst = tmp;
    return (tmp);
}

extern char *ft_perror(const char *path) {
    if (!path) { return (0); }

    char *output = 0;
    switch (g_errno) {
        case (0): {
            output = ft_strjoin(g_prog, ": ");
            output = ft_strjoin_free(output, path);
            output = ft_strjoin_free(output, ": success\n");
        } break;
        
        case (1): {
            output = ft_strjoin(g_prog, ": ");
            output = ft_strjoin_free(output, path);
            output = ft_strjoin_free(output, ": No such file\n");
        } break;
        
        case (2): {
            output = ft_strjoin(g_prog, ": '");
            output = ft_strjoin_free(output, path);
            output = ft_strjoin_free(output, "' is a directory\n");
        } break;
        
        case (3): {
            output = ft_strjoin(g_prog, ": ");
            output = ft_strjoin_free(output, path);
            output = ft_strjoin_free(output, ": file format not recognized\n");
        } break;
        
        case (4): {
            output = ft_strjoin(g_prog, ": ");
            output = ft_strjoin_free(output, path);
            output = ft_strjoin_free(output, ": no symbols\n");
        } break;
    }
    g_errno = 0;
    return (output);
}
