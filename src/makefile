
SUB_DIRS=lib pm store trans cli

all:
	for dir in $(SUB_DIRS); do \
		make -C $$dir; \
	done


.PHONY:clean

clean:
	for dir in $(SUB_DIRS); do \
		make -C $$dir clean; \
	done
