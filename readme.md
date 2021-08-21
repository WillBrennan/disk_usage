# disker 
[![.github/workflows/create_release.yml](https://github.com/WillBrennan/disk_usage/actions/workflows/create_release.yml/badge.svg)](https://github.com/WillBrennan/disk_usage/actions/workflows/create_release.yml)
## Overview 
Disker analyses disk usage and lets you free up space on your computer. Over the years I've used WinDirStat on Windows, paid for Daisy Disk on Mac, and then used the in-built one on Ubuntu. It got annoying swapping between all of them with their different features and quirks, so now its time for a single application covering all of them! 

Feel free to make PRs or Issues for features and bugs! 

![overview](https://raw.githubusercontent.com/WillBrennan/disk_usage/master/assets/examples/overview.png)
![analyser](https://raw.githubusercontent.com/WillBrennan/disk_usage/master/assets/examples/analyser.png)

## Releases 
AppVeyor and Jenkins are used for CI/CD, to release application builds for Windows, OS X, and Ubuntu. They can be downloaded from the releases page, 

- Pre-built Windows
- Pre-built OS X  / Mac
- Pre-build Ubuntu

## Building from scratch
This application is built using Qt, OS calls, and the standard library. To build from scratch, these packages must be installed,

```bash
sudo apt-get install cmake mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev
```
. The project uses CMake, the location of Qt must be given to cmake. Then it can be built and tested in the normal way, 

```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=~/Qt/6.2.0/gcc_64 ..
make -j8 
make test 
./app
```

## Within Docker 
You can also build within docker, this is helpful to debug the workflow actions,
```bash 
docker build -t disker --platform linux/x86_64 .
```
then to execute it, 
```bash
docker run --platform linux/amd64 -it disker bash
```

## todo
### remaining
- add nice icons
- fix hang when search has finished in the Analyser widget (caused by lots of work in the Qt thread...)
    - slowly updating the QTreeWidget would amortize this cost over the search so nobody realises...
- show loading bar correctly
- add disk finder to show all drives attached
- fix size ordering for folders? no easy way...
- display total storage and size of drive in selection_widget... maybe with bar chart?