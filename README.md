# hlsplit
Print every x/nth line in a file similar to sed -n '1~3p'

Usage:
    hlsplit mode x n

Modes are:
    1 - print the lines
    2 - count the lines
    4 - do every line (ignores x/n)
    8 - faster wc -l (ignores x/n)

