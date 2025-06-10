#pragma once
#include <iostream>

// Структура Chain представляет элемент цепочки для метода разрешения коллизий
template <typename t_key, typename t_value>
struct Chain {
    // Ключ элемента
    t_key key;
    // Значение элемента
    t_value value;
    // Указатель на следующий элемент в цепочке
    Chain* next = nullptr;

    // Конструктор для инициализации ключа и значения
    Chain(t_key k, t_value v) {
        key = k;
        value = v;
    }
};

// Класс Dictionary реализует хэш-таблицу с методом цепочек
template <typename t_key, typename t_value>
class Dictionary
{
private:
    // Размер хэш-таблицы (начальное значение - 16)
    size_t table_size = 16;
    // Массив указателей на цепочки (сама хэш-таблица)
    Chain<t_key, t_value>** table;
    // Счетчик текущего количества элементов
    int element_count;
    // Максимально допустимый коэффициент заполнения таблицы (75%)
    float max_load_factor = 0.75f;

    // Хэш-функция с перегрузкой для разных типов ключей
    int hashFunction(const t_key& key) const {
        // Обработка целочисленных ключей (метод Кнута)
        if constexpr (std::is_same<t_key, int>::value) {
            unsigned int knuth = static_cast<unsigned int>(key);
            return (knuth * 2654435761) % table_size;
        }
        // Обработка строковых ключей (полиномиальный хэш)
        else if constexpr (std::is_same<t_key, std::string>::value) {
            unsigned int hash = 0;
            for (char ch : key) {
                hash = hash * 31 + ch;
            }
            return hash % table_size;
        }
        // Обработка других типов ключей через стандартный std::hash
        else {
            return int(std::hash<t_key>{}(key)) % table_size;
        }
    }

    // Метод увеличения размера таблицы и перераспределения элементов
    void resize() {
        // Удваиваем размер таблицы
        table_size *= 2;
        // Создаем новую таблицу с обнуленными указателями
        Chain<t_key, t_value>** new_table = new Chain<t_key, t_value>* [table_size]();

        // Переносим элементы в новую таблицу
        for (size_t i = 0; i < table_size / 2; ++i) {
            Chain<t_key, t_value>* current = table[i];
            while (current != nullptr) {
                Chain<t_key, t_value>* next = current->next;

                // Вычисляем новый индекс для элемента
                size_t new_index = hashFunction(current->key);

                // Добавляем элемент в начало новой цепочки
                current->next = new_table[new_index];
                new_table[new_index] = current;

                current = next;
            }
        }

        // Удаляем старую таблицу (не удаляем элементы!)
        delete[] table;

        // Обновляем указатель на таблицу и ее размер
        table = new_table;
    }

public:
    // Конструктор по умолчанию
    Dictionary() : table(new Chain<t_key, t_value>* [16]()), table_size(16), element_count(0) {
        // Инициализируем все указатели таблицы значением nullptr
        for (size_t i = 0; i < table_size; ++i) {
            table[i] = nullptr;
        }
    }

    // Получить текущий размер таблицы
    size_t get_size() {
        return table_size;
    }

    // Получить максимальный коэффициент заполнения
    float get_max_load_factor() {
        return max_load_factor;
    }

    // Вставка пары ключ-значение в таблицу
    void insert(const t_key& key, const t_value& value) {
        // Проверяем необходимость увеличения таблицы
        if (element_count >= table_size * max_load_factor) {
            resize();
        }
        // Вычисляем индекс для ключа
        int index = hashFunction(key);

        // Проверяем наличие дубликата ключа
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                // Обновляем значение существующего ключа
                temp->value = value;
                return;
            }
        }

        // Добавляем новый элемент в цепочку
        if (table[index] == nullptr) {
            // Добавление в пустую ячейку
            table[index] = new Chain<t_key, t_value>(key, value);
        }
        else {
            // Добавление в начало существующей цепочки
            Chain<t_key, t_value>* temp = new Chain<t_key, t_value>(key, value);
            temp->next = table[index];
            table[index] = temp;
        }
        // Увеличиваем счетчик элементов
        element_count++;
    }

    // Поиск значения по ключу
    t_value* find(const t_key& key) const {
        // Вычисляем индекс для поиска
        int index = hashFunction(key);

        // Проходим по цепочке в поиске нужного ключа
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                // Возвращаем указатель на найденное значение
                return &(temp->value);
            }
        }
        // Если ключ не найден, возвращаем nullptr
        return nullptr;
    }

    // Проверка наличия ключа в таблице
    bool contains(const t_key& key) const {
        int index = hashFunction(key);
        // Проверяем цепочку на наличие ключа
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                return true;
            }
        }
        return false;
    }

    // Удаление элемента по ключу
    void erase(const t_key& key) {
        int index = hashFunction(key);
        Chain<t_key, t_value>* current = table[index];
        Chain<t_key, t_value>* before = nullptr;

        // Ищем элемент для удаления
        while (current != nullptr) {
            if (current->key == key) {
                // Уменьшаем счетчик элементов
                element_count--;
                if (before == nullptr) {
                    // Удаление первого элемента цепочки
                    table[index] = current->next;
                }
                else {
                    // Удаление элемента из середины/конца цепочки
                    before->next = current->next;
                }
                delete current;
                return;
            }
            before = current;
            current = current->next;
        }
    }

    // Вывод содержимого таблицы в консоль
    void print() const {
        for (int i = 0; i < table_size; i++) {
            for (Chain<t_key, t_value>* temp = table[i]; temp != nullptr; temp = temp->next) {
                std::cout << '[' << temp->key << ':' << temp->value << "]\n";
            }
        }
    }

    // Очистка всей таблицы
    void clear() {
        for (size_t i = 0; i < table_size; ++i) {
            Chain<t_key, t_value>* current = table[i];
            while (current != nullptr) {
                Chain<t_key, t_value>* next = current->next;
                delete current;
                current = next;
            }
            table[i] = nullptr;
        }
        element_count = 0;
    }

    // Получить количество элементов в таблице
    int size() {
        return element_count;
    }

    // Проверить, пуста ли таблица
    bool empty() {
        return element_count == 0;
    }
};


//#pragma once
//#include <iostream>
//#include <type_traits>  // Для std::is_same
//
//// Узел цепочки для разрешения коллизий
//template <typename KeyType, typename ValueType>
//struct ChainNode {
//    KeyType key;
//    ValueType value;
//    ChainNode* next = nullptr;
//
//    ChainNode(KeyType k, ValueType v) : key(k), value(v) {}
//};
//
//// Хеш-таблица с методом цепочек
//template <typename KeyType, typename ValueType>
//class Dictionary {
//private:
//    size_t table_size = 16;  // Начальный размер таблицы
//    ChainNode<KeyType, ValueType>** table;  // Основной массив указателей
//    int element_count = 0;    // Счетчик элементов
//    const float max_load_factor = 0.75f;  // Максимальный коэффициент заполнения
//
//    // Универсальная хеш-функция
//    size_t hashFunction(const KeyType& key) const {
//        // Специализация для целых чисел (метод Кнута)
//        if constexpr (std::is_same<KeyType, int>::value) {
//            return (static_cast<size_t>(key) * 2654435761 % table_size);
//        }
//        // Специализация для строк (полиномиальный хеш)
//        else if constexpr (std::is_same<KeyType, std::string>::value) {
//            size_t hash = 0;
//            for (char ch : key) {
//                hash = hash * 31 + ch;
//            }
//            return hash % table_size;
//        }
//        // Общий случай (стандартная библиотечная хеш-функция)
//        else {
//            return std::hash<KeyType>{}(key) % table_size;
//        }
//    }
//
//    // Увеличение таблицы при достижении порога заполнения
//    void resize() {
//        size_t new_size = table_size * 2;
//        ChainNode<KeyType, ValueType>** new_table = new ChainNode<KeyType, ValueType>* [new_size]();
//
//        // Перехеширование всех элементов
//        for (size_t i = 0; i < table_size; ++i) {
//            ChainNode<KeyType, ValueType>* current = table[i];
//            while (current) {
//                ChainNode<KeyType, ValueType>* next_node = current->next;
//
//                // Пересчет индекса для нового размера
//                size_t new_index = hashFunction(current->key);
//
//                // Вставка в начало цепочки новой таблицы
//                current->next = new_table[new_index];
//                new_table[new_index] = current;
//
//                current = next_node;
//            }
//        }
//
//        delete[] table;
//        table = new_table;
//        table_size = new_size;
//    }
//
//public:
//    Dictionary() : table(new ChainNode<KeyType, ValueType>* [16]()) {}
//
//    // Запрет копирования (избегаем двойного удаления)
//    Dictionary(const Dictionary&) = delete;
//    Dictionary& operator=(const Dictionary&) = delete;
//
//    ~Dictionary() {
//        clear();
//        delete[] table;
//    }
//
//    // Основные операции
//    void insert(const KeyType& key, const ValueType& value) {
//        // Проверка необходимости расширения
//        if (element_count >= table_size * max_load_factor) {
//            resize();
//        }
//
//        size_t index = hashFunction(key);
//
//        // Проверка на дубликат ключа
//        ChainNode<KeyType, ValueType>* current = table[index];
//        while (current) {
//            if (current->key == key) {
//                current->value = value;  // Обновление значения
//                return;
//            }
//            current = current->next;
//        }
//
//        // Создание нового узла
//        ChainNode<KeyType, ValueType>* new_node = new ChainNode<KeyType, ValueType>(key, value);
//        new_node->next = table[index];  // Вставка в начало цепочки
//        table[index] = new_node;
//        element_count++;
//    }
//
//    // Поиск значения (возвращает nullptr если не найдено)
//    ValueType* find(const KeyType& key) const {
//        size_t index = hashFunction(key);
//        ChainNode<KeyType, ValueType>* current = table[index];
//
//        while (current) {
//            if (current->key == key) {
//                return &(current->value);
//            }
//            current = current->next;
//        }
//        return nullptr;
//    }
//
//    // Проверка существования ключа
//    bool contains(const KeyType& key) const {
//        return find(key) != nullptr;
//    }
//
//    // Удаление элемента
//    void erase(const KeyType& key) {
//        size_t index = hashFunction(key);
//        ChainNode<KeyType, ValueType>* current = table[index];
//        ChainNode<KeyType, ValueType>* prev = nullptr;
//
//        while (current) {
//            if (current->key == key) {
//                if (prev) {
//                    prev->next = current->next;
//                }
//                else {
//                    table[index] = current->next;
//                }
//
//                delete current;
//                element_count--;
//                return;
//            }
//            prev = current;
//            current = current->next;
//        }
//    }
//
//    // Очистка таблицы
//    void clear() {
//        for (size_t i = 0; i < table_size; ++i) {
//            ChainNode<KeyType, ValueType>* current = table[i];
//            while (current) {
//                ChainNode<KeyType, ValueType>* next = current->next;
//                delete current;
//                current = next;
//            }
//            table[i] = nullptr;
//        }
//        element_count = 0;
//    }
//
//    // Вспомогательные методы
//    size_t capacity() const { return table_size; }
//    int size() const { return element_count; }
//    bool empty() const { return element_count == 0; }
//    float load_factor() const { return static_cast<float>(element_count) / table_size; }
//
//    // Вывод содержимого (для отладки)
//    void print() const {
//        for (size_t i = 0; i < table_size; ++i) {
//            ChainNode<KeyType, ValueType>* current = table[i];
//            while (current) {
//                std::cout << "[" << current->key << ": " << current->value << "]\n";
//                current = current->next;
//            }
//        }
//    }
//};