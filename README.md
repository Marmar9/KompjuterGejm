## KompjuterGejm
This project covers vulkan game engine wirtten for android platform.
## Building from source
> [!NOTE]
> The build was tested on
> 
> android-ndk r27
> 
> clang18 (when building on linux)
> 
> android-platform 35

Export build configuration environment variables 

```sh

export ARCH= # either 'aarch64' or 'x86-64'
export TARGET= # 'android' or 'linux'
export DEBUG= # 'true' if want to enable debuging


```
If building for android

Create a keystore and key using either android **keytool** (cli) or **android studio**

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

If running development build validation layer architecture directiories are expected to live in val-layers/ directory

Finally run 

```sh
./build.sh
```
