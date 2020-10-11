from sorts.insertion_sort import insertion_sort


def partition(array, start, end):
    mid = (start + end) // 2
    array[start], array[mid], array[end - 1] = sorted([array[start], array[mid], array[end - 1]])
    array[mid], array[end - 2] = array[end - 2], array[mid]
    pivot = array[end - 2]

    i = start
    j = end - 3
    while True:
        while array[i] < pivot:
            i += 1
        while array[j] > pivot:
            j -= 1
        if i >= j:
            break
        array[i], array[j] = array[j], array[i]
        i += 1
        j -= 1

    array[i], array[end-2] = array[end-2], array[i]
    return i


def _quick_sort(array, k, start, end):
    # assert k > 2
    if end - start > k:
        p = partition(array, start, end)
        _quick_sort(array, k, start, p)
        _quick_sort(array, k, p + 1, end)
    else:
        insertion_sort(array, start, end)


def quick_sort(array, k):
    _quick_sort(array, k, 0, len(array))


if __name__ == '__main__':
    # driver code to test above
    from random import randint

    for _ in range(500):
        arr = [randint(-20, 20) for _ in range(randint(0, 50))]
        copy = arr.copy()
        quick_sort(arr, 10)
        if arr != sorted(copy):
            print('quick_sort failed with array:')
            print(copy)
    print('quick_sort test success')