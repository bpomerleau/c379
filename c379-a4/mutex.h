/**
* mutex.h
*
* developed for CMPUT379 Assignment 4
*
* author: Brady Pomerleau  -- bpomerle@ualberta.ca
*
*
*/

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

void init_mutex(pthread_mutex_t *mutex);
void lock(pthread_mutex_t *mutex);
void unlock(pthread_mutex_t *mutex);

#endif
