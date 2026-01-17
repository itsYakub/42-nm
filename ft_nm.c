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

/* SECTION: preprocessor
 * */

#if defined (__LP64__)
# define ElfEhdr    Elf64_Ehdr
# define ElfShdr    Elf64_Shdr
# define ElfPhdr    Elf64_Phdr
# define ElfSym     Elf64_Sym
#else
# define ElfEhdr    Elf32_Ehdr
# define ElfShdr    Elf32_Shdr
# define ElfPhdr    Elf32_Phdr
# define ElfSym     Elf32_Sym
#endif /* __LP64__ */

/* SECTION: api
 * */

extern int ft_getopt(int, char **);
extern int ft_procFile(const char *);
extern int ft_procELF(const char *, const char *);

extern ElfEhdr *ft_ElfEhdrLoad(const char *, const char *);
extern ElfPhdr *ft_ElfPhdrLoad(ElfEhdr *, const char *);
extern ElfShdr *ft_ElfShdrLoad(ElfEhdr *, const char *);

extern ElfSym *ft_ElfSymLoad(const char *, const size_t, const size_t);
extern ElfSym *ft_ElfGetStrtab(ElfEhdr *, ElfShdr *, const char *);

extern int ft_ElfUnload(void *);

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
        if (!ft_procFile(name)) { break; }
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

int ft_procFile(const char *path) {
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
    if (!ft_procELF(path, buffer)) {
        free(buffer), buffer = 0;
        return (0);
    }
    
    /* cleanup... */
    free(buffer), buffer = 0;

    return (1);
}

int ft_procELF(const char *path, const char *buffer) {
    int exitcode = 1;

    /* ehdr... */
    ElfEhdr *ehdr = ft_ElfEhdrLoad(path, buffer);
    if (!ehdr) {
        exitcode = 0; goto exit0;
    }

    /* phdr table... */
    ElfPhdr *phdr_tb = ft_ElfPhdrLoad(ehdr, buffer);
    if (!phdr_tb) {
        exitcode = 0; goto exit0;
    }

    /* shdr table... */
    ElfShdr *shdr_tb = ft_ElfShdrLoad(ehdr, buffer);
    if (!shdr_tb) {
        exitcode = 0; goto exit0;
    }

    /* .strtab ... */
    ElfSym *strtab = ft_ElfGetStrtab(ehdr, shdr_tb, buffer);
    if (!strtab) {
        exitcode = 0; goto exit0;
    }

    /* phdr print... */
    for (size_t i = 0; i < ehdr->e_phnum; i++) {
        ElfPhdr phdr   = phdr_tb[i];
        printf("phdr:\n");
        printf("- type: 0x%x\n", phdr.p_type);
        
        ElfSym *sym_tb = ft_ElfSymLoad(buffer, phdr.p_filesz, phdr.p_offset);
        if (!sym_tb) {
            exitcode = 0; goto exit0;
        }

        for (size_t j = 0; j < phdr.p_filesz / sizeof(ElfSym); j++) {
            ElfSym sym = sym_tb[j];

            /* ... */
            printf("\tsymbol:\n");
            printf("\t- name:  0x%x\n", sym.st_name);
            printf("\t- value: 0x%lx\n", sym.st_value);
            printf("\t- size:  0x%lx\n", sym.st_size);
        } 
        
        ft_ElfUnload(sym_tb), sym_tb = 0;
    }

    /* shdr print... */
    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        ElfShdr shdr   = shdr_tb[i];
        printf("shdr:\n");
        printf("- type: 0x%x\n", shdr.sh_type);
        printf("- name: %u\n", shdr.sh_name);
        
        ElfSym *sym_tb = ft_ElfSymLoad(buffer, shdr.sh_size, shdr.sh_offset);
        if (!sym_tb) {
            exitcode = 0; goto exit0;
        }

        for (size_t j = 0; j < shdr.sh_size / sizeof(ElfSym); j++) {
            ElfSym sym = sym_tb[j];

            /* ... */
            printf("\tsymbol:\n");
            printf("\t- name:  0x%x\n", sym.st_name);
            printf("\t- value: 0x%lx\n", sym.st_value);
            printf("\t- size:  0x%lx\n", sym.st_size);
        }

        ft_ElfUnload(sym_tb), sym_tb = 0;
    }

exit0:
    if (strtab)  { ft_ElfUnload(strtab),  strtab = 0; }
    if (shdr_tb) { ft_ElfUnload(shdr_tb), shdr_tb = 0; }
    if (phdr_tb) { ft_ElfUnload(phdr_tb), phdr_tb = 0; }
    if (ehdr)    { ft_ElfUnload(ehdr),    ehdr = 0; }
    return (exitcode);
}

ElfEhdr *ft_ElfEhdrLoad(const char *path, const char *buffer) {
    /* safety check... */
    if (!path) { return (0); }
    if (!buffer) { return (0); }

    /* allocate ehdr... */
    ElfEhdr *ehdr = malloc(sizeof(ElfEhdr));
    if (!ehdr) { return (0); }
    if (!ft_memcpy((void *) ehdr, buffer, sizeof(ElfEhdr))) {
        free(ehdr);
        return (0);
    } 

    /* check magic... */
    if (ft_memcmp(ehdr->e_ident, ELFMAG, SELFMAG)) {
        printf("%s: '%s': Not an ELF\n", g_prog, path);
        ft_ElfUnload(ehdr);
        return (0);
    }

    return (ehdr);
}

extern ElfPhdr *ft_ElfPhdrLoad(ElfEhdr *ehdr, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); }
    if (!buffer) { return (0); }

    /* helper variables... */
    size_t phoff = ehdr->e_phoff;
    size_t phnum = ehdr->e_phnum;
    size_t phentsize = ehdr->e_phentsize;
    const char *phbuf = buffer + phoff;

    /* allocate phdr... */
    ElfPhdr *phdr = malloc(phentsize * phnum);
    if (!phdr) { return (0); }
    if (!ft_memcpy((void *) phdr, phbuf, phnum * phentsize)) {
        free(phdr), phdr = 0;
        return (0);
    }

    return (phdr);
}

ElfShdr *ft_ElfShdrLoad(ElfEhdr *ehdr, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); }
    if (!buffer) { return (0); }

    /* helper variables... */
    size_t shoff = ehdr->e_shoff;
    size_t shnum = ehdr->e_shnum;
    size_t shentsize = ehdr->e_shentsize;
    const char *shbuf = buffer + shoff;

    /* allocate shdr... */
    ElfShdr *shdr = malloc(shentsize * shnum);
    if (!shdr) { return (0); }
    if (!ft_memcpy((void *) shdr, shbuf, shnum * shentsize)) {
        free(shdr), shdr = 0;
        return (0);
    }

    return (shdr);
}

ElfSym *ft_ElfSymLoad(const char *buffer, const size_t size, const size_t offset) {
    /* safety check... */
    if (!buffer) { return (0); }

    /* allocate sym... */
    ElfSym *sym = malloc(size);
    if (!sym) { return (0); }
    if (!ft_memcpy((void *) sym, buffer + offset, size)) {
        free(sym), sym = 0;
        return (0);
    }

    return (sym);
}

ElfSym *ft_ElfGetStrtab(ElfEhdr *ehdr, ElfShdr *shdr, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); }
    if (!shdr) { return (0); }
    if (!buffer) { return (0); }

    /* get .strtab index... */
    size_t shstrndx = 0;
    if (ehdr->e_shstrndx != SHN_XINDEX) {
        shstrndx = ehdr->e_shstrndx;
    }
    else {
        /* process SH_XINDEX... */
    }

    ElfShdr section = shdr[shstrndx];
    if (section.sh_type == SHT_NULL) {
        return (0);
    }

    return (ft_ElfSymLoad(buffer, section.sh_size, section.sh_offset));
}

int ft_ElfUnload(void *data) {
    if (!data) { return (0); }
    
    free(data);
    return (1);
}


/* SECTION: globals
 * */

t_list *g_paths = 0;
const char *g_prog = 0;
