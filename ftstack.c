#include <stdlib.h>
#include <stdio.h>
#include "ftstack.h"

struct ftstack *ftstack_create(unsigned short d)
{
#ifdef FTSTACK_DEBUG
	fprintf(stderr, "creating ftstack (d=%d)\n", d);
#endif

	struct ftstack *fts = malloc(sizeof(struct ftstack));
	if (fts == NULL) {
		ftstack_error("out of memory");
		return NULL;
	}

	fts->d = d;
	fts->size = 0;
	fts->head = NULL;
	fts->handles = malloc(sizeof(struct ftstack *) * 2 * d);

	int n;
	for (n = 0; n < 2 * d; n++)
		fts->handles[n] = NULL;

	return fts;
}

void ftstack_destroy(struct ftstack *fts)
{
	if (fts == NULL) {
		ftstack_error("invalid reference");
		return;
	}
}

inline unsigned int ftstack_layer(struct ftstack *fts, unsigned int index)
{
	if (index <= ((2 * fts->d) - 1)) {
		return 0;
	}
	return (index / (2 * fts->d));
}

inline unsigned int ftstack_slice(struct ftstack *fts, unsigned int index)
{
	return (index % (fts->d * 2));
}

struct ftstack_node *ftstack_node_create(void *data, struct ftstack_node *straight, struct ftstack_node *diagonal)
{
	struct ftstack_node *ftsn = malloc(sizeof(struct ftstack_node));
	if (ftsn == NULL) {
		ftstack_error("out of memory");
		return NULL;
	}

	ftsn->data = data;
	ftsn->straight = straight;
	ftsn->diagonal = diagonal;

	return ftsn;
}

void ftstack_node_destroy(struct ftstack_node *ftsn)
{
	if (ftsn == NULL) {
		ftstack_error("invalid reference");
		return;
	}
}

struct ftstack_node *ftstack_head(struct ftstack *fts)
{
	if (fts == NULL) {
		ftstack_error("invalid reference");
		return;
	}

	return fts->head;
}

/** returns NULL if node does not exist
 */
struct ftstack_node *ftstack_get_coord(struct ftstack *fts, unsigned int i, unsigned int j)
{
#ifdef FTSTACK_DEBUG
	fprintf(stderr, "\n===== searching 0x%08llx for node at (%d,%d) =====\n", fts, i, j);
#endif
	if (fts == NULL) {
		ftstack_error("invalid reference");
		return NULL;
	}

	if (i < 0 || j < 0) {
		return NULL;
	}

	/* traverse through a slice of layers layers */
	struct ftstack_node *node = fts->handles[j];

	/* slice layer depends on which slice the head is on */
	unsigned int head_layer = ftstack_layer(fts, fts->size - 1);
	unsigned int head_slice = ftstack_slice(fts, fts->size - 1);
	unsigned int slice_layer;
       	if (head_layer == 0) {
		slice_layer = 0;
	} else {
		if (j <= head_slice)
			slice_layer = head_layer;
		else
			slice_layer = head_layer - 1;
	}

	int n;
	for (n = slice_layer; n > i; n--) {
		if (node == NULL)
			break;

		node = node->straight;
	}

#ifdef FTSTACK_DEBUG
	fprintf(stderr, "returning 0x%08llx\n", node);
#endif
	return node;
}

/** returns 0 on error and 1 on success
 */
unsigned int ftstack_push(struct ftstack *fts, void *data)
{
	if (fts == NULL) {
		ftstack_error("invalid reference");
		return 0;
	}

	unsigned int new_layer = ftstack_layer(fts, fts->size);
	unsigned int new_slice = ftstack_slice(fts, fts->size);
	unsigned int diagonal_slice = new_slice ^ (1 << (new_layer % (2 * fts->d)));

	struct ftstack_node *straight;
	struct ftstack_node *diagonal;
	if (new_layer > 0) {
		straight = ftstack_get_coord(fts, new_layer - 1, new_slice);
		diagonal = ftstack_get_coord(fts, new_layer - 1, diagonal_slice);
	} else {
		straight = NULL;
		diagonal = NULL;
	}

	struct ftstack_node *ftsn = ftstack_node_create(data, straight, diagonal);

	fts->head = ftsn;
	fts->size++;

	/* update handle */
	fts->handles[new_slice] = ftsn;

#ifdef FTSTACK_DEBUG
	fprintf(stderr, "\npush (0x%08llx): size=%d / layer=%d / slice=%d / diagonal=%d\n", ftsn, fts->size, new_layer, new_slice, diagonal_slice);
#endif
}

/** returns 0 on error and 1 on success
 */
unsigned int ftstack_pop(struct ftstack *fts)
{
	if (fts == NULL) {
		ftstack_error("invalid reference");
		return 0;
	}

	if (fts->size == 0)
		return 0;

	unsigned int old_layer = ftstack_layer(fts, fts->size - 1);
	unsigned int old_slice = ftstack_slice(fts, fts->size - 1);

	unsigned int new_layer = ftstack_layer(fts, fts->size - 2);
	unsigned int new_slice = ftstack_slice(fts, fts->size - 2);

	struct ftstack_node *old_head = ftstack_head(fts);
	struct ftstack_node *new_head = ftstack_get_coord(fts, new_layer, new_slice);

	/* update handle */
	if (old_layer == 0) {
		fts->handles[old_slice] = NULL;
	} else {
		fts->handles[old_slice] = old_head->straight;
	}

	fts->head = new_head;
	fts->size--;

#ifdef FTSTACK_DEBUG
	fprintf(stderr, "\npop  (0x%08llx): size=%d / old_layer=%d / old_slice=%d / new_layer=%d / new_slice=%d\n", old_head, fts->size, old_layer, old_slice, new_layer, new_slice);
#endif
}

void ftstack_node_print(struct ftstack_node *ftsn)
{
	printf("\n===== ftstack_node (0x%08llx) =====\n", ftsn);

	if (ftsn != NULL) {
		printf("straight -> 0x%08llx / diagonal -> 0x%08llx\n", ftsn->straight, ftsn->diagonal);
	}
}

void ftstack_print(struct ftstack *fts)
{
	printf("\n===== ftstack (0x%08llx) =====\n", fts);
	printf("head -> 0x%08llx\n", fts->head);

	struct ftstack_node *ftsn = ftstack_head(fts);
	ftstack_node_print(ftsn);

	fprintf(stderr, "\n");
	int n;
	for (n = 0; n < 2 * fts->d; n++) {
		printf("handle[%d] -> 0x%08llx\n", n, fts->handles[n]);
	}
}

int main(void)
{
	struct ftstack *fts = ftstack_create(4);
	ftstack_print(fts);

	int num_elements = 100;

	int n;

	/* push elements onto stack */
	for (n = 0; n <= num_elements - 1; n++) {
		int *data = malloc(sizeof(int));
		*data = n;
		ftstack_push(fts, data);
	}
	ftstack_print(fts);

	/* test for simple connectivity */
	ftstack_get_coord(fts, 0, 0);

	unsigned int correct = 0;
	unsigned int incorrect = 0;

	/* pop and check contents of elements */
	for (n = num_elements - 1; n >= 0; n--) {
		struct ftstack_node *node = ftstack_head(fts);
		ftstack_node_print(node);
		int node_data = *((int *)node->data);
		if (node_data == n) {
			printf("\ncorrect result for element %d (0x%08llx)\n", n, node);
			correct++;
		} else {
			printf("\nINCORRECT result for element %d (got %d from 0x%08llx)\n", n, node_data, node);
			incorrect++;
		}
		ftstack_pop(fts);
	}
	ftstack_print(fts);

	printf("\nresults: %d/%d correct\n", correct, correct + incorrect);

	return 0;
}
