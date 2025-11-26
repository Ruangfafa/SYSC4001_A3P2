# Exam Grading Simulation

This project simulates multiple teaching assistants (TAs) concurrently grading student exams using shared memory and semaphores in C++.

## Build Instructions

To compile the program, run:

```g++ app/application.cpp app/service/ta.cpp app/service/loader.cpp -o run```


This will generate an executable named run.

## Generate Resources (Rubric + Exams)

Before running the program, generate the exam files and rubric automatically using the script included in the app/ directory.

```
cd app/
chmod +x generate_resources.sh
./generate_resources.sh
```

This will create:

resources/rubric.txt

resources/exams/exam_xx.txt (a set of randomly generated exam files)

## Run the Program

You can execute the program and specify the number of TAs (e.g., 2, 3, 5):

./run 2


Example usage:

./run 3
./run 5