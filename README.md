### MINOS C++
C++ desktop version of https://github.com/cbass-d/minos

Used as a tool to learn Qt tools for C++

This tool provides the user with the ability to create the desired amount of subnets from a network address and mask

### Dependencies
* Qt5 or higher

### Building with CMake
From root directory
```
$ cmake -S . -B {target build directory path}
```

#### To-do
* Add visiual representation of bits
* Optimize creation of subnets

#### Issues
* Program crashes when wanting to create more than ~8k subnets
