# disker 

```
sudo apt-get install cmake mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev

cmake -DCMAKE_PREFIX_PATH=~/Qt/6.2.0/gcc_64 ..
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

### doing 
- apply styling theme to App to make it all look nice
- add appveyor for CI / CD 
- freezing when clicking on app...

### priorities
- add donate button
- make fancy readme
- publish!
- show loading bar correctly
- handle not being able to access a folder
- add disk finder to show all drives attached
- fix size ordering for folders? no easy way...
- display total storage and size of drive in selection_widget... maybe with bar chart? 