"""
This module defines the Radix Sort algorithm using
Counting Sort as the secondary algorithm.

This code has been adapted from C++ code I wrote
for an assignment in CSCE 3110.

If this module is run directly, it will test itself.
"""


def counting_sort(array, base, digit_selector):
    # array for counting how many times each digit occurs
    # count[1] = how many times 1 occurred.
    count = [0] * base

    # array for putting together the sorted values
    result = [None] * len(array)

    # using digit_selector to isolate the relevant digit,
    # count how many times each digit occurs.
    for n in array:
        count[n//digit_selector % base] += 1

    # modify count to contain the starting index
    # for numbers using the corresponding digit
    for i in range(1, base):
        count[i] += count[i - 1]

    # populate the sorted array: starting from the last element
    # in the array, place that element in the place designated
    # by the index stored in the count array
    for i in reversed(range(len(array))):
        x = array[i]//digit_selector % base
        result[count[x] - 1] = array[i]
        count[x] -= 1

    return result


def radix_sort(array, base=256):
    if len(array) < 2:
        return array

    # Maximum (absolute) array value is necessary
    # for determining when to end the sort.
    # Multiplying by `base` allows the sort
    # to work with negative values.
    m = max(map(abs, array)) * base

    digit_selector = 1
    while m // digit_selector > 0:
        array = counting_sort(array, base, digit_selector)
        digit_selector *= base

    # negative values are placed on the right end
    # of the array, so move them to the left end
    first_negative = -1
    for i, n in enumerate(array):
        if n < 0:
            first_negative = i
            break
    if first_negative >= 0:
        array = array[first_negative:] + array[:first_negative]
    return array


if __name__ == '__main__':
    # driver code to test above
    from random import randint

    for _ in range(500):
        original = [randint(-20, 20) for _ in range(randint(0, 50))]
        arr = radix_sort(original, 10)
        if arr != sorted(original):
            print('radix_sort failed with array:')
            print(original)
    print('radix_sort test complete')
