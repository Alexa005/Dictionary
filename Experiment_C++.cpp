#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include "Dictionary.h"       // Пользовательская хеш-таблица
#include "RB_Dictionary.h"    // Пользовательское красно-черное дерево
#include <windows.h>
#include <psapi.h>

// Функция для получения текущего потребления памяти процессом (в байтах)
size_t get_current_memory_usage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(
        GetCurrentProcess(),
        reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
        sizeof(pmc)
    )) {
        return pmc.PrivateUsage;
    }
    return 0;
}

/**
 * Тестирует производительность словаря и записывает результаты в файл.
 *
 * @tparam DictionaryType Тип тестируемого словаря
 * @tparam KeyType Тип ключей словаря
 * @param testName Название теста для вывода
 * @param allKeys Все доступные ключи для тестирования
 * @param outputFile Путь к выходному файлу с результатами
 */
template<typename DictionaryType, typename KeyType>
void benchmarkDictionary(
    const std::string& testName,
    const std::vector<KeyType>& allKeys,
    const std::string& outputFile
) {
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Ошибка открытия файла: " << outputFile << "\n";
        return;
    }

    // Заголовок таблицы результатов
    outFile << std::setw(10) << "Элементы" << " | "
        << std::setw(12) << "Вставка (нс)" << " | "
        << std::setw(12) << "Поиск (нс)" << " | "
        << std::setw(12) << "Удаление (нс)" << " | "
        << std::setw(12) << "Память (КБ)" << "\n";
    outFile << std::string(70, '-') << "\n";

    // Размеры тестовых наборов данных
    const std::vector<size_t> testSizes = { 10, 100, 1000, 10000, 100000, 1000000 };

    for (size_t currentSize : testSizes) {
        // Проверка достаточности ключей
        if (currentSize > allKeys.size()) {
            std::cerr << "Пропуск размера " << currentSize << " (недостаточно ключей)\n";
            continue;
        }

        // Подготовка тестовых ключей
        std::vector<KeyType> testKeys(allKeys.begin(), allKeys.begin() + currentSize);

        // Количество итераций в зависимости от размера
        const int iterations =
            (currentSize == 10) ? 1000 :
            (currentSize == 100) ? 100 :
            (currentSize <= 10000) ? 10 :
            (currentSize == 100000) ? 5 : 3;

        // Измерение времени операций
        double totalInsertTime = 0;
        double totalSearchTime = 0;
        double totalEraseTime = 0;

        for (int i = 0; i < iterations; ++i) {
            DictionaryType dict;

            // Тест вставки
            auto startTime = std::chrono::high_resolution_clock::now();
            for (const auto& key : testKeys) {
                // Обработка разных интерфейсов словарей
                if constexpr (std::is_same_v<DictionaryType, Dictionary<KeyType, int>> ||
                    std::is_same_v<DictionaryType, RB_Dictionary<KeyType, int>>) {
                    dict.insert(key, 1);
                }
                else {
                    dict[key] = 1;
                }
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            totalInsertTime += std::chrono::duration_cast<std::chrono::nanoseconds>(
                endTime - startTime).count();

            // Тест поиска
            startTime = std::chrono::high_resolution_clock::now();
            for (const auto& key : testKeys) {
                if constexpr (std::is_same_v<DictionaryType, Dictionary<KeyType, int>> ||
                    std::is_same_v<DictionaryType, RB_Dictionary<KeyType, int>>) {
                    if (!dict.find(key)) {
                        std::cerr << "Ключ не найден: " << key << "\n";
                    }
                }
                else {
                    if (dict.find(key) == dict.end()) {
                        std::cerr << "Ключ не найден: " << key << "\n";
                    }
                }
            }
            endTime = std::chrono::high_resolution_clock::now();
            totalSearchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(
                endTime - startTime).count();

            // Тест удаления
            startTime = std::chrono::high_resolution_clock::now();
            for (const auto& key : testKeys) {
                dict.erase(key);
            }
            endTime = std::chrono::high_resolution_clock::now();
            totalEraseTime += std::chrono::duration_cast<std::chrono::nanoseconds>(
                endTime - startTime).count();
        }

        // Измерение потребления памяти
        const int memoryMeasurements = 5;
        size_t totalMemoryUsage = 0;
        int validMeasurements = 0;

        for (int i = 0; i < memoryMeasurements; ++i) {
            size_t startMem = get_current_memory_usage();
            {
                DictionaryType dict;
                for (const auto& key : testKeys) {
                    if constexpr (std::is_same_v<DictionaryType, Dictionary<KeyType, int>> ||
                        std::is_same_v<DictionaryType, RB_Dictionary<KeyType, int>>) {
                        dict.insert(key, 1);
                    }
                    else {
                        dict[key] = 1;
                    }
                }
                size_t endMem = get_current_memory_usage();

                if (endMem > startMem) {
                    totalMemoryUsage += (endMem - startMem);
                    validMeasurements++;
                }
            }
            // Пауза для стабилизации памяти
            if (i < memoryMeasurements - 1) {
                Sleep(5000);
            }
        }

        size_t avgMemoryUsage = validMeasurements > 0
            ? totalMemoryUsage / validMeasurements
            : 0;

        // Расчет среднего времени операций
        double avgInsertTime = totalInsertTime / iterations;
        double avgSearchTime = totalSearchTime / iterations;
        double avgEraseTime = totalEraseTime / iterations;

        // Запись результатов
        outFile << std::setw(10) << currentSize << " | "
            << std::setw(12) << static_cast<uint64_t>(avgInsertTime) << " | "
            << std::setw(12) << static_cast<uint64_t>(avgSearchTime) << " | "
            << std::setw(12) << static_cast<uint64_t>(avgEraseTime) << " | "
            << std::setw(12) << (avgMemoryUsage / 1024) << "\n";
    }

    outFile.close();
    std::cout << "[" << testName << "] Результаты сохранены в " << outputFile << '\n';
}

/**
 * Загружает вектор данных из файла.
 *
 * @param filename Путь к файлу с данными
 * @param dataVector Вектор для загрузки данных
 */
template<typename T>
void loadVectorFromFile(const std::string& filename, std::vector<T>& dataVector) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << "\n";
        return;
    }

    dataVector.clear();
    T value;
    while (inputFile >> value) {
        dataVector.push_back(value);
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    const size_t MAX_KEYS = 1'000'000;

    // Пути к тестовым данным
    const std::string basePath = "test_results/";
    const std::vector<std::string> keyFiles = {
        "random_keys.txt", "decreasing_str_key.txt", "increasing_str_key.txt",
        "shuffled_numbers.txt", "decreasing_int_key.txt", "increasing_int_key.txt"
    };

    // Тестирование со строковыми ключами
    for (int i = 0; i < 3; ++i) {
        std::vector<std::string> stringKeys;
        stringKeys.reserve(MAX_KEYS);
        loadVectorFromFile(keyFiles[i], stringKeys);

        std::string testName = keyFiles[i].substr(0, keyFiles[i].find('.'));
        std::string filePrefix = basePath + testName;

        benchmarkDictionary<Dictionary<std::string, int>>(
            "HashTable", stringKeys, filePrefix + "_hash_dict.txt");
        benchmarkDictionary<std::unordered_map<std::string, int>>(
            "StdHashMap", stringKeys, filePrefix + "_unordered_map.txt");
        benchmarkDictionary<RB_Dictionary<std::string, int>>(
            "RedBlackTree", stringKeys, filePrefix + "_rb_dict.txt");
        benchmarkDictionary<std::map<std::string, int>>(
            "StdTreeMap", stringKeys, filePrefix + "_std_map.txt");
    }

    // Тестирование с целочисленными ключами
    for (int i = 3; i < 6; ++i) {
        std::vector<int> intKeys;
        intKeys.reserve(MAX_KEYS);
        loadVectorFromFile(keyFiles[i], intKeys);

        std::string testName = keyFiles[i].substr(0, keyFiles[i].find('.'));
        std::string filePrefix = basePath + testName;

        benchmarkDictionary<Dictionary<int, int>>(
            "HashTable", intKeys, filePrefix + "_hash_dict.txt");
        benchmarkDictionary<std::unordered_map<int, int>>(
            "StdHashMap", intKeys, filePrefix + "_unordered_map.txt");
        benchmarkDictionary<RB_Dictionary<int, int>>(
            "RedBlackTree", intKeys, filePrefix + "_rb_dict.txt");
        benchmarkDictionary<std::map<int, int>>(
            "StdTreeMap", intKeys, filePrefix + "_std_map.txt");
    }

    return 0;
}
