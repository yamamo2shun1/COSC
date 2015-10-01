# COSC
Console command for Open Sound Control

## How to Compile
`gcc -o cosc cosc.c`

## How to Use
`$ ./cosc 127.0.0.1 8080 /test ifs 1 -3.14 hoge`

1st arg. = host IP(necessary) 
2nd arg. = host port(necessary) 
3rd arg. = osc address(necessary) 
4th arg. = osc typetag(necessary. supported i, f and s) 
5th and more args. = osc arguments
