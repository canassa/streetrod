.PHONY: all prepare objects check clean
all:
	./sr build
objects:
	./sr build --objects-only
prepare:
	./sr build --prepare-only
check:
	./sr check --all
clean:
	rm -rf work/layout
