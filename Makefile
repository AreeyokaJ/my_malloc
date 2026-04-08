CC = gcc 
CFLAGS = -Wall -std=c99 -fsanitize=address,undefined 

all: memgrind memtest t_split t_coalesce t_oom t_invalid_notmalloc t_invalid_notstart t_invalid_doublefree t_leak

memgrind: memgrind.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) memgrind.c mymalloc.c -o memgrind

memtest: memtest.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) memtest.c mymalloc.c -o memtest

t_split: tests/t_split.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) tests/t_split.c mymalloc.c -o t_split

t_coalesce: tests/t_coalesce.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) tests/t_coalesce.c mymalloc.c -o t_coalesce


t_invalid_doublefree: tests/t_invalid_doublefree.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) tests/t_invalid_doublefree.c mymalloc.c -o t_invalid_doublefree


t_invalid_notmalloc: tests/t_invalid_notmalloc.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) tests/t_invalid_notmalloc.c mymalloc.c -o t_invalid_notmalloc


t_invalid_notstart: tests/t_invalid_notstart.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) tests/t_invalid_notstart.c mymalloc.c -o t_invalid_notstart


t_leak: tests/t_leak.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) tests/t_leak.c mymalloc.c -o t_leak


t_oom: tests/t_oom.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) tests/t_oom.c mymalloc.c -o t_oom
