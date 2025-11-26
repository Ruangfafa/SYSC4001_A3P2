#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/sem.h>
#include "ta.hpp"

using namespace std;

static void P(int semid) {
    sembuf op{0, -1, 0};
    semop(semid, &op, 1);
}

static void V(int semid) {
    sembuf op{0, 1, 0};
    semop(semid, &op, 1);
}

void TA_process(int id, SharedData* data) {

    srand(getpid());

    while (true) {

        while (!data->exam_loaded) {
            usleep(100000);
        }

        if (data->student_id == 9999) {
            cout << "[TA " << id << "] Received termination exam. Exiting.\n";
            _exit(0);
        }

        int sid = data->student_id;

        cout << "[TA " << id << "] ----- Start marking exam of student "
             << sid << " -----" << endl;

        cout << "[TA " << id << "] Checking rubric..." << endl;

        for (int i = 0; i < 5; i++) {
            int delay_ms = (rand() % 500 + 500);
            usleep(delay_ms * 1000);

            if (rand() % 10 == 0) {
                P(sem_rubric);
                data->rubric[i][2] += 1;
                cout << "[TA " << id << "] Corrected rubric line "
                     << (i + 1) << " → " << data->rubric[i] << endl;
                V(sem_rubric);
            }
        }

        cout << "[TA " << id << "] Marking exam..." << endl;

        while (true) {
            int chosen_q = -1;

            P(sem_qstatus);
            for (int q = 0; q < 5; q++) {
                if (data->question_status[q] == 0) {
                    data->question_status[q] = 1;
                    chosen_q = q;
                    data->active_TA_count++;
                    break;
                }
            }
            V(sem_qstatus);

            if (chosen_q == -1) {
                usleep(100000);

                P(sem_qstatus);
                if (data->active_TA_count == 0) {
                    data->exam_loaded = 0;
                    cout << "[TA " << id << "] All questions done for student "
                        << sid << ". Signaling exam complete.\n";
                    V(sem_qstatus);
                    break;
                }
                V(sem_qstatus);

                continue;
            }

            int delay_ms = (rand() % 1000 + 1000);
            usleep(delay_ms * 1000);

            if (data->student_id != sid || data->exam_loaded == 0) {
                P(sem_qstatus);
                data->active_TA_count--;
                V(sem_qstatus);
                continue;
            }

            cout << "[TA " << id << "] Marked Q" << (chosen_q + 1)
                << " for student " << sid << endl;

            P(sem_qstatus);
            data->active_TA_count--;
            V(sem_qstatus);
        }

        cout << "[TA " << id << "] Finished exam " << sid
             << ". Waiting for next exam..." << endl;
    }
}
