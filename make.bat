echo off
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=~/Documents/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release || (
    echo Build failed.
    pause
    exit /b
)

copy /Y Release\main.exe ..
copy /Y Release\*.dll ..

cd ..