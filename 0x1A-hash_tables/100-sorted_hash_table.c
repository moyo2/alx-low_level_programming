#include "hash_tables.h"

/*
  Returns a pointer to the hash table created, or NULL if malloc failed.
 */
shash_table_t *shash_table_create(unsigned long int size)
{
	unsigned int i = 0;
	shash_table_t *ht = malloc(sizeof(shash_table_t));

	if (ht == NULL)
		return (NULL);
	ht->size = size;
	ht->shead = NULL;
	ht->stail = NULL;
	ht->array = malloc(sizeof(shash_node_t *) * size);
	if (ht->array == NULL)
	{
		free(ht);
		return (NULL);
	}
	for (; i < size; i++)
		ht->array[i] = NULL;
	return (ht);
}

/*
Return pointer to the new node.
 */
shash_node_t *set_spair(const char *key, const char *value)
{
	shash_node_t *node = malloc(sizeof(shash_node_t));

	if (node == NULL)
		return (NULL);
	node->key = malloc(strlen(key) + 1);
	if (node->key == NULL)
		return (NULL);
	node->value = malloc(strlen(value) + 1);
	if (node->value == NULL)
		return (NULL);
	strcpy(node->key, key);
	strcpy(node->value, value);
	return (node);
}

/*
(no collision) set key:value pair to first array element
Returns the node, or NULL if failed.
 */
shash_node_t *set_spair_only(shash_table_t *ht, const char *key,
			     const char *value, unsigned long int index)
{
	shash_node_t *node = set_spair(key, value);

	if (node == NULL)
		return (NULL);
	node->next = NULL;
	ht->array[index] = node;
	return (node);
}

/*
update_value - updates the value of a given node.
Returns 1 on success, 0 on fail.
 */
int update_value(shash_node_t *node, const char *value)
{
	if (strcmp(node->value, value) == 0)
		return (1);
	free(node->value);
	node->value = malloc(strlen(value) + 1);
	if (node->value == NULL)
		return (0);
	strcpy(node->value, value);
	return (1);
}

/*
sets key:value pair node to front of given index's list.
Returns new node, or NULL if failed.
 */
shash_node_t *set_spair_front(shash_table_t *ht, const char *key,
			     const char *value, unsigned long int index)
{
	shash_node_t *node = set_spair(key, value);

	if (node == NULL)
		return (0);
	node->next = ht->array[index];
	ht->array[index] = node;
	return (node);
}

/*
sets the first addition to the sorted list.
 */
int slist_set_first(shash_table_t *ht, shash_node_t *node)
{
	node->sprev = NULL;
	node->snext = NULL;
	ht->shead = node;
	ht->stail = node;
	return (1);
}

/*
 sets a new node before the given old node.
Return always 1 (success).
 */
int slist_set(shash_table_t *ht, shash_node_t *old_node, shash_node_t *node)
{
	if (old_node->sprev == NULL)
		ht->shead = node;
	node->snext = old_node;
	node->sprev = old_node->sprev;
	old_node->sprev = node;
	if (node->sprev != NULL)
		node->sprev->snext = node;
	return (1);
}

/*
sets a node at the end of the sorted list
Return Always 1 (success).
 */
int slist_set_end(shash_table_t *ht, shash_node_t *node)
{
	shash_node_t *old_node = ht->stail;

	ht->stail = node;
	node->snext = NULL;
	node->sprev = old_node;
	old_node->snext = node;
	return (1);
}

/*
adds or updates an element to the sorted hash table.
Returns 1 on success, 0 on error.
 */
int shash_table_set(shash_table_t *ht, const char *key, const char *value)
{
	unsigned long int index;
	shash_node_t *node, *curr_old_node;

	if (key == NULL || ht == NULL)
		return (0);
/* set the item in the table */
	index = key_index((unsigned char *)key, ht->size);
	node = ht->array[index];
	if (node == NULL)
		node = set_spair_only(ht, key, value, index);
	else
	{
		while (node != NULL)
		{
			if (strcmp(node->key, key) == 0)
				return (update_value(node, value));
			node = node->next;
		}
		node = set_spair_front(ht, key, value, index);
	}
	if (node == NULL)
		return (0);
/* arrange the item in the sorted linked list */
	if (ht->shead == NULL)
		return (slist_set_first(ht, node));
	curr_old_node = ht->shead;
	while (curr_old_node != NULL)
	{
		if (strcmp(curr_old_node->key, node->key) >= 0)
			return (slist_set(ht, curr_old_node, node));
		curr_old_node = curr_old_node->snext;
	}
	return (slist_set_end(ht, node));
}

/*
retrieves a value associated with a key.
Returns the value associated with the key, or NULL if key can't be found.
 */
char *shash_table_get(const shash_table_t *ht, const char *key)
{
	unsigned long int index;
	shash_node_t *node;

	if (ht == NULL)
		return (NULL);
	if (key == NULL)
		return (NULL);
	index = key_index((unsigned char *)key, ht->size);
	if (ht->array[index] == NULL)
		return (NULL);
	if (strcmp(ht->array[index]->key, key) == 0)
		return (ht->array[index]->value);
	node = ht->array[index];
	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
			return (node->value);
		node = node->next;
	}
	return (NULL);
}

/*
 prints key/value pairs in sorted hash table order.

 */
void shash_table_print(const shash_table_t *ht)
{
	unsigned long int count = 0;
	shash_node_t *node = ht->shead;

	if (ht == NULL)
		return;
	printf("{");
	while (node != NULL)
	{
		if (count > 0)
			printf(", ");
		printf("'%s': '%s'", node->key, node->value);
		node = node->snext;
		count++;
	}
	printf("}\n");
}

/*
prints sorted key/value pairs in reverse.
 
 */
void shash_table_print_rev(const shash_table_t *ht)
{
	unsigned long int count = 0;
	shash_node_t *node = ht->stail;

	if (ht == NULL)
		return;
	printf("{");
	while (node != NULL)
	{
		if (count > 0)
			printf(", ");
		printf("'%s': '%s'", node->key, node->value);
		node = node->sprev;
		count++;
	}
	printf("}\n");
}

/*
shash_table_delete - deletes a sorted hash table

 */
void shash_table_delete(shash_table_t *ht)
{
	unsigned long int i = 0;
	shash_node_t *node;

	while (i < ht->size)
	{
		while (ht->array[i] != NULL)
		{
			node = ht->array[i]->next;
			free(ht->array[i]->key);
			free(ht->array[i]->value);
			free(ht->array[i]);
			ht->array[i] = node;
		}
		i++;
	}
	free(ht->array);
	free(ht);
}

