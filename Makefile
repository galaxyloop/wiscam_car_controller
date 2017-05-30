TARGET:controller
all:
	gcc -o $(TARGET) $(TARGET).c
clean:
	rm -f *.o $(TARGET)
