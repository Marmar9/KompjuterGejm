## Game written in vulkan 
## Building from source
> [!NOTE]
> The build was tested on
> android-ndk r27
> clang18 (when building on linux)
> sdk 

Chose a build target, checkout [build/BUILDCONFIG.GN](buildconfig/BUILDCONFIG.GN) file.
Two variables **arch** and **target** allow configuration
Currently "android" and "linux" targets are supported "x86_64" arch is supported.

If building for android

Create a keystore and key using either android **keytool** (cli) or **android studio**

Export build configuration environment variables 

```sh

export ANDROID_HOME=
export NDK=

export KS_FILE=
export KS_PASS=
export KEY_ALIAS=
export KEY_PASS=

export SDK_VERSION=
export PACKAGE_NAME=
export APP_NAME=
export NDK_LIB_NAME=

```

If running development build  **debug** variable can be set to enable debug symbols
validation are also expected in val-layers/$arch/ directory

Finally run 

```sh
./build.sh
```
