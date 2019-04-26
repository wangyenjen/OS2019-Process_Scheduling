#include <stdio.h>
#include <stdlib.h>

struct Node {
    struct Node *lc, *rc;
    int dist, value, size, index;
};

struct Node* new_Node(int value, int index) {
    struct Node *o = (struct Node*)malloc(sizeof(struct Node));
    o->lc = 0;
    o->rc = 0;
    o->dist = 0;
    o->value = value;
    o->size = 1;
    o->index = index;
    return o;
}

#define swap_Node(a, b) do {			\
	struct Node *c = (b);			\
	(b) = (a);				\
	(a) = (c);				\
    } while (0)

int get_size(struct Node *o) {
    return o ? o->size : 0;
}

void maintain(struct Node *o) {
    o->size = get_size(o->lc) + 1 + get_size(o->rc);
}

struct Node* merge(struct Node *a, struct Node *b) {
    if (a == NULL || b == NULL)
        return (a == NULL ? b : a);

    if (b->value < a->value) 
        swap_Node(a, b);
    
    a->size += b->size;
    a->rc = merge(a->rc, b);
    
    if (a->lc == NULL && a->rc) 
        swap_Node(a->lc, a->rc);
    else if (a->rc && a->lc->dist < a->rc->dist)
        swap_Node(a->lc, a->rc);
    
    if (a->rc == 0)
        a->dist = 0;
    else
        a->dist = a->rc->dist + 1;

    maintain(a);
    
    return a;
}

struct Node* insert_heap(struct Node* root, int value, int index) {
    root = merge(root, new_Node(value, index));
    return root;
}

struct Node* pop_heap(struct Node *root) {
    struct Node *tmp = root;
    root = merge(root->lc, root->rc);
    free(tmp);
    return root;
}

int test() {
    struct Node *root = NULL;
    root = merge(root, new_Node(87, 1));
    root = merge(root, new_Node(55, 2));
    root = merge(root, new_Node(96, 3));
    while (root) {
        printf("index = %d, size = %d: %d\n", root->index, root->size, root->value);
        root = merge(root->lc, root->rc);
    }
    return 0;
}
