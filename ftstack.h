#ifndef __FTSTACK_H
#define __FTSTACK_H

#undef FTSTACK_DEBUG

#define ftstack_error(msg) fprintf(stderr, "[%s %s] %s:%u (%s): %s\n", __DATE__, __TIME__, __FILE__, __LINE__, __func__, msg)

struct ftstack_node {
	void *data;
	struct ftstack_node *straight;
	struct ftstack_node *diagonal;
};

struct ftstack {
	unsigned short d;
	unsigned int size;
	struct ftstack_node *head;
	struct ftstack_node **handles;
};

struct ftstack *ftstack_create(unsigned short);
void ftstack_destroy(struct ftstack *);

struct ftstack_node *ftstack_node_create(void *, struct ftstack_node *, struct ftstack_node *);
void ftstack_node_destroy(struct ftstack_node *);

/* stack functions */
struct ftstack_node *ftstack_head(struct ftstack *);
unsigned int ftstack_push(struct ftstack *, void *);
unsigned int ftstack_pop(struct ftstack *);

/* helper functions */
inline unsigned int ftstack_layer(struct ftstack *, unsigned int);
inline unsigned int ftstack_slice(struct ftstack *, unsigned int);
struct ftstack_node *ftstack_get_coord(struct ftstack *, unsigned int, unsigned int);
void ftstack_node_print(struct ftstack_node *);
void ftstack_print(struct ftstack *);


#endif /* __FTSTACK_H */
