ADDR ?= 1
SN = E661AC88638AA328
FILE = build/src/RPI_Pico.uf2

build_rp:
	bash build_pico.sh

rm_build:
	rm -rf build

load_u2f:
	picotool load $(FILE) -x -f 
	