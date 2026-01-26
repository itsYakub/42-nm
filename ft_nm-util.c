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

extern int ft_puthex_fd(int n, int fd) {
    const char base[] = "0123456789abcdef";
    
    if (n < 0) {
        ft_putchar_fd('-', 1);
        return (ft_puthex_fd(n * -1, fd));
    }
    
    if (n / 16 > 0) {
        ft_puthex_fd(n / 16, fd);
    }
    ft_putchar_fd(base[n % 16], 1);
    return (1);
}
