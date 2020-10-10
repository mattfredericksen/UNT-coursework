# Based on pseudocode from Wikipedia:
# https://en.wikipedia.org/wiki/Insertion_sort#Algorithm

def insertion_sort(array, start, end):
    for i in range(start + 1, end):
        x = array[i]
        j = i - 1
        while j >= start and array[j] > x:
            array[j+1] = array[j]
            j -= 1
        array[j+1] = x


if __name__ == '__main__':
    # driver code to test above
    from random import randint

    for _ in range(500):
        arr = [randint(-20, 20) for _ in range(randint(0, 50))]
        copy = arr.copy()
        insertion_sort(arr, 0, len(arr))
        if arr != sorted(copy):
            print('insertion_sort failed with array:')
            print(copy)
    print('insertion_sort test success')
