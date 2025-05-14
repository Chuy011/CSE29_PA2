/**
 * CSE 29: Systems Programming and Software Tools
 * Spring Quarter 2025
 * Programming Assignment 2
 *
 * calendar.c
 * @file This file contains the implementation of functions for
scp -o PreferredAuthentications=password -o PubkeyAuthentication=no jelaguna@ieng6.ucsd.edu:/home/linux/ieng6/oce/2a/jelaguna/pa2-Chuy011.tar.gz .* the calendar program that students implement.
 *
 * Author: CSE 30 Spring 2025 PA Team
 * April 2026
 */

/**
 * These are the only imports you are allowed to use.
 * Do not include any other libraries or headers.
 */

#define _GNU_SOURCE // So that you can use strcasestr
#include "calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 2025
#define SUNDAY_MAY4 1746307200
#define MONDAY_MAY5 1746393600
#define TUESDAY_MAY6 1746480000
#define WEDNESDAY_MAY7 1746566400
#define THURSDAY_MAY8 1746652800
#define FRIDAY_MAY9 1746739200
#define SATURDAY_MAY10 1746825600

int lastIdUsed = 0;

time_t testTimes[DAYS_IN_WEEK] = {SUNDAY_MAY4,    MONDAY_MAY5,   TUESDAY_MAY6,
                                  WEDNESDAY_MAY7, THURSDAY_MAY8, FRIDAY_MAY9,
                                  SATURDAY_MAY10};

// Refer to calendar.h for the required behaviors of each function

event_t *makeNewEvent(int id, char *name, time_t start_time, time_t end_time) {
    event_t *newEvent = malloc(sizeof(event_t));
    newEvent->id = id;
    // strdup() uses malloc() implicitly
    newEvent->name = strdup(name);
    newEvent->start_time = start_time;
    newEvent->end_time = end_time;
    newEvent->next = NULL;
    return newEvent;
}

int add_event(week_t *week, time_t start_time, time_t end_time, char *name) {
    // find correct day
    for (int i = 0; i < DAYS_IN_WEEK; ++i) {
        if (!(same_date(start_time, week->days[i].date)))
            continue;

        event_t *curr = week->days[i].events;
        event_t *prev = NULL;

        // iterate through linkedList
        while (curr != NULL) {
            // check for overlap
            // if(start happens after or at end || end happens before or at
            // start)
            if (!(start_time >= curr->end_time || end_time <= curr->start_time))
                return -1;

            if (start_time < curr->start_time)
                break;
            prev = curr;
            curr = curr->next;
        }

        event_t *NEW = makeNewEvent(++lastIdUsed, name, start_time, end_time);

        if (prev == NULL) {
            NEW->next = week->days[i].events;
            week->days[i].events = NEW;
            week->days[i].num_events += 1;
            return lastIdUsed;
        }
        week->days[i].num_events += 1;
        NEW->next = curr;
        prev->next = NEW;
        return lastIdUsed;
    }
    return -1;
}

int remove_event(week_t *week, int id) {
    if (week == NULL)
        return -1;
    for (int i = 0; i < DAYS_IN_WEEK; ++i) {

        // find event
        event_t *curr = week->days[i].events;
        event_t *prev = NULL;

        while (curr != NULL) {
            if (curr->id == id) {
                if (prev == NULL) {
                    week->days[i].events = curr->next;
                    free(curr->name);
                    free(curr);
                    week->days[i].num_events -= 1;
                    return 0;
                }
                prev->next = curr->next;
                free(curr->name);
                free(curr);
                week->days[i].num_events -= 1;
                return 0;
            }
            prev = curr;
            curr = curr->next;
        }
    }
    return -1;
}

int search_event(week_t *week, const char *query, event_t *results[]) {
    if (week == NULL)
        return -1;
    int j = 0;
    for (int i = 0; i < DAYS_IN_WEEK; ++i) {

        // find event
        event_t *curr = week->days[i].events;
        event_t *prev = NULL;

        while (curr != NULL) {
            char *found = strcasestr(curr->name, query);
            if (found == NULL) {
                prev = curr;
                curr = curr->next;
                continue;
            }
            if (j == 10)
                return j;
            results[j++] = curr;
            prev = curr;
            curr = curr->next;
        }
    }
    return j;
}

int reschedule_event(week_t *week, int id, time_t start_time, time_t end_time) {
    for (int i = 0; i < DAYS_IN_WEEK; ++i) {

        // find event
        event_t *curr = week->days[i].events;

        while (curr != NULL) {
            if (curr->id == id) {
                if(same_date(start_time, curr->start_time) == 0) return -1;
                event_t *tmp = week->days[i].events;
                while (tmp != NULL) {
                    if (tmp != curr) {
                        if (!(end_time <= tmp->start_time || start_time >= tmp->end_time)) return -1;
                    }
                    tmp = tmp->next;
                }
                curr->start_time = start_time;
                curr->end_time = end_time;
                return 0;
            }
            curr = curr->next;
        }
        // no id found
    }
    return 1;
}

int duplicate_event(week_t *week, int id, int day) {
    for (int i = 0; i < DAYS_IN_WEEK; ++i) {
        event_t *curr = week->days[i].events;
        event_t *prev = NULL;

        while (curr != NULL) {
            if (curr->id == id) {
                if(i == day) return -1;

                time_t newStartTime =
                    combine_date_time(week->days[day].date, curr->start_time);
                time_t newEndTime =
                    combine_date_time(week->days[day].date, curr->end_time);


                event_t *copy = makeNewEvent(++lastIdUsed, curr->name, newStartTime,
                                             newEndTime);


                event_t *tmp = NULL;
                event_t *findPrev = week->days[day].events;


                while (findPrev != NULL) {

                    if (!(copy->end_time <= findPrev->start_time ||
                        copy->start_time >= findPrev->end_time)) {
                        free(copy);
                        return -1;
                    }

                    if (copy->start_time < findPrev->start_time) {
                        break;
                    }

                    tmp = findPrev;
                    findPrev = findPrev->next;
                }
                if(tmp == NULL) {
                    copy->next = week->days[day].events;
                    week->days[day].events = copy;
                }
                else {
                    copy->next = tmp->next;
                    tmp->next = copy;
                }
                week->days[day].num_events += 1;
                return lastIdUsed;
            }
            prev = curr;
            curr = curr->next;
        }
    }
    return -2;
}

void free_week(week_t *week) {
    for (int i = 0; i < DAYS_IN_WEEK; ++i) {
        event_t *curr = week->days[i].events;
        while (curr != NULL) {
            event_t *tmp = curr->next;
            free(curr->name);
            free(curr);
            curr = tmp;
        }
        week->days[i].events = NULL;
        week->days[i].num_events = 0;
    }
    free(week);
}
