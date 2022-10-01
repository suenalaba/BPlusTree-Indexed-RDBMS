# Windows Setup and Installation guide

1. Clone this repository using `git clone <REPO_URL>` or unzip the zip folder.
2. `cd` to the folder and open the project in Visual Studio Code using `code .`
3. Ensure that you have Visual Studio Code and C++ Compiler (MingW g++ for Windows) installed

- Run `g++ --version` and a version should show up if your compiler exist.
- Ensure your compiler is able to support `C++11`
- To install a compiler for Visual Studio Code more information can be found here: [CppCompilerGuide] https://code.visualstudio.com/docs/cpp/config-mingw

3. Ensure all C++ files are included in compilation by:

4. Click on `terminal` and then -> `configure default build task`
5. Include the following `args` in `tasks.json` to ensure all C++ files are compiled on build:

```
"tasks": [
  {
		"args": [
			"-g",
			"-Wall",
			"-Wextra",
			"-Wpedantic",
			"${fileDirname}/*.cpp",
			"-o",
			"${fileDirname}/${fileBasenameNoExtension}",
			"-std=c++11"
		],
  }
]
```

6. <b>IMPORTANT<b>: Ensure that your active file is `main.cpp` (It is the one selected.)
7. Open terminal and select `run build task` with the `main.cpp` file selected and select your C++ Compiler(g++)
8. After successful compilation you should see: `Build finished successfully.`
9. Open a new terminal and run `./main`. or use command `g++ *.cpp -std=c++11 -o output` for executable file.
10. Select block size by inputting `1` or `2` accordingly.
11. If the compilation is successful, that should be the following user input prompt: <br> `Select Block Size (Enter 1 or 2): ` <br>
12. Upon selecting a block size by inputting `1` or `2`, something like this template will be printed (May differ based on Block Size Selected)

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

# Windows Executable

Windows Executable provided. Double click the executable `output.exe` to run
