#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <cstring>

#include "service/ta.hpp"
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

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cout << "Usage: ./app <num_TAs>\n";
        return 0;
    }

    int numTA = atoi(argv[1]);
    if (numTA < 1) {
        cout << "Must have at least 1 TA!\n";
        return 0;
    }

    int shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    SharedData* data = (SharedData*)shmat(shmid, nullptr, 0);
    data->exam_loaded = 0;

    loadRubric(data);

    for (int i = 1; i <= numTA; i++) {
        if (fork() == 0) {
            TA_process(i, data);
            exit(0);
        }
    }

    int examIndex = 1;
    while (true) {
        while (data->exam_loaded == 1) {
            usleep(100000);
        }

        if (!loadExam(data, examIndex)) break;
        examIndex++;
    }

    data->student_id = 9999;
    data->exam_loaded = 1;

    cout << "[MAIN] All exams finished. Sending termination signal (9999).\n";

    for (int i = 0; i < numTA; i++) {
        wait(NULL);
    }

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    cout << "[MAIN] Clean exit.\n";
    return 0;
}

