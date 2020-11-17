MAKEFLAGS += -s
MAKEFLAGS += --always-make

main_file = src/main.c
output_file = libprojectbase
flags = -m64 -c -Wall -O3

ifeq ($(OS), Windows_NT)
	install_dir = C:/mingw/mingw64/x86_64-w64-mingw32/
	permissions = 
	libs = -lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -ldbghelp

	# Commands
	install_deps_command = empty
	create_examples_command = examples_windows
	generate_docs_command = docs_windows
else
	install_dir = /usr/
	permissions = sudo
	libs = -lX11 -lGL -lGLU -lXrandr -lm -lpthread -ldl

	# Commands
	install_deps_command = install_deps
	create_examples_command = examples_linux
	generate_docs_command = docs_linux
endif

include_dir = $(install_dir)include/projectbase
lib_dir = $(install_dir)lib/$(output_file).a

all:
	make $(install_deps_command)
	make examples
	make docs

empty:
	@$(NOECHO) $(NOOP)

# Install deps (Linux)
install_deps:
	sudo apt-get --yes --force-yes install freeglut3-dev
	sudo apt-get --yes --force-yes install binutils-gold
	sudo apt-get --yes --force-yes install g++
	sudo apt-get --yes --force-yes install mesa-common-dev
	sudo apt-get --yes --force-yes install build-essential
	sudo apt-get --yes --force-yes install libglew1.5-dev
	sudo apt-get --yes --force-yes install libglm-dev
	sudo apt-get --yes --force-yes install mesa-utils
	sudo apt-get --yes --force-yes install libglu1-mesa-dev
	sudo apt-get --yes --force-yes install libgl1-mesa-dev
	sudo apt-get --yes --force-yes install libxrandr-dev

# Build (Windows + Linux)
build:
	$(permissions) mkdir -p "build/"
	$(permissions) rm -rf "$(include_dir)"
	$(permissions) mkdir -p "$(include_dir)"

	ld -r -b binary -o build/data.o src/resources/mono.ttf
	gcc $(flags) $(main_file) -o build/$(output_file).o $(libs)
	ar rcs build/$(output_file).a build/$(output_file).o build/data.o

	$(permissions) cp -a "src/." "$(include_dir)"
	$(permissions) cp "build/$(output_file).a" "$(lib_dir)"

## Examples (Windows + Linux)
examples:
	make build
	make $(create_examples_command)

examples_windows:
	gcc -m64 -g examples/example_1.c -o build/example1.exe -lprojectbase $(libs)

examples_linux:
	gcc -m64 -g examples/example_1.c -o build/example1 -lprojectbase $(libs)
	sudo chmod +x build/example1

# Docs (Windows)
docs:
	$(permissions) mkdir -p "docs/"
	gcc -m64 -g utils/gen_docs.c -o build/gen_docs.exe -lprojectbase $(libs)
	./build/gen_docs.exe
	htmldoc -f docs/Documentation.pdf -t pdf build/docs.html --embedfonts --encryption --headfootsize 8 --charset utf-8 --titlefile build/docs_title.html --toctitle Content --footer h.1 --numbered --left 0.4in --right 0.4in --textcolor 222222 --fontsize 10
