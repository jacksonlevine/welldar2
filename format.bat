astyle.exe src/*.cpp src/wegot/*.hpp

@echo off
for %%f in (src\*.orig) do (
    del "%%f"
)
for %%f in (src\wegot\*.orig) do (
    del "%%f"
)

pause