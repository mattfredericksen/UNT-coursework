def counting_sort(array, base, digit_selector):
    count = [0] * base
    result = [None] * len(array)

    for n in array:
        count[n//digit_selector % base] += 1

    for i in range(1, base):
        count[i] += count[i - 1]

    for i in reversed(range(len(array))):
        x = array[i]//digit_selector % base
        result[count[x] - 1] = array[i]
        count[x] -= 1

    return result


def radix_sort(array, base):
    if len(array) < 2:
        return array

    m = max(map(abs, array)) * base

    digit_selector = 1
    while m // digit_selector > 0:
        array = counting_sort(array, base, digit_selector)
        digit_selector *= base

    first_negative = -1
    for i, n in enumerate(array):
        if n < 0:
            first_negative = i
            break
    if first_negative >= 0:
        array = array[first_negative:] + array[:first_negative]
    return array


if __name__ == '__main__':
    from random import randint

    for _ in range(500):
        arr = [randint(-20, 20) for _ in range(randint(0, 50))]
        copy = arr.copy()
        arr = radix_sort(arr, 16)
        assert arr == sorted(copy)
