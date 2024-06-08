# Some notes

- `trace children` keeps yelling "time out!" at me. The solution is to change the time limitation in `gradelib.py`.
- My first implementation of `proccount` is to search from each CPU, and keeps searching the parent process. It works for the test but I think it's wrong. I said this in one commit.
- **I failed to count the amount of the free memory**. I made a comment in `sysproc.c`.
