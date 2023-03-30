CC = gcc # This line sets the compiler to be used to GCC.
#This line sets the compilation flags to be used. -Wall and -Wextra enable additional warnings during compilation,
#and -std=c99 specifies that the code should be compiled according to the C99 standard.
CFLAGS = -Wall -Wextra -std=c99 -Werror=implicit-function-declaration


#This line creates a rule that specifies the target all, which depends on the targets q1 and q2.
#This means that running make all will build both q1 and q2.
all: q1 q2


q1: q1.o
	$(CC) $(CFLAGS) -o q1 q1.o

q2: q2.o
	$(CC) $(CFLAGS) -o q2 q2.o

q1.o: q1.c
	$(CC) $(CFLAGS) -c q1.c

q2.o: q2.c
	$(CC) $(CFLAGS) -c q2.c

# This line creates a rule that specifies the target clean.
# This rule is used to clean up the directory by removing all the object files (*.o)
# and the executable files (q1 and q2).
clean:
	rm -f *.o q1 q2