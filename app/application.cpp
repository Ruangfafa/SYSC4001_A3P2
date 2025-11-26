#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <cstring>

#include "service/ta.hpp"
#include "service/loader.hpp"
using namespace std;

int sem_rubric;
int sem_qstatus;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static void P(int semid) {
    sembuf op{0, -1, 0};
    semop(semid, &op, 1);
}

static void V(int semid) {
    sembuf op{0, 1, 0};
    semop(semid, &op, 1);
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
    data->active_TA_count = 0;

    semun arg;

    sem_rubric = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_rubric < 0) {
        perror("semget sem_rubric");
        exit(1);
    }
    arg.val = 1;
    semctl(sem_rubric, 0, SETVAL, arg);

    sem_qstatus = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_qstatus < 0) {
        perror("semget sem_qstatus");
        exit(1);
    }
    arg.val = 1;
    semctl(sem_qstatus, 0, SETVAL, arg);
    for (int i = 1; i <= numTA; i++) {
        if (fork() == 0) {
            TA_process(i, data);
            exit(0);
        }
    }

    int examIndex = 1;
    while (true) {
        while (data->active_TA_count > 0 || data->exam_loaded == 1) {
            P(sem_qstatus);
            if (data->student_id == 9999 && data->active_TA_count <= 0) {
                data->exam_loaded = 0;
            }
            V(sem_qstatus);
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

    semctl(sem_rubric, 0, IPC_RMID);
    semctl(sem_qstatus, 0, IPC_RMID);

    cout << "[MAIN] Clean exit.\n";
    return 0;
}

