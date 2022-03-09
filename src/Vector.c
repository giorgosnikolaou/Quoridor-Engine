#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "Vector.h"


#define MIN_VECTOR_SIZE 10


struct vector
{
    void** array;
    size_t size;
    size_t capacity;
    DestroyFunc destroy;
};


static void resize(Vector vec)
{
    if (vec->size == vec->capacity)
    {
        size_t new_cap = 2 * vec->capacity;
        void** new_array = realloc(vec->array, new_cap * sizeof(*new_array));
        assert(new_array);

        vec->capacity = new_cap;
        vec->array = new_array;
    }
}


static void swap(void** a, void** b)
{
    void* temp = *a;
    *a = *b;
    *b = temp;
}

static int partition (void** array, int left, int right, CompareFunc compare)
{
    void* pivot = array[right];

    int i = left - 1; 
 
    for (int j = left; j <= right - 1; j++)
    {
        if (compare(array[j], pivot) < 0)
            swap(&array[++i], &array[j]);
        
    }

    swap(&array[i + 1], &array[right]);

    return i + 1;
}

static void quicksort(void** array, int left, int right, CompareFunc compare)
{
    if (left < right)
    {
        int pi = partition(array, left, right, compare);
 
        quicksort(array, left, pi - 1, compare);
        quicksort(array, pi + 1, right, compare);
    }
}


Vector vector_create(size_t size, DestroyFunc destroy)
{
    Vector vec = malloc(sizeof(*vec));
    assert(vec);
    
    vec->destroy = destroy;

    vec->capacity = size > MIN_VECTOR_SIZE ? size : MIN_VECTOR_SIZE;
    vec->size = 0;

    vec->array = calloc(vec->capacity, sizeof(*vec->array));

    return vec;

}


size_t vector_size(Vector vec)
{
    assert(vec);
    return vec->size;
}


void vector_insert(Vector vec, void* value)
{
    assert(vec);

    vector_insert_at(vec, vec->size, value);
}

void vector_insert_sorted(Vector vec, void* value, CompareFunc compare)
{
    assert(vec && value && compare);

    size_t index = -1; // overflow back to 0 after first loop

    while (++index < vec->size)
    {
        if (compare(vec->array[index], value) >= 0)
            break ;
    }

    vector_insert_at(vec, index, value);

}

void vector_insert_at(Vector vec, size_t index, void* value)
{
    
    vec->size++;
    resize(vec);

    for (size_t i = vec->size; i > index; i--)
        vec->array[i] = vec->array[i - 1];
    
    vec->array[index] = value;
    
}


void vector_remove(Vector vec)
{
    assert(vec);
    vector_remove_at(vec, vec->size - 1);
}

void vector_remove_at(Vector vec, size_t ind)
{
    assert(vec && ind < vec->size);
    
    if (vec->destroy)
        vec->destroy(vec->array[ind]);
    
    for (size_t i = ind; i < vec->size - 1; i++)
        vec->array[i] = vec->array[i + 1];
    
    vec->size--;
    vec->array[vec->size] = NULL;
}


void* vector_get_at(Vector vec, size_t ind)
{
    assert(vec && ind < vec->size);
    return vec->array[ind];
}

void* vector_set_at(Vector vec, size_t ind, void* value)
{
    assert(vec && ind < vec->size);
    
    void* old = vec->array[ind];
    vec->array[ind] = value;

    return old;
}


void* vector_find(Vector vec, void* value, CompareFunc compare)
{
    assert(vec && value && compare);

    for (size_t i = 0; i < vec->size; i++)
    {
        if (compare(vec->array[i], value) == 0)
            return vec->array[i];
    }

    return NULL;
}

void* vector_bsearch(Vector vec, void* value, CompareFunc compare)
{
    assert(vec && value && compare);

    int r = 0;
    int l = vec->size - 1;
    
    size_t m = 0;

    while (r <= l)
    {
        m = (r + l) >> 1;

        int comp = compare(vec->array[m], value);
        
        if (comp == 0)
            return vec->array[m];
        else if (comp < 0)
            r = m + 1;
        else
            l = m - 1;
    }

    return NULL;

}

void vector_qsort(Vector vec, CompareFunc compare, size_t start)
{
    assert(vec && compare);

    quicksort(vec->array, start, vec->size - 1, compare);
}


void vector_print(Vector vec, PrintFunc print)
{
    assert(vec && print);

    for (size_t i = 0; i < vec->size; i++)
        print(vec->array[i]);
    
}


void vector_destroy(Vector vec)
{
    for (size_t i = 0; i < vec->size; i++)
    {
        if (vec->destroy)
            vec->destroy(vec->array[i]);
        
    }
    
    free(vec->array);

    free(vec);
}






