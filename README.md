
# Project: Building a Simple Unix Shell (`dash`)

## Introduction

This project involves creating a simple Unix shell, named `dash` (short for DAllas SHell), as a group project. The primary objectives are to deepen understanding of the Unix/Linux programming environment, process management, and shell functionality.

## Project Specifications

### Overview

- Implement a command-line interpreter (CLI) or shell.
- The shell operates in an interactive loop, processing user commands and executing them.

### Basic Shell: `dash`

- **Executable Name:** `dash`
- **Operation Modes:**
  - **Interactive Mode:** Prints a prompt `dash>` and accepts commands.
  - **Batch Mode:** Reads commands from a batch file (e.g., `./dash batch.txt`).

### Structure and Features

1. **Input Handling:** 
   - Use `getline()` for input reading.
   - Support for both interactive and batch modes.

2. **Command Parsing and Execution:**
   - Parse input using `strtok()` or `strtok_r()`.
   - Use `fork()`, `execv()`, and `wait()/waitpid()` for command execution.
   - Handle the shell path for command location.

3. **Built-in Commands:**
   - Implement `exit`, `cd`, and `path` as built-in commands.

4. **Redirection:**
   - Support output redirection to a file using the `>` operator.

5. **Parallel Commands:**
   - Execute multiple commands in parallel using the `&` operator.

### Program Errors

- Use a standard error message for all errors.
- Continue processing after most errors, except for critical ones.

### Miscellaneous Hints

- Develop incrementally, testing each feature thoroughly.
- Handle variable white spaces in inputs.
- Keep versions of your code for safe development.
- Use source control systems like git.

### Testing

- **Compilation:** `gcc dash.c -o dash -Wall -Werror -O`
- **Test Cases:** Available at `/cs5348-xv6/src/testcases/P1` on `csjaws.utdallas.edu`.
- **Process Limit:** Set `ulimit -u 90` in `.bashrc` to prevent fork bombs.
