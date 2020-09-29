import subprocess
import platform
import shutil
import os

os = platform.system()
main_file = "src/project_base.h"
compiler = "gcc"
linker = "ar"
install_dir = "C:\\mingw\\mingw64\\x86_64-w64-mingw32\\"
obj_output_file = "libprojectbase.o"
lib_output_file = "libprojectbase.a"

def do_windows_build():
    # make object file
    libs = "-lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -ldbghelp"
    flags = "-m64 -c -Wall -O3 -Wno-unused-label -Wno-unused-variable"
    command = compiler + " " + flags + " " + main_file + " " + "-o " + obj_output_file + " " + libs
    subprocess.call(command)

    # make static lib
    command = linker + " rcs " + lib_output_file + " " + obj_output_file
    subprocess.call(command)

def do_install():
    shutil.move(obj_output_file, install_dir + "lib\\" + obj_output_file)
    shutil.copytree("src", install_dir + "lib\\projectbase", symlink=False, ignore=None, copy_function=copy2, ignore_dangling_symlins=False)

def do_cleanup():
    os.remove(obj_output_file)



if os == "Windows":
    do_windows_build()

do_install()
do_cleanup()