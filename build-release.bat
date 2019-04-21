Rem Prepare folders
mkdir build
mkdir build\x32
mkdir build\x64
mkdir build\x32\plugins
mkdir build\x64\plugins

Rem x86 build
set platform=x86
set configuration=release
msbuild "src\x64dbgPlaytime.sln" /m /verbosity:normal

Rem x64 build
set platform=x64
set configuration=release
msbuild "src\x64dbgPlaytime.sln" /m /verbosity:normal

Rem Prepare package
robocopy "bin\x32" "build\x32\plugins" /E /NJH /NJS
robocopy "bin\x64" "build\x64\plugins" /E /NJH /NJS
robocopy "lua" "build\lua" /E /NJH /NJS

Rem Complete
EXIT /B 0