
SUB_DIRS=lib pm tomatod cli

all:
	for dir in $(SUB_DIRS); do \
		make -C $$dir; \
	done


.PHONY:clean

clean:
	for dir in $(SUB_DIRS); do \
		make -C $$dir clean; \
	done
