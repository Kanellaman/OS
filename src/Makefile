#Compiler
CC=gcc

#.cpp files
Files=main.c ../modules/functions.c ../modules/execchild.c ../modules/my_rand.c
#.o Files
OBJS1=main.o ../modules/functions.o 
OBJS2=../modules/execchild.o ../modules/functions.o 
#Executable
EXEC1=exe
EXEC2=execchild
#Parameters for run
ARGS= text.txt 1 10 3

#Create executable
$(EXEC1):$(OBJS1) $(OBJS2)
	$(CC) $(OBJS1)  -o $(EXEC1) -lpthread -lrt
	$(CC) $(OBJS2) -o $(EXEC2) -lpthread -lrt

#Run
run: $(EXEC1) $(EXEC2)
	./$(EXEC1) $(ARGS) $(INPUT) > em.txt

#debug
#debug: 
#	$(CC) -g3 $(Files) -o $(EXEC) 
#	gdb -q $(EXEC)

#memory-loss
#memory:
#	make
#	valgrind --leak-check=full -s ./$(EXEC) $(ARGS) $(INPUT)
#--leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
		 ./executable exampleParam1 \
https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks 

#Clear
clean:
	rm -f $(OBJS1) $(EXEC1) $(EXEC2) $(OBJS2) 