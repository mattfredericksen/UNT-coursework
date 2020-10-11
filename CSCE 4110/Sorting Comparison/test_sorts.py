from sorts import *
from time import perf_counter as clock
from random import randint
from functools import partial


def get_runtime(sort_function, array):
    start = clock()
    sort_function(array)
    return clock() - start


def test(trials=10):
    algos = {
        'Merge Sort (k=10)': partial(merge_sort, k=10),
        'Merge Sort (k=1000)': partial(merge_sort, k=1000),
        'Quick Sort (k=10)': partial(quick_sort, k=10),
        'Quick Sort (k=1000)': partial(quick_sort, k=1000),
        'Radix Sort': partial(radix_sort, base=256),
        'Tim Sort': lambda array: array.sort()
    }

    runtimes = {k: 0 for k in algos.keys()}

    for i in range(trials):
        print('Generating test data set #{}'.format(i+1))
        test_data = [randint(-10**6, 10**6) for _ in range(10**6)]
        for algo, func in algos.items():
            print('\tRunning: {}'.format(algo))
            runtimes[algo] += get_runtime(func, test_data.copy())
        print()

    print('Average Runtimes:')
    for algo, runtime in sorted(runtimes.items(), key=lambda x: x[1]):
        print('\t{}: {:.3} seconds'.format(algo, runtime/trials))
    print()

    for algo in runtimes:
        runtimes[algo] = 0

    for i in range(trials):
        print('Generating test data set #{}'.format(i+1))
        test_data = [randint(1, 10) for _ in range(10**6)]
        for algo, func in algos.items():
            print('\tRunning: {}'.format(algo))
            runtimes[algo] += get_runtime(func, test_data.copy())
        print()

    print('Average Runtimes (integers limited to 1-10, inclusive):')
    for algo, runtime in sorted(runtimes.items(), key=lambda x: x[1]):
        print('\t{}: {:.3} seconds'.format(algo, runtime/trials))
    print()


if __name__ == '__main__':
    test()
