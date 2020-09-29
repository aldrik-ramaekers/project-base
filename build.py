import subprocess
import platform
import shutil
import glob
import os

os_name = platform.system()
main_file = "src/project_base.h"
compiler = "gcc"
linker = "ar"
install_dir = "C:/mingw/mingw64/x86_64-w64-mingw32/"
obj_output_file = "libprojectbase.o"
lib_output_file = "libprojectbase.a"

include_dir = install_dir + "include/projectbase"
lib_dir = install_dir + "lib/" + lib_output_file

def do_windows_build():
    # make object file
    libs = "-lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -ldbghelp"
    flags = "-m64 -c -s -Wall -O3"
    command = compiler + " " + flags + " " + main_file + " " + "-o " + obj_output_file + " " + libs
    subprocess.call(command)

    # make static lib
    command = linker + " rcs " + lib_output_file + " " + obj_output_file
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

def do_cleanup():
    if os.path.isfile(obj_output_file):
        os.remove(obj_output_file)

if os_name == "Windows":
    do_windows_build()

do_setup()
do_install()
do_cleanup()