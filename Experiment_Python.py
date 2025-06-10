import time
import sys
import os
import collections
from sortedcontainers import SortedDict


def load_keys(file_path, key_type):
    """Загружает ключи из файла с указанным типом данных."""
    keys = []
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if key_type == 'str':
                keys.append(line)
            elif key_type == 'int':
                keys.append(int(line))
    return keys


def get_iterations(n):
    """Возвращает количество итераций в зависимости от размера данных."""
    if n <= 100:
        return 100
    elif n <= 10000:
        return 10
    elif n == 100000:
        return 5
    elif n == 1000000:
        return 3
    return 10


def test_dict_performance(keys, sizes, dict_class):
    """Тестирует производительность словаря для заданных ключей и размеров."""
    results = []

    for n in sizes:
        if n > len(keys):
            continue

        subset = keys[:n]
        num_iter = get_iterations(n)
        print(f"Testing size: {n} with {num_iter} iterations...")

        # Тест вставки
        total_insert = 0.0
        for _ in range(num_iter):
            d = dict_class()
            start_time = time.perf_counter()
            for key in subset:
                d[key] = True
            end_time = time.perf_counter()
            total_insert += (end_time - start_time)
        avg_insert = total_insert / num_iter

        # Тест поиска
        total_search = 0.0
        for _ in range(num_iter):
            d = dict_class()
            for key in subset:
                d[key] = True
            start_time = time.perf_counter()
            for key in subset:
                if key in d:
                    pass
            end_time = time.perf_counter()
            total_search += (end_time - start_time)
        avg_search = total_search / num_iter

        # Тест удаления
        total_delete = 0.0
        for _ in range(num_iter):
            d = dict_class()
            for key in subset:
                d[key] = True
            start_time = time.perf_counter()
            for key in subset:
                try:
                    del d[key]
                except KeyError:
                    pass
            end_time = time.perf_counter()
            total_delete += (end_time - start_time)
        avg_delete = total_delete / num_iter

        # Замер памяти
        d = dict_class()
        size_before = sys.getsizeof(d)
        for key in subset:
            d[key] = True
        size_after = sys.getsizeof(d)
        memory_usage = size_after - size_before

        results.append((n, avg_insert, avg_search, avg_delete, memory_usage))

    return results


def main():
    # Конфигурация тестов
    sizes = [10, 100, 1000, 10000, 100000, 1000000]
    test_files = [
        ('random_keys.txt', 'str'),
        ('decreazing_str_key.txt', 'str'),
        ('increazing_str_key.txt', 'str'),
        ('increazing_int_key.txt', 'int'),
        ('decreazing_int_key.txt', 'int'),
        ('shuffled_numbers.txt', 'int')
    ]

    # Типы словарей для тестирования
    dict_types = [
        ('dict', dict),
        ('OrderedDict', collections.OrderedDict),
        ('SortedDict', SortedDict)
    ]

    # Создаем директорию для результатов
    os.makedirs('test_results', exist_ok=True)

    for file_name, key_type in test_files:
        if not os.path.exists(file_name):
            print(f"File {file_name} not found. Skipping...")
            continue

        print(f"\nProcessing file: {file_name}")
        keys = load_keys(file_name, key_type)

        for dict_name, dict_class in dict_types:
            print(f"\nTesting dictionary: {dict_name}")
            results = test_dict_performance(keys, sizes, dict_class)

            # Сохраняем результаты
            result_file = os.path.join('test_results',
                                       f"{os.path.splitext(file_name)[0]}_{dict_name}_results.txt")
            with open(result_file, 'w') as f:
                f.write("Size\tInsert Time (s)\tSearch Time (s)\tDelete Time (s)\tMemory Usage (bytes)\n")
                for r in results:
                    f.write(f"{r[0]}\t{r[1]:.10f}\t{r[2]:.10f}\t{r[3]:.10f}\t{r[4]}\n")
            print(f"Results saved to {result_file}")


if __name__ == "__main__":
    main()
