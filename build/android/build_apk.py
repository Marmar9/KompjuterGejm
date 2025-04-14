from sys import argv, exit
from os import execvp, fork, waitpid, getenv
from zipfile import ZipFile

aapt2 = ["aapt2","link",
         "-o",argv[1],
         "-I", f"/opt/android-sdk/platforms/android-{getenv("SDK_VERSION")}/android.jar",
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
    match argv[2]:
        case "aarch64":
            zipf.write(argv[3], arcname=f"lib/arm64-v8a/{argv[3]}")
            if (len(argv) == 9):
                zipf.write(f"../val-layers/arm64-v8a/libVkLayer_khronos_validation.so", arcname=f"lib/arm64-v8a/libVkLayer_khronos_validation.so")
        case "x86-64":
            zipf.write(argv[3], arcname=f"lib/x86-64/{argv[3]}")
            if (len(argv) == 9):
                zipf.write(f"../val-layers/x86-64/libVkLayer_khronos_validation.so", arcname=f"lib/x86-64/libVkLayer_khronos_validation.so")
    

waitpid(run_binary(apksigner),0)
