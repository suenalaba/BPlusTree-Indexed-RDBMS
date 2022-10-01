# Mac Setup and Installation guide

1. Clone this repository using `git clone https://github.com/suenalaba/BPlusTree-Indexed-RDBMS.git` or unzip the zip folder.
2. `cd` to the folder and open the project in Visual Studio Code using `code .`
3. Ensure that you have a C++ compiler. (clang++ for MAC, not sure for Windows.)

- Run `clang --version` and a version should show up if your compiler exist.
- Ensure your compiler is able to support `C++11`
- To install a compiler for Visual Studio Code more information can be found here: [CppCompilerGuide] (https://code.visualstudio.com/docs/cpp/config-clang-mac)

4. Ensure all C++ files are included in compilation by:

1. Click on `terminal` and then -> `configure default build task`
1. Include the following `args` in `tasks.json` to ensure all C++ files are compiled on build:

```
"tasks": [
  {
    "args": [
      "-g",
      "${fileDirname}/*.cpp",
      "-o",
      "${fileDirname}/${fileBasenameNoExtension}",
      "-std=c++11"
    ],
  }
]
```

5. <b>IMPORTANT<b>: Ensure that your active file is `main.cpp` (It is the one selected.)
6. Open terminal and select `run build task` with the `main.cpp` file selected and select your C++ Compiler(clang preferred)
7. After successful compilation you should see: `Build finished successfully.`
8. Open a new terminal and run `./main`.
9. Select block size by inputting `1` or `2` accordingly.
10. If the compilation is successful, that should be the following user input prompt: <br> `Select Block Size (Enter 1 or 2): ` <br>
11. Upon selecting a block size by inputting `1` or `2`, something like this template will be printed (May differ based on Block Size Selected)

```
===================================================================
Experiment 1 Results
===================================================================
The number of blocks used is: 42813
The database size (in B) based on blocks is: 21406500
The database size (in B) based on records is: 21406360
The database size (in MB) based on blocks is: 21.4065
The database size (in MB) based on records is: 21.4064
```
