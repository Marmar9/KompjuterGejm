#!/bin/env bash

envsubst < android/AndroidManifest.xml.template > android/AndroidManifest.xml

gn gen out

ninja -C out
