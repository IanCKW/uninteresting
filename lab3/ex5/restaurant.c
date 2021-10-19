#include "restaurant.h"
#include <pthread.h>
#include <semaphore.h>
#include <sys/queue.h>

sem_t u_mutex;

TAILQ_ENTRY(id_node) id_node_q;
struct id_node{
    TAILQ_ENTRY(id_node) id_node_q;
    int id;
};

TAILQ_ENTRY(m_node) mutex_node_q;
struct m_node{
    TAILQ_ENTRY(m_node) mutex_node_q;
    sem_t mutex;
    int num_people;
};

TAILQ_HEAD(idq, id_node);
TAILQ_HEAD(mq, m_node);

struct idq one_id_q;
struct idq two_id_q;
struct idq three_id_q;
struct idq four_id_q;
struct idq five_id_q;

struct mq m_q;

int table_capacity[1000]; //map for tables
struct idq *free_q_ptrs[6];
struct idq *curr_free_q;

void restaurant_init(int num_tables[5]) {
    sem_init(&u_mutex, 0,1);

    TAILQ_INIT(&one_id_q);
    TAILQ_INIT(&two_id_q);
    TAILQ_INIT(&three_id_q);
    TAILQ_INIT(&four_id_q);
    TAILQ_INIT(&five_id_q);
    
    TAILQ_INIT(&m_q);

    struct idq *one_ptr = &one_id_q;
    struct idq *two_ptr = &two_id_q;
    struct idq *three_ptr = &three_id_q;
    struct idq *four_ptr = &four_id_q;
    struct idq *five_ptr = &five_id_q;

    free_q_ptrs[1] = one_ptr;
    free_q_ptrs[2] = two_ptr;
    free_q_ptrs[3] = three_ptr;
    free_q_ptrs[4] = four_ptr;
    free_q_ptrs[5] = five_ptr;

    
    int count = 0;

    ////ONE////
    for (int i= 0; i<num_tables[0];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&one_id_q, n1, id_node_q);
        table_capacity[i + count] = 1;
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
        table_capacity[i + count] = 2;

    }
    count += num_tables[1];

    ////THREE////
    for (int i= 0; i<num_tables[2];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&three_id_q, n1, id_node_q);
        table_capacity[i + count] = 3;        
    }
    count += num_tables[2];

    ////FOUR////
    for (int i= 0; i<num_tables[3];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&four_id_q, n1, id_node_q);
        table_capacity[i + count] = 4;
    }
    count += num_tables[3];

    ////FIVE////
    for (int i= 0; i<num_tables[4];i++){
        struct id_node *n1;
        n1 = malloc(sizeof(struct id_node));
        n1->id = i + count;
        TAILQ_INSERT_TAIL(&five_id_q, n1, id_node_q);
        table_capacity[i + count] = 5;
    }
    count += num_tables[4];

}

void restaurant_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    // TODO
}

int request_for_table(group_state *state, int num_people) {

    sem_wait(&u_mutex);    // crit start
    
    state->table_id = -1;
    state->num_people = num_people;
    on_enqueue();

    for(int i = num_people; i < 6; i++) {                   // check all queues of same sized or larger tables
        if (!TAILQ_EMPTY(free_q_ptrs[i])) {
            struct id_node *p;
            p = malloc(sizeof(struct id_node));
            p = TAILQ_FIRST(free_q_ptrs[i]);                              //dequeue
            TAILQ_REMOVE(free_q_ptrs[i], p, id_node_q);
            state->table_id = p->id;
            sem_post(&u_mutex);
            return state->table_id;
        }
    }
    
    sem_t thread_mutex;                                     // make x
    sem_init(&thread_mutex, 0,0);                           // don't let it go yet
    struct m_node *thread_node;                             // make node
    thread_node = malloc(sizeof(struct m_node));
    thread_node->mutex = thread_mutex;  
    thread_node->num_people = num_people;
    TAILQ_INSERT_TAIL(&m_q, thread_node, mutex_node_q);     // insert mutex into waiting queue
    sem_post(&u_mutex);

    // waiting for space in free queue
    sem_wait(&(thread_node->mutex));
    struct id_node *p;
    p = malloc(sizeof(struct id_node));
    p = TAILQ_FIRST(curr_free_q);                              //dequeue from free tables
    TAILQ_REMOVE(curr_free_q, p, id_node_q);
    state->table_id = p->id;
    sem_post(&u_mutex);

    return p->id;
    
}

void leave_table(group_state *state) {

    sem_wait(&u_mutex);    // crit start
    int table_cap = table_capacity[state->table_id];
    curr_free_q = free_q_ptrs[table_cap];
    struct id_node *n1;                         //add id to free queue
    n1 = malloc(sizeof(struct id_node));
    n1->id = state->table_id;
    TAILQ_INSERT_TAIL(curr_free_q, n1, id_node_q);

    struct m_node *m_node_ptr;
    if (!TAILQ_EMPTY(&m_q)) {                       // if there's any group waiting
        TAILQ_FOREACH(m_node_ptr, &m_q, mutex_node_q) {
            if (m_node_ptr->num_people <= table_cap){
                struct m_node *m1;
                m1 = malloc(sizeof(struct m_node));     //deq from waiting queue
                m1 = TAILQ_FIRST(&m_q);                              
                TAILQ_REMOVE(&m_q, m1, mutex_node_q);
                int *m_add;
                m_add = &(m1->mutex);
                sem_post(m_add);                        //post x
                return;
            }
        }
    }
    sem_post(&u_mutex);                        //post k
    return;

}