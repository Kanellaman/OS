#Compiler
CC=gcc

#.c files
Files=main.c ../modules/functions.c ../modules/execchild.c

#.o Files
OBJS1=main.o ../modules/functions.o
OBJS2=../modules/execchild.o ../modules/functions.o

#Executables
EXEC1=exe
EXEC2=execchild

#Output file
OUTPUT= output.txt

#Parameters for run
ARGS= text.txt 10 100 1000

#Create executable
$(EXEC1):$(OBJS1) $(OBJS2)
	$(CC) $(OBJS1)  -o $(EXEC1) -lpthread -lrt
	$(CC) $(OBJS2) -o $(EXEC2) -lpthread -lrt

#Run
run: $(EXEC1) $(EXEC2)
	./$(EXEC1) $(ARGS) > $(OUTPUT)

#Clear
clean:
	rm -f $(EXEC1) $(OBJS1) $(EXEC2) $(OBJS2) $(OUTPUT)