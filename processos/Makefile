CC=gcc
CFLAGS=-Wall

all: scheduler plot-script

plot: scheduler plot-script
	./bin/scheduler
	./bin/plot

scheduler: main.c
	$(CC) $(CFLAGS) -o bin/$@ $^ -lm

plot-script: plot.c
	$(CC) $(CFLAGS) -o bin/plot $^

clean: 
	rm -r ./bin/* ./data/*