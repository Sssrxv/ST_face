#ifndef __XLIST_H__
#define __XLIST_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/*  Macro and struct definitions                                             */
/* --------------------------------------------------------------------------*/

#define INIT_LIST_HEAD(head) list_init(head)

struct list_head {
    struct list_head *prev, *next;
};

/* Get an entry from the list
 *  ptr - the address of this list_head element in "type"
 *  type - the data type that contains "member"
 *  member - the list_head element in "type"
 */
#define list_entry(ptr, type, member)                           \
    ((type *)((uintptr_t)(ptr) - (uintptr_t)offsetof(type, member)))

/* Get the first element of the list */
#define list_first_entry(ptr, type, member)                     \
    list_entry((ptr)->next, type, member)

/* Get each entry from a list, 
 *  TODO: use 'list_for_each_entry_safe' if you will delete something while interating
 *  pos - A structure pointer has a "member" element
 *  head - list head
 *  member - the list_head element in "pos"
 *  type - the type of the first parameter
 */
#define list_for_each_entry(pos, head, member, type)            \
    for (pos = list_entry((head)->next, type, member);          \
         &pos->member != (head);                                \
         pos = list_entry(pos->member.next, type, member))

/* Get each entry from a list, it is safe if you delete something while interating
 *  pos - A structure pointer has a "member" element
 *  n - point to the next structure of 'pos'
 *  head - list head
 *  member - the list_head element in "pos"
 *  type - the type of the first parameter
 */
#define list_for_each_entry_safe(pos, n, head, member, type)    \
    for (pos = list_entry((head)->next, type, member),          \
         n = list_entry(pos->member.next, type, member);        \
         &pos->member != (head);                                \
         pos = n, n = list_entry(n->member.next, type, member))

#define list_empty(entry) ((entry)->next == (entry))

/* --------------------------------------------------------------------------*/
/** 
 * @brief  list_init 
 * 
 * @param entry, list head
 */
/* --------------------------------------------------------------------------*/
static inline void list_init(struct list_head *entry)
{
    entry->prev = entry->next = entry;
}

/* --------------------------------------------------------------------------*/
/** 
 * @brief  list_add, list add before head node
 * 
 * @param entry  entry node to be added
 * @param head   list head
 */
/* --------------------------------------------------------------------------*/
static inline void list_add(struct list_head *entry, struct list_head *head)
{
    entry->next = head->next;
    entry->prev = head;

    head->next->prev = entry;
    head->next = entry;
}

/* --------------------------------------------------------------------------*/
/** 
 * @brief  list_add_tail, list add tail after head node
 * 
 * @param entry  entry node to be added
 * @param head   list head
 */
/* --------------------------------------------------------------------------*/
static inline void list_add_tail(struct list_head *entry,
    struct list_head *head)
{
    entry->next = head;
    entry->prev = head->prev;

    head->prev->next = entry;
    head->prev = entry;
}

/* --------------------------------------------------------------------------*/
/** 
 * @brief  list_del, delete one entry node
 * 
 * @param entry
 */
/* --------------------------------------------------------------------------*/
static inline void list_del(struct list_head *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    entry->next = entry->prev = NULL;
}

/* --------------------------------------------------------------------------*/
/** 
 * @brief  list_cut, cut the 'head', the 'list' will be the new head
 * 
 * @param list  the new list header
 * @param head  the previous list header, it will be re-init to point to itself
 */
/* --------------------------------------------------------------------------*/
static inline void list_cut(struct list_head *list, struct list_head *head)
{
    if (list_empty(head))
        return;

    list->next = head->next;
    list->next->prev = list;
    list->prev = head->prev;
    list->prev->next = list;

    list_init(head);
}

/* --------------------------------------------------------------------------*/
/** 
 * @brief  list_del_init , delete entry from list and reinitialize it
 * 
 * @param entry
 */
/* --------------------------------------------------------------------------*/
static inline void list_del_init(struct list_head *entry)
{
    list_del(entry);
    INIT_LIST_HEAD(entry);
}

/* Example code */
#if 0
void test_xlist()
{
    struct ctx {
        int ctx_num;
        struct list_head work_list;
        struct list_head free_list;
    };

    struct dev {
        int num;
        struct list_head list;
    };

    struct ctx cur_ctx;
    struct dev dev0, dev1, dev2;

    cur_ctx.ctx_num = 0;

    dev0.num = 0;
    dev1.num = 1;
    dev2.num = 2;

    list_init(&cur_ctx.work_list);
    list_init(&cur_ctx.free_list);

    list_add_tail(&dev0.list, &cur_ctx.free_list);
    list_add_tail(&dev1.list, &cur_ctx.free_list);
    list_add_tail(&dev2.list, &cur_ctx.free_list);

    struct dev *cur, *next;
    
    // move all devs in free_list into work_list
    list_for_each_entry_safe(cur, next, &cur_ctx.free_list, list, struct dev) {
        fprintf(stderr, "free_list: %d.\n", cur->num);
        list_del(&cur->list);
        list_add_tail(&cur->list, &cur_ctx.work_list);
    }

    cur = list_first_entry(&cur_ctx.work_list, struct dev, list);
    fprintf(stderr, "work_list: %d.\n", cur->num);
    list_del(&cur->list);
    
    ASSERT(list_empty(&cur_ctx.free_list));

    list_for_each_entry(cur, &cur_ctx.work_list, list, struct dev) {
        fprintf(stderr, "work_list: %d.\n", cur->num);
    }
    
}
#endif // #if 0

#ifdef __cplusplus
}
#endif


#endif  //__XLIST_H__
