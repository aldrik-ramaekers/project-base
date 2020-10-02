import subprocess
import platform
import shutil
import glob
import sys
import os

# config
os_name = platform.system()
main_file = "src/main.c"
compiler = "gcc"
linker = "ar"
install_dir = "C:/mingw/mingw64/x86_64-w64-mingw32/"
obj_output_file = "libprojectbase.o"
lib_output_file = "libprojectbase.a"

include_dir = install_dir + "include/projectbase"
lib_dir = install_dir + "lib/" + lib_output_file

def do_windows_build():
    # make resource object file
    command = "ld -r -b binary -o data.o src/resources/mono.ttf"
    subprocess.call(command)

    # make object file
    libs = "-lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -ldbghelp"
    flags = "-m64 -c -Wall -O3"
    command = compiler + " " + flags + " " + main_file + " " + "-o " + obj_output_file + " " + libs
    subprocess.call(command)

    # make static lib
    command = linker + " rcs " + lib_output_file + " " + obj_output_file + " data.o "
    subprocess.call(command)

# remove old files
def do_setup():
    if os.path.isfile(lib_dir):
        os.remove(lib_dir)
    
    if (os.path.isdir(include_dir)):
        shutil.rmtree(include_dir)
    shutil.copytree("src", include_dir)

# install static lib and header files
def do_install():
    shutil.move(lib_output_file, lib_dir)
    for file in glob.glob("src/**/*.h", recursive=True):
        shutil.copy(file, include_dir + file[4:0])

# cleanup rogue files
def do_cleanup():
    if os.path.isfile(obj_output_file):
        os.remove(obj_output_file)

    if os.path.isfile("data.o"):
        os.remove("data.o")

# ##################
# Build library
# ##################
if os_name == "Windows":
    do_windows_build()

do_setup()
do_install()

# ##################
# Build examples
# ##################
subprocess.call("ld -r -b binary -o data.o examples/mono.ttf")

for file in glob.glob("examples/*.c"):
    subprocess.call(compiler + " -m64 -O3 " + file + " data.o -o " + file + ".exe -lprojectbase -lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -ldbghelp")

do_cleanup()

subprocess.call("./examples/example_1.c.exe");