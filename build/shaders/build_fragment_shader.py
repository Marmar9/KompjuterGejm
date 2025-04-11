import os
from sys import argv
from os import execvp, fork, waitpid,path

glsc = ["glslc","-fshader-stage=fragment",argv[1],"-o",argv[2] + ".spv"]

objcopy_bin = f"{os.environ['NDK']}/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-objcopy" if os.environ["TARGET"] == "android" else "/usr/bin/llvm-objcopy"

objcopy = [objcopy_bin,
           "--input-target","binary",
           "--output-target",f"elf64-{os.environ['ARCH'].replace("_","-")}",
           "--set-section-alignment",".data=4",
           argv[2] + ".spv",
           argv[2]]

def run_binary(binary: list[str]) -> int:
    pid = fork() 
    if pid == 0:  
        try:
            execvp(binary[0], binary)  
        except Exception as e:
            exit(f"Error executing {binary[0]}: {e}")
    return pid

waitpid(run_binary(glsc),0);

# Padd file's size to smallest multiple of 4 >= file's size
f_sz = path.getsize(argv[2] + ".spv")

append_sz = ((f_sz + 3) & ~3) - f_sz
print(f_sz)

with open(argv[2] + ".spv","ab") as f:
    f.write(bytearray(append_sz))

waitpid(run_binary(objcopy),0);
