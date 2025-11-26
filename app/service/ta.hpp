#ifndef TA_HPP
#define TA_HPP

#include "shared.hpp"

extern int sem_rubric;
extern int sem_qstatus;  

void TA_process(int id, SharedData* data);

#endif
