#pragma once
#include <iostream>

// ��������� Chain ������������ ������� ������� ��� ������ ���������� ��������
template <typename t_key, typename t_value>
struct Chain {
    // ���� ��������
    t_key key;
    // �������� ��������
    t_value value;
    // ��������� �� ��������� ������� � �������
    Chain* next = nullptr;

    // ����������� ��� ������������� ����� � ��������
    Chain(t_key k, t_value v) {
        key = k;
        value = v;
    }
};

// ����� Dictionary ��������� ���-������� � ������� �������
template <typename t_key, typename t_value>
class Dictionary
{
private:
    // ������ ���-������� (��������� �������� - 16)
    size_t table_size = 16;
    // ������ ���������� �� ������� (���� ���-�������)
    Chain<t_key, t_value>** table;
    // ������� �������� ���������� ���������
    int element_count;
    // ����������� ���������� ����������� ���������� ������� (75%)
    float max_load_factor = 0.75f;

    // ���-������� � ����������� ��� ������ ����� ������
    int hashFunction(const t_key& key) const {
        // ��������� ������������� ������ (����� �����)
        if constexpr (std::is_same<t_key, int>::value) {
            unsigned int knuth = static_cast<unsigned int>(key);
            return (knuth * 2654435761) % table_size;
        }
        // ��������� ��������� ������ (�������������� ���)
        else if constexpr (std::is_same<t_key, std::string>::value) {
            unsigned int hash = 0;
            for (char ch : key) {
                hash = hash * 31 + ch;
            }
            return hash % table_size;
        }
        // ��������� ������ ����� ������ ����� ����������� std::hash
        else {
            return int(std::hash<t_key>{}(key)) % table_size;
        }
    }

    // ����� ���������� ������� ������� � ����������������� ���������
    void resize() {
        // ��������� ������ �������
        table_size *= 2;
        // ������� ����� ������� � ����������� �����������
        Chain<t_key, t_value>** new_table = new Chain<t_key, t_value>* [table_size]();

        // ��������� �������� � ����� �������
        for (size_t i = 0; i < table_size / 2; ++i) {
            Chain<t_key, t_value>* current = table[i];
            while (current != nullptr) {
                Chain<t_key, t_value>* next = current->next;

                // ��������� ����� ������ ��� ��������
                size_t new_index = hashFunction(current->key);

                // ��������� ������� � ������ ����� �������
                current->next = new_table[new_index];
                new_table[new_index] = current;

                current = next;
            }
        }

        // ������� ������ ������� (�� ������� ��������!)
        delete[] table;

        // ��������� ��������� �� ������� � �� ������
        table = new_table;
    }

public:
    // ����������� �� ���������
    Dictionary() : table(new Chain<t_key, t_value>* [16]()), table_size(16), element_count(0) {
        // �������������� ��� ��������� ������� ��������� nullptr
        for (size_t i = 0; i < table_size; ++i) {
            table[i] = nullptr;
        }
    }

    // �������� ������� ������ �������
    size_t get_size() {
        return table_size;
    }

    // �������� ������������ ����������� ����������
    float get_max_load_factor() {
        return max_load_factor;
    }

    // ������� ���� ����-�������� � �������
    void insert(const t_key& key, const t_value& value) {
        // ��������� ������������� ���������� �������
        if (element_count >= table_size * max_load_factor) {
            resize();
        }
        // ��������� ������ ��� �����
        int index = hashFunction(key);

        // ��������� ������� ��������� �����
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                // ��������� �������� ������������� �����
                temp->value = value;
                return;
            }
        }

        // ��������� ����� ������� � �������
        if (table[index] == nullptr) {
            // ���������� � ������ ������
            table[index] = new Chain<t_key, t_value>(key, value);
        }
        else {
            // ���������� � ������ ������������ �������
            Chain<t_key, t_value>* temp = new Chain<t_key, t_value>(key, value);
            temp->next = table[index];
            table[index] = temp;
        }
        // ����������� ������� ���������
        element_count++;
    }

    // ����� �������� �� �����
    t_value* find(const t_key& key) const {
        // ��������� ������ ��� ������
        int index = hashFunction(key);

        // �������� �� ������� � ������ ������� �����
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                // ���������� ��������� �� ��������� ��������
                return &(temp->value);
            }
        }
        // ���� ���� �� ������, ���������� nullptr
        return nullptr;
    }

    // �������� ������� ����� � �������
    bool contains(const t_key& key) const {
        int index = hashFunction(key);
        // ��������� ������� �� ������� �����
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                return true;
            }
        }
        return false;
    }

    // �������� �������� �� �����
    void erase(const t_key& key) {
        int index = hashFunction(key);
        Chain<t_key, t_value>* current = table[index];
        Chain<t_key, t_value>* before = nullptr;

        // ���� ������� ��� ��������
        while (current != nullptr) {
            if (current->key == key) {
                // ��������� ������� ���������
                element_count--;
                if (before == nullptr) {
                    // �������� ������� �������� �������
                    table[index] = current->next;
                }
                else {
                    // �������� �������� �� ��������/����� �������
                    before->next = current->next;
                }
                delete current;
                return;
            }
            before = current;
            current = current->next;
        }
    }

    // ����� ����������� ������� � �������
    void print() const {
        for (int i = 0; i < table_size; i++) {
            for (Chain<t_key, t_value>* temp = table[i]; temp != nullptr; temp = temp->next) {
                std::cout << '[' << temp->key << ':' << temp->value << "]\n";
            }
        }
    }

    // ������� ���� �������
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

    // �������� ���������� ��������� � �������
    int size() {
        return element_count;
    }

    // ���������, ����� �� �������
    bool empty() {
        return element_count == 0;
    }
};


//#pragma once
//#include <iostream>
//#include <type_traits>  // ��� std::is_same
//
//// ���� ������� ��� ���������� ��������
//template <typename KeyType, typename ValueType>
//struct ChainNode {
//    KeyType key;
//    ValueType value;
//    ChainNode* next = nullptr;
//
//    ChainNode(KeyType k, ValueType v) : key(k), value(v) {}
//};
//
//// ���-������� � ������� �������
//template <typename KeyType, typename ValueType>
//class Dictionary {
//private:
//    size_t table_size = 16;  // ��������� ������ �������
//    ChainNode<KeyType, ValueType>** table;  // �������� ������ ����������
//    int element_count = 0;    // ������� ���������
//    const float max_load_factor = 0.75f;  // ������������ ����������� ����������
//
//    // ������������� ���-�������
//    size_t hashFunction(const KeyType& key) const {
//        // ������������� ��� ����� ����� (����� �����)
//        if constexpr (std::is_same<KeyType, int>::value) {
//            return (static_cast<size_t>(key) * 2654435761 % table_size);
//        }
//        // ������������� ��� ����� (�������������� ���)
//        else if constexpr (std::is_same<KeyType, std::string>::value) {
//            size_t hash = 0;
//            for (char ch : key) {
//                hash = hash * 31 + ch;
//            }
//            return hash % table_size;
//        }
//        // ����� ������ (����������� ������������ ���-�������)
//        else {
//            return std::hash<KeyType>{}(key) % table_size;
//        }
//    }
//
//    // ���������� ������� ��� ���������� ������ ����������
//    void resize() {
//        size_t new_size = table_size * 2;
//        ChainNode<KeyType, ValueType>** new_table = new ChainNode<KeyType, ValueType>* [new_size]();
//
//        // ��������������� ���� ���������
//        for (size_t i = 0; i < table_size; ++i) {
//            ChainNode<KeyType, ValueType>* current = table[i];
//            while (current) {
//                ChainNode<KeyType, ValueType>* next_node = current->next;
//
//                // �������� ������� ��� ������ �������
//                size_t new_index = hashFunction(current->key);
//
//                // ������� � ������ ������� ����� �������
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
//    // ������ ����������� (�������� �������� ��������)
//    Dictionary(const Dictionary&) = delete;
//    Dictionary& operator=(const Dictionary&) = delete;
//
//    ~Dictionary() {
//        clear();
//        delete[] table;
//    }
//
//    // �������� ��������
//    void insert(const KeyType& key, const ValueType& value) {
//        // �������� ������������� ����������
//        if (element_count >= table_size * max_load_factor) {
//            resize();
//        }
//
//        size_t index = hashFunction(key);
//
//        // �������� �� �������� �����
//        ChainNode<KeyType, ValueType>* current = table[index];
//        while (current) {
//            if (current->key == key) {
//                current->value = value;  // ���������� ��������
//                return;
//            }
//            current = current->next;
//        }
//
//        // �������� ������ ����
//        ChainNode<KeyType, ValueType>* new_node = new ChainNode<KeyType, ValueType>(key, value);
//        new_node->next = table[index];  // ������� � ������ �������
//        table[index] = new_node;
//        element_count++;
//    }
//
//    // ����� �������� (���������� nullptr ���� �� �������)
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
//    // �������� ������������� �����
//    bool contains(const KeyType& key) const {
//        return find(key) != nullptr;
//    }
//
//    // �������� ��������
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
//    // ������� �������
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
//    // ��������������� ������
//    size_t capacity() const { return table_size; }
//    int size() const { return element_count; }
//    bool empty() const { return element_count == 0; }
//    float load_factor() const { return static_cast<float>(element_count) / table_size; }
//
//    // ����� ����������� (��� �������)
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