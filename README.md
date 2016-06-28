# hlsplit
Print every x/nth line in a file similar to sed -n '1~3p'

Compile:
```
 gcc -Wall -O3 -march=native hlsplit.c -o hlsplit
```

Usage:
```
    hlsplit mode x n
```

Modes are:

```
    1 - print the lines
    2 - count the lines
    4 - do every line (ignores x/n)
    8 - faster wc -l (ignores x/n)
```

Using the result of `seq 1 1e9`

```
   # Wordcount -l
   time ./hlsplit 8 <../hseq/seq.1e9 # 1.5sec  6GB/s
   time wc -l <../hseq/seq.1e9       # 4.99sec 1.8GB/s

   # print every line (read and write)
   time ./hlsplit 5 <../hseq/seq.1e9 >/dev/null # 4.89  1.89GB/s

   # Perform the split
   time ./hlsplit 1 1 3 <../hseq/seq.1e9 >/dev/null # 3.44s
```
