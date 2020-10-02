from insertion_sort import insertion_sort


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


def quick_sort(array, k, start, end):
    # assert k > 2
    if end - start > k:
        p = partition(array, start, end)
        quick_sort(array, k, start, p)
        quick_sort(array, k, p + 1, end)
    else:
        insertion_sort(array, start, end)


if __name__ == '__main__':
    # Driver code to test above
    from random import randint

    for _ in range(100):
        arr = [randint(-10, 10) for _ in range(randint(10, 35))]
        copy = arr.copy()
        quick_sort(arr, 10, 0, len(arr))
        copy.sort()
        assert arr == copy
