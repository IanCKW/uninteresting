#include "restaurant.h"
#include <pthread.h>
#include <semaphore.h>
#include <sys/queue.h>

sem_t one_seat, two_seat, three_seat, four_seat, five_seat;

TAILQ_ENTRY(id_node) id_node_q;
struct id_node{
    TAILQ_ENTRY(id_node) id_node_q;
    int id;
};

TAILQ_ENTRY(m_node) mutex_node_q;
struct m_node{
    TAILQ_ENTRY(mutex_node) mutex_node_q;
    sem_t mutex;
};

TAILQ_HEAD(idq, id_node);
TAILQ_HEAD(mq, m_node);

struct idq one_id_q;
struct mq one_m_q;

struct idq two_id_q;
struct mq two_m_q;

struct idq three_id_q;
struct mq three_m_q;

struct idq four_id_q;
struct mq four_m_q;

struct idq five_id_q;
struct mq five_m_q;

void restaurant_init(int num_tables[5]) {
    sem_init(&one_seat, 0,1);
    sem_init(&two_seat, 0,1);
    sem_init(&three_seat, 0,1);
    sem_init(&four_seat, 0,1);
    sem_init(&five_seat, 0,1);

    TAILQ_INIT(&one_id_q);
    TAILQ_INIT(&one_m_q);

    TAILQ_INIT(&two_id_q);
    TAILQ_INIT(&two_m_q);

    TAILQ_INIT(&three_id_q);
    TAILQ_INIT(&three_m_q);

    TAILQ_INIT(&four_id_q);
    TAILQ_INIT(&four_m_q);

    TAILQ_INIT(&five_id_q);
    TAILQ_INIT(&five_m_q);
    
    int count = 0;


    ////ONE////
    for (int i= 0; i<num_tables[0];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&one_id_q, n1, id_node_q);
    }
    count += num_tables[0];
    // while(!TAILQ_EMPTY(&one_id_q)){
    //     struct id_node *p;
    //     p = malloc(sizeof(struct id_node));
    //     p = TAILQ_FIRST(&one_id_q);
    //     printf("%d",p->id);
    //     TAILQ_REMOVE(&one_id_q, p, id_node_q);
    // }

    ////TWO////
    for (int i= 0; i<num_tables[1];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&two_id_q, n1, id_node_q);
    }
    count += num_tables[1];

    ////THREE////
    for (int i= 0; i<num_tables[2];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&three_id_q, n1, id_node_q);
    }
    count += num_tables[2];

    ////FOUR////
    for (int i= 0; i<num_tables[3];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&four_id_q, n1, id_node_q);
    }
    count += num_tables[3];

    ////FIVE////
    for (int i= 0; i<num_tables[4];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&five_id_q, n1, id_node_q);
    }
    count += num_tables[4];

}

void restaurant_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    // TODO
}

int request_for_table(group_state *state, int num_people) {
    int *mutex;
    struct idq *curr_idq;
    struct mq *curr_mq;
    if (num_people == 1){
        mutex = &one_seat;
        curr_idq = &one_id_q;
        curr_mq = &one_m_q;
    }
    else if (num_people == 2){
        mutex = &two_seat;
        curr_idq = &two_id_q;
        curr_mq = &two_m_q;
    }
    else if (num_people == 3){
        mutex = &three_seat;
        curr_idq = &three_id_q;
        curr_mq = &three_m_q;
    }
    else if (num_people == 4){
        mutex = &four_seat;
        curr_idq = &four_id_q;
        curr_mq = &four_m_q;
    }
    else{
        mutex = &five_seat;
        curr_idq = &five_id_q;
        curr_mq = &five_m_q;
    }

    sem_wait(mutex);    // crit start
    state->num_people = num_people;                             // modify state
    state->table_id = -1;
    on_enqueue();

    if (!TAILQ_EMPTY(curr_idq)) {
        struct id_node *p;
        p = malloc(sizeof(struct id_node));
        p = TAILQ_FIRST(curr_idq);                              //dequeue
        TAILQ_REMOVE(curr_idq, p, id_node_q);
        state->table_id = p->id;

        sem_post(mutex);
        return state->table_id;
    }
    else {
        sem_t thread_mutex;                                     // make x
        sem_init(&thread_mutex, 0,0);                           // don't let it go yet
        struct m_node *thread_node;                             // make node
        thread_node = malloc(sizeof(struct m_node));
        thread_node->mutex = thread_mutex;  
        TAILQ_INSERT_TAIL(curr_mq, thread_node, mutex_node_q);   // insert node
        sem_post(mutex);

        // waiting for space in free queue
        sem_wait(&thread_mutex);
        struct id_node *p;
        p = malloc(sizeof(struct id_node));
        p = TAILQ_FIRST(curr_idq);                              //dequeue
        TAILQ_REMOVE(curr_idq, p, id_node_q);
        state->table_id = p->id;
        sem_post(mutex);

        return state->table_id;
    }
}

void leave_table(group_state *state) {
    int num_people = state->num_people;
    int *mutex;
    struct idq *curr_idq;
    struct mq *curr_mq;
    if (num_people == 1){
        mutex = &one_seat;
        curr_idq = &one_id_q;
        curr_mq = &one_m_q;
    }
    else if (num_people == 2){
        mutex = &two_seat;
        curr_idq = &two_id_q;
        curr_mq = &two_m_q;
    }
    else if (num_people == 3){
        mutex = &three_seat;
        curr_idq = &three_id_q;
        curr_mq = &three_m_q;
    }
    else if (num_people == 4){
        mutex = &four_seat;
        curr_idq = &four_id_q;
        curr_mq = &four_m_q;
    }
    else{
        mutex = &five_seat;
        curr_idq = &five_id_q;
        curr_mq = &five_m_q;
    }

    sem_wait(mutex);
    struct id_node *n1;
    n1 = malloc(sizeof(struct id_node));
    n1->id = num_people;
    TAILQ_INSERT_TAIL(curr_idq, n1, id_node_q);

    if (!TAILQ_EMPTY(curr_mq)) {
        struct m_node *m1;
        m1 = malloc(sizeof(struct m_node));
        m1 = TAILQ_FIRST(curr_mq);                              
        TAILQ_REMOVE(curr_mq, m1, mutex_node_q);
        sem_post(m1->mutex);
    }
    else {
        sem_post(mutex);
    }

}