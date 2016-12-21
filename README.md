# A Q wrapper for the Interactive Brokers API

# Building
````
mkdir build
cd build
cmake ..
make
````
Optionally, run `cmake -G Xcode ..` to generate an XCode project or `cmake -G MSVC ..` for Microsoft Visual Studio.

# Running
Running the following command will dynamically link the QIB functions into the `.ib` namespace:
````
.ib:(`:build/Debug/qib.0.0.1 2:(`LoadLibrary;1))`
````
The program must have the function `.ib.onrecv` defined which should take two arguments; function name and arguments, e.g.:
````
.ib.onrecv:{[fname;args] show (fname;args)};
```
See `app/ib.q` for a more complete example.

# TWS API
To upgrade the TWS API download the source files from http://interactivebrokers.github.io/ and extract the Posix Client into `third_party/ib`:
````
cd third_party/ib
tar -xvf twsapi_macunix.971.01.jar
````
