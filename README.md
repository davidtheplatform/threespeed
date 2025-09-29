# Install
* install python 3.12 and a recent verion of g++ (must support c++20 and coroutines)
* install SDL2
* Run `make -j libs` in the root directory

# Running a project
`python3.12 src/compiler.py -p <path to sb3> -o <folder to output to> --run`
You have to be in the root directory of the project for this to work

# Development
Run `make -j libs` any time you change a .cpp file. If you change a .h file you probably will have to run `make clean` then `make -j libs`.
`python3.12 src/compiler.py --help` shows some more useful options
