#!/bin/env bash


gn gen out

envsubst < app/inc/platform/android/res/AndroidManifest.xml.template > out/AndroidManifest.xml

ninja -C out
