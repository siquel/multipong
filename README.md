# Multipong

Multiplayer up to 32 pong game 

## Building 

### Windows
```bash
# clone source
git clone https://github.com/siquel/jn.git
git clone https://github.com/siquel/multipong.git
# change dir to multipong
cd multipong
# generate projects
..\jn\tools\bin\windows\genie.exe vs2015 # or just run vs2015.bat for now
```
Open the project in Visual Studio. 

### Linux 
```bash
# clone source
git clone https://github.com/siquel/jn.git
git clone https://github.com/siquel/multipong.git
# change dir to multipong
cd multipong
# generate projects for clang and gcc
../jn/tools/bin/linux/genie --gcc=linux-clang gmake
../jn/tools/bin/linux/genie --gcc=linux-gcc gmake
# build with clang or gcc
make -C .build/projects/gmake-linux-clang/ config=debug64
make -C .build/projects/gmake-linux/ config=debug64
```

