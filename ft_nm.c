/* TODO:
 * 1. [X] Store the list of symbols to be printed as a seperate pointer.
 *    We can, technically speaking, memdup the whole symbol table of .symtab section and that should work fine.
 * 2. [X] Implement sort and reverse sort for symbols
 * 3. [X] Find out a type of each symbol based on requirements from man
 * 4. [ ] Get rid of printf in favor of something else (could be the simplest libft or ft_printf because it handles most of the cases pretty well)
 * 4. [ ] Implement proper getopt
 * 5. [ ] Implement -a flag
 * 6. [ ] Implement -g flag
 * 7. [ ] Implement -u flag
 * 8. [X] Implement -r flag
 * 9. [X] Implement -p flag
 * 10. [ ] Implement -h
 * 11. [ ] Invalid flag should default to 'h'
 * 12. [ ] Check if everything works on other compilers
 * */

#include <stdio.h>      /* perror() */
#include <stdlib.h>     /* malloc(), free(), exit()  */

#include <errno.h>
#include <string.h>     /* strerror() */

#include <elf.h>
#include <fcntl.h>      /* open(), close() */
#include <unistd.h>     /* write(), getpagesize() */
#include <sys/stat.h>   /* fstat() */
#include <sys/mman.h>   /* mmap(), munmap() */


#include "libft/libft.h"

/* SECTION: globals
 * */

extern t_list *g_paths;
extern const char *g_prog;

extern int g_opt_debug;
extern int g_opt_extern;
extern int g_opt_undef;
extern int g_opt_sort;

/* SECTION: api
 * */

extern int ft_getopt(int, char **);
extern int ft_file(const char *);

extern int ft_elf32(const char *);

extern int ft_elf64(const char *);
extern int ft_elf64_getLetterCode(Elf64_Shdr *, Elf64_Sym);
extern Elf64_Sym *ft_elf64_sort(Elf64_Sym *, const size_t, const char *, int (*)(Elf64_Sym, Elf64_Sym, const char *));

extern int ft_elf_getMagic(const char *);
extern int ft_elf_getArch(const char *);
extern void *ft_elf_extract(const char *, const size_t, const size_t);

/* SECTION: libft
 * */

static void *ft_memdup(void *src, const size_t size) {
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

static void *ft_memjoin(void *s1, void *s2, const size_t ss1, const size_t ss2) {
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

/* SECTION: helper
 * */

static int ft_elf64_comparea(Elf64_Sym sym0, Elf64_Sym sym1, const char *strtab) {
    const char *name0 = strtab + sym0.st_name;
    const char *name1 = strtab + sym1.st_name;

    while (*name0 && *name1) {
        while (!ft_isalnum(*name0)) { name0++; }
        while (!ft_isalnum(*name1)) { name1++; }
        if (ft_tolower(*name0) != ft_tolower(*name1)) { break; }
        name0++;
        name1++;
    }

    if (!*name0 && !*name1) {
        return (1);
    }
    return (ft_tolower(*name0) > ft_tolower(*name1));
}

static int ft_elf64_compared(Elf64_Sym sym0, Elf64_Sym sym1, const char *strtab) {
    const char *name0 = strtab + sym0.st_name;
    const char *name1 = strtab + sym1.st_name;

    while (*name0 && *name1) {
        while (!ft_isalnum(*name0)) { name0++; }
        while (!ft_isalnum(*name1)) { name1++; }
        if (ft_tolower(*name0) != ft_tolower(*name1)) { break; }
        name0++;
        name1++;
    }

    if (!*name0 && !*name1) {
        return (1);
    }
    return (ft_tolower(*name0) < ft_tolower(*name1));
}

/* SECTION: main
 * */

int main(int ac, char **av) {
    if (!ft_getopt(ac, av)) {
        if (g_paths) {
            ft_lstclear(&g_paths, free), g_paths = 0;
        }
        return (1);
    }
    
    if (!g_paths || !ft_lstsize(g_paths)) {
        g_paths = ft_lstnew(ft_strdup("a.out"));
        if (!g_paths) {
            return (1);
        }
    }

    for (t_list *path = g_paths; path; path = path->next) {
        const char *name = path->content;
        if (!name) {
            ft_lstclear(&g_paths, free), g_paths = 0;
            return (1);
        }

        if (ft_lstsize(g_paths) > 1) { printf("\n%s:\n", name); }

        if (!ft_file(name)) {
            ft_lstclear(&g_paths, free), g_paths = 0;
            return (1);
        }
    }

    ft_lstclear(&g_paths, free), g_paths = 0;
    return (0);
}

/* SECTION: api
 * */

int ft_getopt(int ac, char **av) {
    (void) ac;
    g_prog = (const char *) *av;
    if (!g_prog) { return (0); }
    while (*++av) {
        /* process options... */
        if (**av == '-') {
            const char *opt = *av + 1;

            /* long-option... */
            if (*opt == '-') {
                opt++;

                if (!ft_strncmp(opt, "undefined-only", 15)) { g_opt_undef = 1; }
                else if (!ft_strncmp(opt, "debug-syms", 10)) {
                    if (!g_opt_undef) {
                        g_opt_debug = 1;
                    }   
                }
                else if (!ft_strncmp(opt, "extern-only", 11)) {
                    if (!g_opt_undef) {
                        g_opt_extern = 1;
                    }   
                }

                else if (!ft_strncmp(opt, "no-sort", 8)) { g_opt_sort = 0; }
                else if (!ft_strncmp(opt, "reverse-sort", 12)) {
                    if (g_opt_sort != 0) {
                        g_opt_sort = 2;
                    }
                }

                else {
                    printf("%s: unknown command: %s\n", g_prog, *av);
                    return (0);
                }
            }
            /* short-option... */
            else {
                do {
                    switch (*opt) {
                        case ('u'): { g_opt_undef = 1; } break;
                        case ('a'): {
                            if (!g_opt_undef) {
                                g_opt_debug = 1;
                            }   
                        } break;
                        case ('g'): {
                            if (!g_opt_undef) {
                                g_opt_extern = 1;
                            }   
                        } break;

                        case ('p'): { g_opt_sort = 0; } break;
                        case ('r'): {
                            if (g_opt_sort != 0) {
                                g_opt_sort = 2;
                            }            
                        } break;

                        default: {
                            printf("%s: unknown command: %s\n", g_prog, *av);
                            return (0);
                        }
                    }
                } while (*++opt);
            }
        }

        /* process files... */
        else {
            t_list  *l;

            l = ft_lstnew(ft_strdup(*av));
            if (!l) { return (0); }

            ft_lstadd_back(&g_paths, l);
        }
    }
    return (1);
}

int ft_file(const char *path) {
    /* setup... */
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("%s: '%s': No such file\n", g_prog, path);
        return (0);
    }

    struct stat stat = { 0 };
    if (fstat(fd, &stat) == -1) {
        perror(g_prog);
        return (0);
    }
    
    if (S_ISDIR(stat.st_mode)) {
        printf("%s: Warning: '%s' is a directory\n", g_prog, path);
        return (0);
    }

    /* read file to a buffer... */
    char *buffer = ft_calloc(stat.st_size + 1, sizeof(char));
    if (!buffer) {
        close(fd), fd = 0;
        return (0);
    }

    if (read(fd, buffer, stat.st_size) != stat.st_size) {
        free(buffer), buffer = 0;
        close(fd), fd = 0;
        return (0);
    }
    
    close(fd), fd = 0;
    
    /* execution... */
    if (ft_elf_getMagic(buffer)) {
        switch (ft_elf_getArch(buffer)) {
            case (ELFCLASS32): { ft_elf32(buffer); } break;
            case (ELFCLASS64): { ft_elf64(buffer); } break;
        }
    }
    else {
        printf("%s: %s: file format not recognized\n", g_prog, path);
    }
    
    /* cleanup... */
    free(buffer), buffer = 0;

    return (1);
}

extern int ft_elf32(const char *buffer) {
    (void) buffer;
    return (0);
}

extern int ft_elf64(const char *buffer) {
    int exitcode = 1;

    /* ehdr... */
    Elf64_Ehdr *ehdr = ft_elf_extract(buffer, sizeof(Elf64_Ehdr), 0);
    if (!ehdr) {
        exitcode = 0; goto ft_elf64_exit;
    }

    /* shdr table... */
    Elf64_Shdr *shdr_tb = ft_elf_extract(buffer, ehdr->e_shnum * ehdr->e_shentsize, ehdr->e_shoff);
    if (!shdr_tb) {
        exitcode = 0; goto ft_elf64_exit;
    }

    /* extract STRTAB's... */
    const char *shstrtab = ft_elf_extract(buffer, shdr_tb[ehdr->e_shstrndx].sh_size, shdr_tb[ehdr->e_shstrndx].sh_offset);
    const char *dynstr = 0;
    const char *strtab = 0;
    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        /* section header object... */
        Elf64_Shdr shdr = shdr_tb[i];

        if (!strcmp(shstrtab + shdr.sh_name, ".dynstr")) {
            dynstr = ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset);
            if (!dynstr) {
                exitcode = 0; goto ft_elf64_exit;
            }
        }

        else if (!strcmp(shstrtab + shdr.sh_name, ".strtab")) {
            strtab = ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset);
            if (!strtab) {
                exitcode = 0; goto ft_elf64_exit;
            }
        }
    }

    /* extract symbol table... */
    Elf64_Sym *sym_tb = 0;
    size_t sym_tb_s = 0;
    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        Elf64_Shdr shdr = shdr_tb[i];
        if (shdr.sh_type != SHT_SYMTAB) {
            continue;
        }

        Elf64_Sym *tmp = ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset);
        if (!tmp) {
            exitcode = 0; goto ft_elf64_exit;
        }

        if (!sym_tb) {
            sym_tb = ft_memdup(tmp, shdr.sh_size);
            if (!sym_tb) {
                free(tmp), tmp = 0;
                exitcode = 0; goto ft_elf64_exit;
            }
            sym_tb_s = shdr.sh_size / sizeof(Elf64_Sym);
        }
        else {
            void *tmp1 = sym_tb;
            sym_tb = ft_memjoin(sym_tb, tmp, shdr.sh_size, sym_tb_s);
            if (!sym_tb) {
                free(tmp), tmp = 0;
                exitcode = 0; goto ft_elf64_exit;
            }
            sym_tb_s += shdr.sh_size / sizeof(Elf64_Sym);
            free(tmp1), tmp1 = 0;
        }

        free(tmp), tmp = 0;
    }

    if (sym_tb_s == 0) {
        printf("%s: no symbols\n", g_prog);
        goto ft_elf64_exit;
    }

    switch (g_opt_sort) {
        case (1): { sym_tb = ft_elf64_sort(sym_tb, sym_tb_s, strtab, ft_elf64_comparea); } break;
        case (2): { sym_tb = ft_elf64_sort(sym_tb, sym_tb_s, strtab, ft_elf64_compared); } break;
        default:  { /* ...don't sort... */ } break;
    }

    /* print symbol table...
     * */
    for (size_t i = 0; i < sym_tb_s; i++) {
        Elf64_Sym sym = sym_tb[i];
        const uint8_t st_type = ELF64_ST_TYPE(sym.st_info);
        if (st_type != STT_NOTYPE &&
            st_type != STT_OBJECT &&
            st_type != STT_FUNC &&
            st_type != STT_COMMON
        ) {
            continue;
        }

        const char *st_name = strtab + sym.st_name;
        if (!st_name || !st_name[0]) {
            continue;
        }

        if (g_opt_undef) {
            if (sym.st_shndx != SHN_UNDEF) {
                continue;
            }
        }

        /* print address... */
        if (sym.st_value) {
            printf("%016lx ", sym.st_value);
        }
        else {
            printf("%*c ", 16, ' ');
        }

        /* print type... */
        const char letter_code = ft_elf64_getLetterCode(shdr_tb, sym);
        printf("%c ", letter_code);

        /* print name... */
        printf("%s\n", strtab + sym.st_name);
    }

ft_elf64_exit:
    if (shstrtab) { free((void *) shstrtab), shstrtab = 0; }
    if (strtab)   { free((void *) strtab), strtab = 0; }
    if (dynstr)   { free((void *) dynstr), dynstr = 0; }
    if (shdr_tb)  { free(shdr_tb), shdr_tb = 0; }
    if (sym_tb)   { free(sym_tb), sym_tb = 0; }
    if (ehdr)     { free(ehdr), ehdr = 0; }
    return (exitcode);
}

int ft_elf64_getLetterCode(Elf64_Shdr *shdr_tb, Elf64_Sym sym) {
    /* null-check... */
    if (!shdr_tb) { return (0); }
    
    /* Weak symbols... */
    const uint16_t st_shndx = sym.st_shndx;
    const uint8_t st_type   = ELF64_ST_TYPE(sym.st_info);
    const uint8_t st_bind   = ELF64_ST_BIND(sym.st_info);
    switch (st_bind) {
        case (STB_GNU_UNIQUE): { return ('u'); }
        case (STB_WEAK): {
            if (st_shndx == SHN_UNDEF) {
                if (st_type == STT_OBJECT) { return ('v'); }
                else {
                    return ('w');
                }
            }
            else {
                if (st_type == STT_OBJECT) { return ('V'); }
                else {
                    return ('W');
                }
            }
        }
    }

    /* special indices... */
    switch (st_shndx) {
        case (SHN_UNDEF): {
            return (st_bind == STB_LOCAL ? 'u' : 'U');
        }
        case (SHN_ABS): {
            return (st_bind == STB_LOCAL ? 'a' : 'A');
        }
        case (SHN_COMMON): {
            return (st_bind == STB_LOCAL ? 'c' : 'C');
        }
    }
    
    /* result */
    int c = 0;

    /* Section type/flags... */
    Elf64_Shdr shdr = shdr_tb[sym.st_shndx];
    
    const uint32_t sh_type = shdr.sh_type;
    switch (sh_type) {
        case (SHT_NOBITS): {
            if (shdr.sh_flags == (SHF_ALLOC | SHF_WRITE)) {
                c = 'B';
            }
        } break;

        case (SHT_PROGBITS): {
            switch (shdr.sh_flags) {
                case (SHF_ALLOC):
                case (SHF_ALLOC | SHF_MERGE):
                case (SHF_ALLOC | SHF_MERGE | SHF_STRINGS): { c = 'R'; } break;

                case (SHF_ALLOC | SHF_WRITE):
                case (SHF_ALLOC | SHF_WRITE | SHF_TLS): { c = 'D'; } break;

                case (SHF_ALLOC | SHF_EXECINSTR):
                case (SHF_ALLOC | SHF_EXECINSTR | SHF_GROUP):
                case (SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE): { c = 'T'; } break;
            }
        } break;

        default: { c = 'D'; } break;
    }

    /* check if symbol is global / local... */
    if (st_bind == STB_LOCAL) {
        c = ft_tolower(c);
    }

    return (c);
}

Elf64_Sym *ft_elf64_sort(Elf64_Sym *sym_tb, const size_t size, const char *strtab, int (*compare)(Elf64_Sym, Elf64_Sym, const char *)) {
    /* safety-check... */
    if (!sym_tb) { return (0); }
    if (!strtab) { return (0); }
    if (!size)   { return (0); }

    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - 1 - i; j++) {
            if (compare(sym_tb[j], sym_tb[j + 1], strtab)) {
                Elf64_Sym tmp = sym_tb[j];
                sym_tb[j] = sym_tb[j + 1];
                sym_tb[j + 1] = tmp;
            }
        }
    }
    return (sym_tb);
}

int ft_elf_getMagic(const char *buffer) {
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

int ft_elf_getArch(const char *buffer) {
    /* null-check... */
    if (!buffer) { return (0); }

    return (buffer[EI_CLASS]);
}

void *ft_elf_extract(const char *buffer, const size_t size, const size_t offset) {
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


/* SECTION: globals
 * */

t_list *g_paths = 0;
const char *g_prog = 0;

/* g_opt_debug - show debug symbols
 * > 0 - disabled
 * > 1 - enabled
 * */
int g_opt_debug = 0;

/* g_opt_extern - show extern symbols
 * > 0 - disabled
 * > 1 - enabled
 * */
int g_opt_extern = 0;

/* g_opt_undef - show undefined symbols
 * > 0 - disabled
 * > 1 - enabled
 * */
int g_opt_undef = 0;

/* g_opt_sort - select sorting method
 * > 0 - none
 * > 1 - ascending (a - z)
 * > 2 - descending (z - a)
 * */
int g_opt_sort = 1;
