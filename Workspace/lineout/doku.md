## Übung 5: lineout Testausgabe

```cpp
sbsy@JULIANRIEGEFA78:~$ cd Workspace
bsy@JULIANRIEGEFA78:~/Workspace$ cd lineout
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ gcc -o lineout lineout.c -Wall -Wextra
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ls
lineout  lineout.c
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ touch text.txt
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ vim text.txt
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ cat text.txt
erste zeile
zweite zeile
hallo 3.
das ist 4
5.
bsy ist super toll
das ist ein c programm
das ist zeile 8 und somit die letzte des dokuments.
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout 1,3-4,7- text.txt
1   erste zeile
3   hallo 3.
4   das ist 4
7   das ist ein c programm
8   das ist zeile 8 und somit die letzte des dokuments.
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout text.txt
1   erste zeile
2   zweite zeile
3   hallo 3.
4   das ist 4
5   5.
6   bsy ist super toll
7   das ist ein c programm
8   das ist zeile 8 und somit die letzte des dokuments.
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout 5- text.txt
5   5.
6   bsy ist super toll
7   das ist ein c programm
8   das ist zeile 8 und somit die letzte des dokuments.
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout -5 text.txt
1   erste zeile
2   zweite zeile
3   hallo 3.
4   das ist 4
5   5.
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout 5 text.txt
5   5.
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout
Nutzung: ./lineout [intervalle] <datei>
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout t
Error in fopen.: No such file or directory
bsy@JULIANRIEGEFA78:~/Workspace/lineout$ ./lineout -m text.txt
Ungültiges Intervall: -m
```