#Compiler
CC=gcc

#.cpp files
Files=main.c functions.c
#.o Files
OBJS=main.o functions.o

#Executable
EXEC=exe

#Parameters for run
ARGS= em.txt 3 12 12 6

#input file
INPUT= 

#Create executable
$(EXEC):$(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

#Run
run: $(EXEC)
	./$(EXEC) $(ARGS) $(INPUT)

#debug
debug: 
	$(CC) -g3 $(Files) -o $(EXEC) 
	gdb -q $(EXEC)

#memory-loss
memory:
	make
	valgrind --leak-check=full -s ./$(EXEC) $(ARGS) $(INPUT)
#--leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
		 ./executable exampleParam1 \
https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks 

#Clear
clean:
	rm -f $(OBJS) $(EXEC) 	