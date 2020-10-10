from insertion_sort import insertion_sort


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
        print(f'sorting {array[start:middle]}')
        if middle - start > k:
            merge_sort(array, k, start, middle)
        else:
            insertion_sort(array, start, middle)
        print(f'sorting {array[middle:end]}')
        if end - middle > k:
            merge_sort(array, k, middle, end)
        else:
            insertion_sort(array, middle, end)

        array[start:end] = merge(array, start, middle, end)


if __name__ == '__main__':
    array = [54, 26, 93, 17, 77, 31, 44, 55, 20]
    merge_sort(array, 1, 0, len(array))
    print(array)
