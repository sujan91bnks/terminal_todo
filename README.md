# Terminal Todo

A lightweight command-line todo list manager built in C with SQLite.

## Build

```bash
gcc main.c -I$(brew --prefix sqlite)/include -L$(brew --prefix sqlite)/lib -lsqlite3 -o todo
```

## Usage

```bash
./todo
```

## Commands

| Command | Description |
|---------|-------------|
| `ls` | List all todo tasks |
| `add <task>` | Add a new task to the list |
| `rm <task>` | Remove a task (uses fuzzy matching) |
| `h` | Show help |
| `q` | Quit |



https://github.com/user-attachments/assets/faad75e7-2eef-48b3-afef-3467d86ed1da


## Features

- Persistent storage with SQLite
- Fuzzy matching for task removal
- Simple and fast terminal interface
