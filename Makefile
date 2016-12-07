PIO := platformio

all:
	${PIO} run

upload:
	${PIO} upload

clean:
	${PIO} clean

program:
	${PIO} program

update:
	${PIO} update
