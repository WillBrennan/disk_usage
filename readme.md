# disker 
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

## todo
### done 
- add common folders; can you access pinned / favourites from windows explorer / os x finder?
- create directory traversal to show storage
    * how do you get past permissions like with daisy disk?
    * you don't; but you need to know if you can't access something and get its size...
- populate tree structure 
- add close button to tabs and delete widgets
- create pie chart for storage
    - itemClicked(QTreeWidgetItem *item, int column)
    - itemClicked -> QTreeWidgetItem*
    - QTreeWidgetItem* -> Folder*
    - clicked(QPieSlice* slice)
    - Folder* -> QPieSlice*
    - Folder* -> QTreeWidgetItem*
    - QPieSlice* -> Folder*
- freezing when clicking on app...
- apply styling theme to App to make it all look nice
- make fancy readme
- publish!
- handle not being able to access a folder

### doing 
- add appveyor for CI / CD

### priorities
- add donate button
- show loading bar correctly
- add disk finder to show all drives attached
- fix size ordering for folders? no easy way...
- display total storage and size of drive in selection_widget... maybe with bar chart?