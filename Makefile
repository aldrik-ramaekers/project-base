MAKEFLAGS += -s
MAKEFLAGS += --always-make

main_file = src/main.c
output_file = libprojectbase
flags = -m64 -c -Wall -O3

ifeq ($(OS), Windows_NT)
	install_dir = C:/mingw/mingw64/x86_64-w64-mingw32/
	install_deps_command = empty
	create_examples_command = examples_windows
	permissions = 
	libs = -lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -ldbghelp
else
	install_dir = /usr/
	install_deps_command = install_deps
	create_examples_command = examples_linux
	permissions = sudo
	libs = -lX11 -lGL -lGLU -lXrandr -lm -lpthread -ldl
endif

include_dir = $(install_dir)include/projectbase
lib_dir = $(install_dir)lib/$(output_file).a

all:
	make $(install_deps_command)
	make build

empty:
	@$(NOECHO) $(NOOP)

install_deps:
	sudo apt-get install freeglut3-dev
	sudo apt-get install binutils-gold
	sudo apt-get install g++
	sudo apt-get install mesa-common-dev
	sudo apt-get install build-essential
	sudo apt-get install libglew1.5-dev
	sudo apt-get install libglm-dev
	sudo apt-get install mesa-utils
	sudo apt-get install libglu1-mesa-dev
	sudo apt-get install libgl1-mesa-dev
	sudo apt-get install libxrandr-dev

build:
	$(permissions) mkdir -p "build/"
	$(permissions) rm -rf "$(include_dir)"
	$(permissions) mkdir -p "$(include_dir)"

	ld -r -b binary -o build/data.o src/resources/mono.ttf
	gcc $(flags) $(main_file) -o build/$(output_file).o $(libs)
	ar rcs build/$(output_file).a build/$(output_file).o build/data.o

	$(permissions) cp -a "src/." "$(include_dir)"
	$(permissions) cp "build/$(output_file).a" "$(lib_dir)"

## Examples
examples:
	make $(create_examples_command)

examples_windows:
	gcc -m64 -g examples/example_1.c -o build/example1.exe -lprojectbase $(libs)

examples_linux:
	gcc -m64 -g examples/example_1.c -o build/example1 -lprojectbase $(libs)
	sudo chmod +x build/example1