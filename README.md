# hlsplit
Print every x/nth line in a file similar to sed -n '1~3p'

Usage:
 hlsplit -1   - count lines like wc -l
 hlsplit  0   - print every line
 hlsplit  1 3 - print the first line out of every 3

