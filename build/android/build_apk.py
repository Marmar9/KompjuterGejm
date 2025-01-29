from sys import argv, exit
from os import execvp, fork, waitpid
from zipfile import ZipFile

aapt2 = ["aapt2","link",
         "-o",argv[1],
         "-I", "/opt/android-sdk/platforms/android-35/android.jar",
         "--manifest",argv[2]
         ]

apksigner = ["apksigner","sign",
             "--ks",argv[5],
             "--ks-pass",f"pass:{argv[6]}",
             "--ks-key-alias",argv[7],
             ]

if (argv[8] == ""):
    apksigner += ["--key-pass",f"pass:{argv[8]}"]

apksigner += ["--out", argv[1],
             argv[1]]

print(*apksigner)


def run_binary(binary: list[str]) -> int:
    pid = fork() 
    if pid == 0:  
        try:
            execvp(binary[0], binary)  
        except Exception as e:
            exit(f"Error executing {binary[0]}: {e}")
    return pid

waitpid(run_binary(aapt2),0)

with ZipFile(argv[1], "a") as zipf:
    zipf.write(argv[4], arcname=f"lib/{argv[3]}/{argv[4]}")

    if (argv[9] == "DEBUG"):
        zipf.write(f"../val-layers/{argv[3]}/libVkLayer_khronos_validation.so", arcname=f"lib/{argv[3]}/libVkLayer_khronos_validation.so")

waitpid(run_binary(apksigner),0)
