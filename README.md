# Assignments-Evaluator
Automated C Program Evaluator

### Introduction

Welcome to the Automated C Program Evaluator! This tool simplifies the process of testing C programs submitted by users, making it efficient for evaluating assignments or projects.

### How to Use

Using the program involves the following steps:
1. Clone "ex21.c" and "ex22.c" programs into your working directory. 
2. Compile the source code using the Makefile attached:
```bash
    $ make
```
3. Prepare a configuration file (`conf.txt`) according to the specified format built with three lines:
        
    1. **Line 1**: Path to a directory containing subdirectories. Each subdirectory represents a student, and each student directory should contain a `.c` file representing their                 submission that requires evaluation.
    2. **Line 2**: Path to the input file.
    3. **Line 3**: Path to the file containing the expected output corresponding to the input file specified in line 2.
    (The configuration file ends with a newline character.)

    For example:

    ```
    students
    io/input.txt
    io/output.txt
    ```
    
4. Place input files and expected output files in their respective directories. Example files are attached in the repository (io/input.txt, io/correct_output.txt).
5. Run the program with the configuration file as an argument. Below is an example command:
    ```bash
    $ ./a.out conf.txt
    ```
6. A `results.csv` file containing the performance of each student will be generated in the directory. the performence will include: student name(or the name of its directory), score, and a reason/note accordingly.
   for example :
   
       Monica,100,EXCELLENT
       Phoebe,0,NO_C_FILE
       .
       .
       .

### File Comparison Program

The program utilizes a files comparison program (`ex21.c`) written in C. This program is attached to the repository and is used by the main program (ex21.c) to compare output files with expected results.

### Coding Principles

Several coding principles are implemented in this project:
- Error messages are handled gracefully, providing detailed explanations where necessary to aid in debugging.
- Pipe redirection is employed to redirect input and output streams between processes, enabling communication between the main program and compiled C programs being tested.
- Temporary files and resources are managed appropriately to maintain system cleanliness and prevent clutter.
- System calls and functions from the C standard library are utilized for file handling, process management, and error handling to ensure reliability and portability.


### Example Usage

The program will compile and execute each student's program, compare the output with expected results, and generate a `results.csv` file containing scores and reasons for each submission.

![image](https://github.com/yeela8g/Assignments-Evaluator/assets/118124478/e08bf4bf-8a55-44b0-ac01-fade8518a260)

By using this repository you can efficiently evaluate C program submissions with minimal manual effort.ENJOY!
