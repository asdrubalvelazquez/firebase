#!/bin/bash

# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -ex

cd "${REPO}"/ReleaseTooling

# This file will have non Firebase SDKs that will be built by ZipBuilder.
ZIP_POD_JSON="non_firebase_sdk.json"
rm -f "${ZIP_POD_JSON}"
IFS=' ,' read -a NON_FIREBASE_SDKS <<< "${NON_FIREBASE_SDKS}"

num_sdk="${#NON_FIREBASE_SDKS[@]}"
echo "[" >> "${ZIP_POD_JSON}"
for sdk in "${NON_FIREBASE_SDKS[@]}"
do
  echo "{\"name\":\"${sdk}\"}" >>  "${ZIP_POD_JSON}"
  if [ "$num_sdk" -ne 1 ]; then
    echo ",">>  "${ZIP_POD_JSON}"
  fi
  num_sdk=$((num_sdk-1))
done
echo "]" >>  "${ZIP_POD_JSON}"
mkdir -p "${REPO}"/sdk_zip
swift run zip-builder --keep-build-artifacts --update-pod-repo --platforms ios \
    --zip-pods "${ZIP_POD_JSON}" --output-dir "${REPO}"/sdk_zip --disable-build-dependencies

unzip -o "${REPO}"/sdk_zip/Frameworks.zip -d "${HOME}"/ios_frameworks/Firebase/

# Move Frameworks to Firebase dir, so be align with Firebase SDKs.
mv -n "${HOME}"/ios_frameworks/Firebase/Binaries "${HOME}"/ios_frameworks/Firebase/NonFirebaseSDKs/

tree -L 3 "${HOME}"/ios_frameworks/Firebase/NonFirebaseSDKs/

for xcframework in "${HOME}"/ios_frameworks/Firebase/NonFirebaseSDKs/**/*.xcframework; do
  echo "checkpoint 1"
    if [ -d "$xcframework/Resources" ]; then
      echo "checkpoint 2"
        for framework_resource in "$xcframework/Resources"/*; do
          echo "checkpoint 3"
            for platform in "ios-arm64" "ios-arm64_x86_64-simulator"; do
              echo "checkpoint 4"
                framework="$xcframework/$platform/$(basename "$xcframework" .xcframework).framework"
                if [ -d  $framework ]; then
                  echo "checkpoint 5"
                    cp -rP $framework_resource $framework
                    echo "Copying $framework_resource to $framework"
                fi
            done
            rm -rf $framework_resource
            echo "Removing $framework_resource"
        done
    fi
done
