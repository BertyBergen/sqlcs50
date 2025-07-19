# CSDB — C SQL Database (educational)

```
 _   _      _ _          ____  _____ _____    ____
| | | | ___| | | ___    / ___|/ ____|  ___|  / __ \
| |_| |/ _ \ | |/ _ \  | |    | (__ |  |__  | |  | |
|     | |_|| | | | | | | |    \___ \|___  \ | |  | |
|  _  |  __/ | | |_| | | |___ ____) |___)  /| |__| |
|_| |_|\___|_|_|\___/   \____|_____/______/  \____/

```

## Overview

sqlcs50 is a minimal SQL database written in C from scratch.
It replaces key SQL mechanisms: paginig, B-tree, serialization, and perseption data.
The project is educational - no third-pary librares, no magic, just C and bytes

## Features

- Manual parsing of simple SQL-like commands (insert, select, delete, update)
- Insert and retrieve rows (INSERT, SELECT)
- Update rows (UPDATE)
- Soft deletion of rows (DELETE with is_deleted flag)
- Custom B-Tree
- Serialization of rows and binary file storage
- Meta-commands: `.help`,`.use`, `.exit`,`.quit`,`.tables`,`.drop`, `.btree`, `.constants`

## Build and Run

make
./csdb <db name>

## Example

```
$ ./sqlcs50 mydb.db
sqlcs50 version 0.1
Enter ".help" for commands.

> insert 1 devon devon@outland.com;
> select;
(1, devon, devon@outland.com)

> update 1 devon norton@outland.com;
> select;
(1, devon, norton@outland.com)

> delete 1;
> select;
```

## Project Structure

```

├── include/
│   ├── btree.h
│   ├── cursor.h
│   ├── execute.h
│   ├── input_buffer.h
│   ├── meta_commands.h
│   ├── pager.h
│   ├── prepare.h
│   ├── row.h
│   ├── table.h
├── src/
│   ├── btree.c
│   ├── cursor.c
│   ├── execute.c
│   ├── input_buffer.c
│   ├── meta_commands.c
│   ├── pager.c
│   ├── prepare.c
│   ├── row.c
│   ├── table.c
├── tests/
│   ├── tests_output/
│   │   ├── test_btree_large.py
│   │   ├── test_btree_small.py
│   │   ├── test_constants.py
│   ├── test_btree_large.py
│   ├── test_btree_small.py
│   ├── test_constants.py
│   └── ...
├── Makefile
├── main.c
├── run_script.py
└── README.md
```

## Goals

- Understand how a real data storage works at the byte level
- Learn systems programming: ponters, memory management, file system basics
- Build SQL prototype

## Author

Bertybergen
