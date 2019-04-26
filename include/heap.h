struct Node {
    struct Node *lc, *rc;
    int dist, value, size, index;
};

int get_size(struct Node *o);

struct Node* insert_heap(struct Node* root, int value, int index); 

struct Node* pop_heap();
