# ft_nm - list symbols from object files

## Getting Started:

`ft_nm` is a re-implementation of `nm`, GNU utility for listing ELF-file symbol tables.
It behaves almost exactly like a regular `nm`, with support for:

- `-h`, `-a`, `-g`, `-u`, `-r`, `-p` flags
- reading ELF object and executable files

For more information refer to `man 1 nm`.

## Compilation:

This project is made for 42 advanced core curriculum and follows general guidelines for C/C++ projects.

Project is completely dependent only on:
- C Standard Library.
- libft - 42network's C standard library.

For compilation there's GNU Makefile providing following rules:
- `make all` - standard rule for performing all the compilation steps
- `make re`  - rule for re-compiling whole project (including dependencies)
- `make clean` - rule for removing all the compilation artifacts
- `make fclean` - ruke for removing all the compilation results (artifacts + binaries)

To compile the project you can simply type:

```tty
$ make all
```

## Testing:

GNU Makefile provides a simple testing rule: `make test`.
It's role is to test our `ft_nm` output with standard `nm` output. If output matches one-to-one then everything works fine.
If output doesn't match then probably this project isn't implemented correctly and requires solid re-evaluation.

You can test the project against the `ft_nm` binary itself by simply typing:

```tty
$ make test
```

You can also test the project against any other binary on the system by typing:

```tty
$ make test TEST_FILE={ filename }
```

## Sources:

- [TheCoreArtist/elf-parser](https://github.com/TheCodeArtist/elf-parser/tree/master)
- [elf_format_cheatsheet.md](https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779)
- [Executable and Linkable Format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)

## 
