EXAMPLE_DIRS = unit experimental_unit

all:
	for dir in $(EXAMPLE_DIRS); do ($(MAKE) -C $$dir) || exit 1; done
