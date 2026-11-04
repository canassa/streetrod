.PHONY: all prepare objects dos check clean
all:
	./sr build
objects:
	./sr build --objects-only
prepare:
	./sr build --prepare-only
dos:
	./dosbox.sh --make
check:
	./sr check --all
clean:
	rm -rf work/layout work/dos
