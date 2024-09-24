#ifndef PAIR_H
#define PAIR_H

typedef struct {
    char* key;
    char* value;
} key_value_pair_t;

key_value_pair_t key_value_parse(const char* input);
void key_value_free(key_value_pair_t* pair);

#endif // PAIR_H
