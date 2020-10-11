from sorts import *
from time import perf_counter as clock
from random import randint
from functools import partial


def get_runtime(sort_function, array):
    """Returns the time in seconds required to sort an array."""
    start = clock()
    sort_function(array)
    return clock() - start


def test(trials=10):
    """Times the sorts defined in the `sorts` package.

    Randomly generates multiple lists of integers.
    For each list, measure the time each sorting
    takes to complete. Then print the average times.
    """

    # Map algorithm names to their functions. Use functools.partial
    # so that the arguments are uniform for each function call.
    algos = {
        'Merge Sort (k=10)': partial(merge_sort, k=10),
        'Merge Sort (k=1000)': partial(merge_sort, k=1000),
        'Quick Sort (k=10)': partial(quick_sort, k=10),
        'Quick Sort (k=1000)': partial(quick_sort, k=1000),
        'Radix Sort': radix_sort,
        'Tim Sort': lambda array: array.sort()
    }

    # dictionary used to record runtimes
    runtimes = {k: 0 for k in algos.keys()}

    # generate test data and time each algorithm
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

    # reset times for the next test
    for algo in runtimes:
        runtimes[algo] = 0

    # generate test data and time each algorithm
    # (integers limited to 1-10, inclusive)
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
