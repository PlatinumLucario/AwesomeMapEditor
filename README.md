# Awesome Map Editor
Awesome Map Editor (AME) is a map editor for 3rd generation Pokémon games: Ruby, Sapphire, FireRed, LeafGreen, and Emerald.

## Dependencies
[QBoy](https://github.com/pokedude9/QBoy)  
[yaml-cpp](https://github.com/jbeder/yaml-cpp)

## Getting Started
First, you will need to install Qt Creator, you can download it at https://www.qt.io/download-open-source

1. Create a folder named AME (or whatever you'd like to call it), this will be the folder used for the AME root directory.

2. Ensure that Cmake is installed during the installation of Qt. It's required in order to open the CMakeLists.txt file and build yaml-cpp.


### Building (GNU/Linux)

#### Setting up the directories
1. Place the dependencies in the same directory as the AME root and rename them as the following: `QBoy-master` > `QBoy` and `yaml-cpp-master` > `yaml-cpp`.

2. Create a folder named "bin" in both `../[AMEroot]/QBoy` and `../[AMEroot]/yaml-cpp` directories (must be at the root of those directories).

3. Create two folders in `../[AMEroot]/QBoy/bin` and name them "debug" and "release".

So your directories should look somewhat like this:

[AME]</br>
|</br>
|————[AwesomeMapEditor]</br>
|</br>
|————[QBoy]——[bin]</br>
| 　 　 　 　 　 　 　 　 　|</br>
| 　 　 　 　 　 　 　 　 [debug]</br>
| 　 　 　 　 　  　　 　 [release]</br>
|</br>
|————[yaml-cpp]———[bin]</br>

#### Building AME

1. Open all the projects in Qt and set QBoy build directories to `../[AMEroot]/QBoy/bin/debug` and `../[AMEroot]/QBoy/bin/release` respectively and set yaml-cpp build directory to `../[AMEroot]/yaml-cpp/bin`. For yaml-cpp, open the `CMakeLists.txt` file and ensure BUILD_SHARED_LIBS is enabled in its project settings.

2. Build them all, working forwards from yaml-cpp to QBoy to AME itself.

3. Copy the `../[AMEroot]/AwesomeMapEditor/resources/config` folder into the AME executable directory (usually named `build-AwesomeMapEditor-Desktop_Qt_[version-number]_[kit-used]_64bit-[debug-or-release]`).

Now you can help us develop the future of Pokémon ROM hacking!

## Credits
**Diegoisawesome** - Developer

**Pokedude** - Developer
