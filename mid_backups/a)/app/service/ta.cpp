#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "ta.hpp"

using namespace std;

void TA_process(int id, SharedData* data) {

    srand(getpid());

    while (true) {

        while (!data->exam_loaded) {
            usleep(100000);
        }

        if (data->student_id == 9999) {
            cout << "[TA " << id << "] Received termination exam. Exiting.\n";
            exit(0);
        }

        int sid = data->student_id;

        cout << "[TA " << id << "] ----- Start marking exam of student " 
             << sid << " -----" << endl;

        cout << "[TA " << id << "] Checking rubric..." << endl;

        for (int i = 0; i < 5; i++) {

            int delay_ms = (rand() % 500 + 500);
            usleep(delay_ms * 1000);

            if (rand() % 10 == 0) {
                data->rubric[i][2] += 1;
                cout << "[TA " << id << "] Corrected rubric line "
                     << (i + 1) << " → " << data->rubric[i] << endl;
            }
        }

        cout << "[TA " << id << "] Marking exam..." << endl;

        for (int q = 0; q < 5; q++) {

            if (data->question_status[q] == 0) {

                data->question_status[q] = 1;

                int delay_ms = (rand() % 1000 + 1000);
                usleep(delay_ms * 1000);

                cout << "[TA " << id << "] Marked Q" << q + 1
                     << " for student " << sid << endl;
            }
        }

        cout << "[TA " << id << "] Finished exam " << sid 
             << ". Waiting for next exam..." << endl;

        data->exam_loaded = 0;
    }
}

