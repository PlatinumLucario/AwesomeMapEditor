# Awesome Map Editor
Awesome Map Editor (AME) is a map editor for 3rd generation Pokémon games: Ruby, Sapphire, FireRed, LeafGreen, and Emerald.

<b>Please note that AME currently cannot write to ROMs, which means the save function is unusable, as this functionality has not been implemented yet.</b>

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

So the directories you made should look somewhat like this:
```
[AME Root directory]
├── AwesomeMapEditor
├── QBoy
│    └── bin
│         ├── debug
│         └── release
└── yaml-cpp
     └── bin
```
#### Building AME

1. Open all the projects in Qt and set QBoy build directories to `../[AMEroot]/QBoy/bin/debug` and</br>`../[AMEroot]/QBoy/bin/release` respectively and set yaml-cpp build directory to `../[AMEroot]/yaml-cpp/bin`. For yaml-cpp, open the `CMakeLists.txt` file and ensure <b>BUILD_SHARED_LIBS</b> is <b>enabled</b> in its project settings.

2. Build them all, working forwards from yaml-cpp to QBoy to AME itself.

3. Copy the `../[AMEroot]/AwesomeMapEditor/resources/config` folder into the AME executable directory (usually named `build-AwesomeMapEditor-Desktop_Qt_[version-number]_[kit-used]_64bit-[debug-or-release]`).

### Building (Windows)

#### Setting up the directories
1. Place the dependencies in the same directory as the AME root and rename them as the following: `QBoy-master` > `QBoy` and `yaml-cpp-master` > `yaml-cpp`.

2. Create a folder named "bin" in both `../[AMEroot]/QBoy` and `../[AMEroot]/yaml-cpp` directories (must be at the root of those directories).

3. Create two folders in `../[AMEroot]/QBoy/bin` and name them "debug" and "release".

So the directories you made should look somewhat like this:
```
[AME Root directory]
├── AwesomeMapEditor
├── QBoy
│    └── bin
│         ├── debug
│         └── release
└── yaml-cpp
     └── bin
```
#### Building AME

1. Open all the projects in Qt and set QBoy build directories to `../[AMEroot]/QBoy/bin/debug` and</br>`../[AMEroot]/QBoy/bin/release` respectively and set yaml-cpp build directory to `../[AMEroot]/yaml-cpp/bin`. For yaml-cpp, open the `CMakeLists.txt` file and ensure <b>BUILD_SHARED_LIBS</b> is <b>enabled</b> in its project settings. <b>YAML_CPP_BUILD_TESTS</b> must also be <b>disabled</b> in the its project settings, otherwise GTest (Google Test Framework) build errors will prevent it from being built, as it doesn't work with MinGW for some reason.

2. Build them all, working forwards from yaml-cpp to QBoy to AME itself.

3. Copy the `../[AMEroot]/AwesomeMapEditor/resources/config` folder into the AME executable directory (usually named `build-AwesomeMapEditor-Desktop_Qt_[version-number]_[kit-used]_64bit-[debug-or-release]`).

Now you can help us develop the future of Pokémon ROM hacking!

## Credits
**Diegoisawesome** - Developer

**Pokedude** - Developer
