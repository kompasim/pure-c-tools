// stack

#ifndef H_PCT_STACK
#define H_PCT_STACK

#include "block.h" // [M[ IGNORE ]M]
#include "cursor.h" // [M[ IGNORE ]M]

typedef struct _Stack {
    struct _Object;
    int size;
    Block *head;
    Block *tail;
    Cursor *cursor;
} Stack;

Stack *Stack_new()
{
    Stack *stack = (Stack *)pct_mallloc(sizeof(Stack));
    Object_init(stack, PCT_OBJ_STACK);
    stack->size = 0;
    stack->head = NULL;
    stack->tail = NULL;
    stack->cursor = Cursor_new(NULL);;
    return stack;
}

void Stack_print(Stack *this)
{
    printf("[(STACK_START) => address:%d]\n", this);
    Block *current = this->head;
    while (current != NULL)
    {
        Block_print(current);
        current = current->next;
    }
    printf("[(STACK_END) => address:%d]\n", this);
}

void Stack_push(Stack *this, void *data)
{
    this->size++;
    Block *block = Block_new(data);
    if (this->head == NULL)
    {
        this->head = block;
    }
    else
    {
        Block_append(this->tail, block);
    }
    this->tail = block;
}

void *Stack_pop(Stack *this)
{
    if (this->tail == NULL)
    {
        this->size = 0;
        return NULL;
    }
    else
    {
        void *data = this->tail->data;
        Block *tail = this->tail;
        if (this->tail == this->head)
        {
            this->size = 0;
            this->head = NULL;
            this->tail = NULL;
        }
        else
        {
            this->size--;
            this->tail = this->tail->last;
            this->tail->next = NULL;
        }
        Object_release(tail);
        return data;
    }
}

void Stack_clear(Stack *this)
{
    void *data = Stack_pop(this);
    while (data != NULL)
    {
        data = Stack_pop(this);
    }
}

void Stack_free(Stack *this)
{
    Block *tail = this->tail;
    while (tail != NULL)
    {
        this->tail = tail->last;
        Object_release(tail);
        tail = this->tail;
    }
    Object_free(this->cursor);
    Object_free(this);
}

void Stack_RESTE(Stack *this) {
    Cursor_set(this->cursor, this->tail);
}

void *Stack_NEXT(Stack *this) {
    Block *temp = Cursor_get(this->cursor);
    if (temp == NULL) return NULL;
    Cursor_set(this->cursor, temp->last);
    return temp->data;
}

Cursor *Stack_reset(Stack *this)
{
    return Cursor_new(this->tail);
}

void *Stack_next(Stack *this, Cursor *cursor)
{
    Block *temp = Cursor_get(cursor);
    if (temp == NULL) return NULL;
    Cursor_set(cursor, temp->last);
    return temp->data;
}

void Stack_reverse(Stack *this)
{
    Queue *queue = Queue_new();
    //
    Cursor *cursor = Stack_reset(this);
    void *data1 = Stack_next(this, cursor);
    while (data1 != NULL)
    {
        Queue_push(queue, data1);
        data1 = Stack_next(this, cursor);
    }
    Cursor_free(cursor);
    Stack_clear(this);
    //
    cursor = Queue_reset(queue);
    void *data = Queue_next(queue, cursor);
    while (data != NULL)
    {
        Stack_push(this, data);
        data = Queue_next(queue, cursor);
    }
    Cursor_free(cursor);
    Object_release(queue);
}


typedef void (*STACK_FOREACH_FUNC)(void *, void *);

void Stack_foreachItem(Stack *this, STACK_FOREACH_FUNC func, void *arg) {
    Cursor *cursor = Stack_reset(this);
    void *ptr = NULL;
    while ((ptr = Stack_next(this, cursor)) != NULL) {
        func(ptr, arg);
    }
    Cursor_free(cursor);
}

#endif
