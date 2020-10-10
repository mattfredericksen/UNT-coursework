from sorts.insertion_sort import insertion_sort


def merge(array, start, middle, end):
    # Two indexes for traversing the two halves
    i = start
    j = middle

    # temporary array for sorting
    merged_array = []

    while i < middle and j < end:
        if array[i] < array[j]:
            # The value from the left half has been used
            merged_array.append(array[i])
            # Move the iterator forward
            i += 1
        else:
            merged_array.append(array[j])
            j += 1

    # For all the remaining values
    while i < middle:
        merged_array.append(array[i])
        i += 1

    while j < end:
        merged_array.append(array[j])
        j += 1

    return merged_array


# Adapted from https://www.educative.io/edpresso/merge-sort-in-python
def merge_sort(array, k, start, end):
    if end - start > 1:
        middle = (end - start)//2 + start

        # Recursive call on each half
        if middle - start > k:
            merge_sort(array, k, start, middle)
        else:
            insertion_sort(array, start, middle)
        if end - middle > k:
            merge_sort(array, k, middle, end)
        else:
            insertion_sort(array, middle, end)

        array[start:end] = merge(array, start, middle, end)


if __name__ == '__main__':
    # driver code to test above
    from random import randint

    for _ in range(500):
        arr = [randint(-20, 20) for _ in range(randint(0, 50))]
        copy = arr.copy()
        merge_sort(arr, 10, 0, len(arr))
        if arr != sorted(copy):
            print('merge_sort failed with array:')
            print(copy)
    print('merge_sort test success')
