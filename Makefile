OBJS=test.o entities.o ecs.o sparse.o memory.o

all: test

test: $(OBJS)
	gcc $^ -o $@

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -f *.o test