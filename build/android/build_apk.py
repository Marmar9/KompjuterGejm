from sys import argv, exit
from os import execvp, fork, waitpid
from zipfile import ZipFile

aapt2 = ["aapt2","link",
         "-o",argv[1],
         "-I", "/opt/android-sdk/platforms/android-35/android.jar",
         "--manifest","AndroidManifest.xml"
         ]

apksigner = ["apksigner","sign",
             "--ks",argv[4],
             "--ks-pass",f"pass:{argv[5]}",
             "--ks-key-alias",argv[6],
             ]

if (argv[7] == ""):
    apksigner += ["--key-pass",f"pass:{argv[7]}"]

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
    zipf.write(argv[3], arcname=f"lib/{argv[2]}/{argv[3]}")

    if (argv[8] == "DEBUG"):
        zipf.write(f"../val-layers/{argv[2]}/libVkLayer_khronos_validation.so", arcname=f"lib/{argv[2]}/libVkLayer_khronos_validation.so")

waitpid(run_binary(apksigner),0)
