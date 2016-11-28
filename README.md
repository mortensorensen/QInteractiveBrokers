# A Q wrapper for the Interactive Brokers API

# Building
````
mkdir build
cd build
cmake ..
make
````
Optionally, to generate an XCode project, run `cmake -G Xcode ..`.

# TWS API
To upgrade the TWS API download the source files from http://interactivebrokers.github.io/ and extract the Posix Client into `third_party/ib`:
````
cd third_party/ib
tar -xvf twsapi_macunix.971.01.jar
````
