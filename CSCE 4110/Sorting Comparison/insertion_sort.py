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
    array = [54, 26, 93, 17, 77, 31, 44, 55, 20]
    insertion_sort(array, 0, len(array))
    print(array)

