/* TODO:
 * 1. Store the list of symbols to be printed as a seperate pointer.
 *    We can, technically speaking, memdup the whole symbol table of .symtab section and that should work fine.
 * 2. Implement sort and reverse sort for symbols
 * 3. Find out a type of each symbol based on requirements from man
 * 4. Get rid of printf in favor of something else (could be the simplest libft or ft_printf because it handles most of the cases pretty well)
 * 4. Implement proper getopt
 * 5. Implement -a flag
 * 6. Implement -g flag
 * 7. Implement -u flag
 * 8. Implement -r flag
 * 9. Implement -p flag
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

/* SECTION: api
 * */

extern int ft_getopt(int, char **);
extern int ft_file(const char *);

extern int ft_elf32(const char *);

extern int ft_elf64(const char *);

extern int ft_elf_getMagic(const char *);
extern int ft_elf_getArch(const char *);
extern void *ft_elf_extract(const char *, const size_t, const size_t);

/* SECTION: globals
 * */

extern t_list *g_paths;
extern const char *g_prog;

/* SECTION: main
 * */

int main(int ac, char **av) {
    if (!ft_getopt(ac, av)) { return (1); }
    if (!g_paths || !ft_lstsize(g_paths)) {
        g_paths = ft_lstnew(ft_strdup("a.out"));
        if (!g_paths) {
            return (1);
        }
    }

    for (t_list *path = g_paths; path; path = path->next) {
        const char *name = path->content;

        if (!name) { return (1); }
        if (ft_lstsize(g_paths) > 1) { printf("\n%s:\n", name); }
        if (!ft_file(name)) { break; }
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
        if (**av == '-') { }

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

    /* shdr print... */
    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        Elf64_Shdr shdr = shdr_tb[i];
        if (shdr.sh_type != SHT_SYMTAB) {
            continue;
        }

        /* select current string table... */
        const char *string_tb = 0;
        if (shdr.sh_type == SHT_SYMTAB) {
            string_tb = strtab;
        }

        Elf64_Sym *sym_tb = ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset);
        if (!sym_tb) {
            exitcode = 0; goto ft_elf64_exit;
        }

        for (size_t j = 0; j < shdr.sh_size / sizeof(Elf64_Sym); j++) {
            Elf64_Sym sym = sym_tb[j];
            
            const uint8_t st_type = ELF64_ST_TYPE(sym.st_info);
            if (st_type != STT_NOTYPE &&
                st_type != STT_OBJECT &&
                st_type != STT_FUNC &&
                st_type != STT_COMMON
            ) {
                continue;
            }

            const char *st_name = string_tb + sym.st_name;
            if (!st_name || !st_name[0]) {
                continue;
            }

            /* print address... */
            if (sym.st_value) { printf("%016lx ", sym.st_value); }
            else { printf("%*c ", 16, ' '); }
            
            /* print type... */
            printf("A ");

            /* print name... */
            printf("%s\n", st_name);
        }

        free(sym_tb), sym_tb = 0;
    }

ft_elf64_exit:
    if (shstrtab) { free((void *) shstrtab), shstrtab = 0; }
    if (strtab)   { free((void *) strtab), strtab = 0; }
    if (dynstr)   { free((void *) dynstr), dynstr = 0; }
    if (shdr_tb)  { free(shdr_tb), shdr_tb = 0; }
    if (ehdr)     { free(ehdr), ehdr = 0; }
    return (exitcode);
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
