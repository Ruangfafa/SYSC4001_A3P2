#include <iostream>
#include <fstream>
#include <cstring>
#include "loader.hpp"

using namespace std;

void loadRubric(SharedData* data) {
    ifstream file("resources/rubric.txt");
    if (!file.is_open()) {
        perror("Error opening resources/rubric.txt");
        exit(1);
    }

    for (int i = 0; i < 5; i++) {
        file.getline(data->rubric[i], 10);
    }
    file.close();
    cout << "[MAIN] Loaded rubric into shared memory.\n";
}

bool loadExam(SharedData* data, int index) {
    char filename[64];
    sprintf(filename, "resources/exams/exam_%02d.txt", index);

    ifstream file(filename);
    if (!file.is_open()) return false;

    file >> data->student_id;
    file.close();

    memset(data->question_status, 0, sizeof(data->question_status));
    data->exam_loaded = 1;

    cout << "[MAIN] Loaded " << filename << " (student = "
         << data->student_id << ")\n";
    return true;
}
