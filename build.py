import subprocess
import platform
import shutil
import glob
import sys
import os

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def log(message):
    print(bcolors.OKGREEN + message + bcolors.ENDC)

def report_and_exit_if_failed(result, message):
    if (result.returncode != 0):
        print(result.stderr, file=sys.stderr)
        report_and_exit(message)

def report_and_exit(message):
    print(bcolors.FAIL + message + bcolors.ENDC)
    do_cleanup()
    sys.exit()

if (sys.version_info.major < 3 or (sys.version_info.major == 3 and sys.version_info.minor < 5)):
    report_and_exit("Need Python version 3.5 or newer, using: " + sys.version)

# config
os_name = platform.system()
main_file = "src/main.c"
compiler = "gcc"
linker = "ar"
install_dir = "C:/mingw/mingw64/x86_64-w64-mingw32/"
if (os_name == "Linux"):
    install_dir = "/usr/"

obj_output_file = "libprojectbase.o"
lib_output_file = "libprojectbase.a"

include_dir = install_dir + "include/projectbase"
lib_dir = install_dir + "lib/" + lib_output_file


def do_linux_setup():
    log("Installing dependencies")
    subprocess.run(["sudo", "apt-get", "install", "freeglut3-dev"], stdout=subprocess.DEVNULL)
    subprocess.run(["sudo", "apt-get", "install", "binutils-gold", "g++", "mesa-common-dev", "build-essential", "libglew1.5-dev", "libglm-dev"], stdout=subprocess.DEVNULL)
    subprocess.run(["sudo", "apt-get", "install", "mesa-utils"], stdout=subprocess.DEVNULL)
    subprocess.run(["sudo", "apt-get", "install", "libglu1-mesa-dev"], stdout=subprocess.DEVNULL)
    subprocess.run(["sudo", "apt-get", "install", "libgl1-mesa-dev"], stdout=subprocess.DEVNULL)
    subprocess.run(["sudo", "apt-get", "install", "libxrandr-dev"], stdout=subprocess.DEVNULL)

def do_build():
    log("Creating library")
    # make resource object file
    command = ["ld", "-r", "-b", "binary", "-o", "data.o", "src/resources/mono.ttf"]
    result = subprocess.run(command, capture_output=True, text=True)
    report_and_exit_if_failed(result, "Linker failed")

    # make object file
    if (os_name == "Windows"):  
        command = [compiler, "-m64", "-c", "-Wall", "-O3", main_file, "-o", obj_output_file, "-lopengl32", "-lkernel32", "-lglu32", "-lgdi32", "-lcomdlg32", "-ldbghelp"]
    if (os_name == "Linux"):
        command = [compiler, "-m64", "-c", "-Wall", "-O3", main_file, "-o", obj_output_file, "-lX11", "-lGL", "-lGLU", "-lXrandr", "-lm", "-lpthread", "-ldl"]

    result = subprocess.run(command, capture_output=True, text=True)
    report_and_exit_if_failed(result, "Compilation failed")

    # make static lib
    command = [linker, "rcs", lib_output_file, obj_output_file, "data.o"]
    result = subprocess.run(command, capture_output=True, text=True)
    report_and_exit_if_failed(result, "Archiver failed")

# remove old files
def do_setup():
    log("Removing old files")
    if os.path.isfile(lib_dir):
        os.remove(lib_dir)
    
    if (os.path.isdir(include_dir)):
        shutil.rmtree(include_dir)
    
    for file in glob.glob("examples/*.exe"):
        os.remove(file)

# install static lib and header files
def do_install():
    log("Installing library and headers")
    shutil.move(lib_output_file, lib_dir)
    shutil.copytree("src", include_dir)

# cleanup rogue files
def do_cleanup():
    log("Cleaning up rogue files")
    if os.path.isfile(obj_output_file):
        os.remove(obj_output_file)

    if os.path.isfile("data.o"):
        os.remove("data.o")

# ##################
# Build library
# ##################
if os_name == "Linux":
    do_linux_setup()

do_build()
do_setup()
do_install()

# ##################
# Build examples
# ##################
log("Compiling example programs")
for file in glob.glob("examples/*.c"):
    subprocess.call(compiler + " -m64 -O3 " + file + " data.o -o " + file + ".exe -lprojectbase -lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -ldbghelp")

do_cleanup()

subprocess.call("./examples/example_1.c.exe")