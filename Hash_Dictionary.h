#pragma once
#include <iostream>

// Ñòðóêòóðà Chain ïðåäñòàâëÿåò ýëåìåíò öåïî÷êè äëÿ ìåòîäà ðàçðåøåíèÿ êîëëèçèé
template <typename t_key, typename t_value>
struct Chain {
    // Êëþ÷ ýëåìåíòà
    t_key key;
    // Çíà÷åíèå ýëåìåíòà
    t_value value;
    // Óêàçàòåëü íà ñëåäóþùèé ýëåìåíò â öåïî÷êå
    Chain* next = nullptr;

    // Êîíñòðóêòîð äëÿ èíèöèàëèçàöèè êëþ÷à è çíà÷åíèÿ
    Chain(t_key k, t_value v) {
        key = k;
        value = v;
    }
};

// Êëàññ Dictionary ðåàëèçóåò õýø-òàáëèöó ñ ìåòîäîì öåïî÷åê
template <typename t_key, typename t_value>
class Dictionary
{
private:
    // Ðàçìåð õýø-òàáëèöû (íà÷àëüíîå çíà÷åíèå - 16)
    size_t table_size = 16;
    // Ìàññèâ óêàçàòåëåé íà öåïî÷êè (ñàìà õýø-òàáëèöà)
    Chain<t_key, t_value>** table;
    // Ñ÷åò÷èê òåêóùåãî êîëè÷åñòâà ýëåìåíòîâ
    int element_count;
    // Ìàêñèìàëüíî äîïóñòèìûé êîýôôèöèåíò çàïîëíåíèÿ òàáëèöû (75%)
    float max_load_factor = 0.75f;

    // Õýø-ôóíêöèÿ ñ ïåðåãðóçêîé äëÿ ðàçíûõ òèïîâ êëþ÷åé
    int hashFunction(const t_key& key) const {
        // Îáðàáîòêà öåëî÷èñëåííûõ êëþ÷åé (ìåòîä Êíóòà)
        if constexpr (std::is_same<t_key, int>::value) {
            unsigned int knuth = static_cast<unsigned int>(key);
            return (knuth * 2654435761) % table_size;
        }
        // Îáðàáîòêà ñòðîêîâûõ êëþ÷åé (ïîëèíîìèàëüíûé õýø)
        else if constexpr (std::is_same<t_key, std::string>::value) {
            unsigned int hash = 0;
            for (char ch : key) {
                hash = hash * 31 + ch;
            }
            return hash % table_size;
        }
        // Îáðàáîòêà äðóãèõ òèïîâ êëþ÷åé ÷åðåç ñòàíäàðòíûé std::hash
        else {
            return int(std::hash<t_key>{}(key)) % table_size;
        }
    }

    // Ìåòîä óâåëè÷åíèÿ ðàçìåðà òàáëèöû è ïåðåðàñïðåäåëåíèÿ ýëåìåíòîâ
    void resize() {
        // Óäâàèâàåì ðàçìåð òàáëèöû
        table_size *= 2;
        // Ñîçäàåì íîâóþ òàáëèöó ñ îáíóëåííûìè óêàçàòåëÿìè
        Chain<t_key, t_value>** new_table = new Chain<t_key, t_value>* [table_size]();

        // Ïåðåíîñèì ýëåìåíòû â íîâóþ òàáëèöó
        for (size_t i = 0; i < table_size / 2; ++i) {
            Chain<t_key, t_value>* current = table[i];
            while (current != nullptr) {
                Chain<t_key, t_value>* next = current->next;

                // Âû÷èñëÿåì íîâûé èíäåêñ äëÿ ýëåìåíòà
                size_t new_index = hashFunction(current->key);

                // Äîáàâëÿåì ýëåìåíò â íà÷àëî íîâîé öåïî÷êè
                current->next = new_table[new_index];
                new_table[new_index] = current;

                current = next;
            }
        }

        // Óäàëÿåì ñòàðóþ òàáëèöó (íå óäàëÿåì ýëåìåíòû!)
        delete[] table;

        // Îáíîâëÿåì óêàçàòåëü íà òàáëèöó è åå ðàçìåð
        table = new_table;
    }

public:
    // Êîíñòðóêòîð ïî óìîë÷àíèþ
    Dictionary() : table(new Chain<t_key, t_value>* [16]()), table_size(16), element_count(0) {
        // Èíèöèàëèçèðóåì âñå óêàçàòåëè òàáëèöû çíà÷åíèåì nullptr
        for (size_t i = 0; i < table_size; ++i) {
            table[i] = nullptr;
        }
    }

    // Ïîëó÷èòü òåêóùèé ðàçìåð òàáëèöû
    size_t get_size() {
        return table_size;
    }

    // Ïîëó÷èòü ìàêñèìàëüíûé êîýôôèöèåíò çàïîëíåíèÿ
    float get_max_load_factor() {
        return max_load_factor;
    }

    // Âñòàâêà ïàðû êëþ÷-çíà÷åíèå â òàáëèöó
    void insert(const t_key& key, const t_value& value) {
        // Ïðîâåðÿåì íåîáõîäèìîñòü óâåëè÷åíèÿ òàáëèöû
        if (element_count >= table_size * max_load_factor) {
            resize();
        }
        // Âû÷èñëÿåì èíäåêñ äëÿ êëþ÷à
        int index = hashFunction(key);

        // Ïðîâåðÿåì íàëè÷èå äóáëèêàòà êëþ÷à
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                // Îáíîâëÿåì çíà÷åíèå ñóùåñòâóþùåãî êëþ÷à
                temp->value = value;
                return;
            }
        }

        // Äîáàâëÿåì íîâûé ýëåìåíò â öåïî÷êó
        if (table[index] == nullptr) {
            // Äîáàâëåíèå â ïóñòóþ ÿ÷åéêó
            table[index] = new Chain<t_key, t_value>(key, value);
        }
        else {
            // Äîáàâëåíèå â íà÷àëî ñóùåñòâóþùåé öåïî÷êè
            Chain<t_key, t_value>* temp = new Chain<t_key, t_value>(key, value);
            temp->next = table[index];
            table[index] = temp;
        }
        // Óâåëè÷èâàåì ñ÷åò÷èê ýëåìåíòîâ
        element_count++;
    }

    // Ïîèñê çíà÷åíèÿ ïî êëþ÷ó
    t_value* find(const t_key& key) const {
        // Âû÷èñëÿåì èíäåêñ äëÿ ïîèñêà
        int index = hashFunction(key);

        // Ïðîõîäèì ïî öåïî÷êå â ïîèñêå íóæíîãî êëþ÷à
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                // Âîçâðàùàåì óêàçàòåëü íà íàéäåííîå çíà÷åíèå
                return &(temp->value);
            }
        }
        // Åñëè êëþ÷ íå íàéäåí, âîçâðàùàåì nullptr
        return nullptr;
    }

    // Ïðîâåðêà íàëè÷èÿ êëþ÷à â òàáëèöå
    bool contains(const t_key& key) const {
        int index = hashFunction(key);
        // Ïðîâåðÿåì öåïî÷êó íà íàëè÷èå êëþ÷à
        for (Chain<t_key, t_value>* temp = table[index]; temp != nullptr; temp = temp->next) {
            if (temp->key == key) {
                return true;
            }
        }
        return false;
    }

    // Óäàëåíèå ýëåìåíòà ïî êëþ÷ó
    void erase(const t_key& key) {
        int index = hashFunction(key);
        Chain<t_key, t_value>* current = table[index];
        Chain<t_key, t_value>* before = nullptr;

        // Èùåì ýëåìåíò äëÿ óäàëåíèÿ
        while (current != nullptr) {
            if (current->key == key) {
                // Óìåíüøàåì ñ÷åò÷èê ýëåìåíòîâ
                element_count--;
                if (before == nullptr) {
                    // Óäàëåíèå ïåðâîãî ýëåìåíòà öåïî÷êè
                    table[index] = current->next;
                }
                else {
                    // Óäàëåíèå ýëåìåíòà èç ñåðåäèíû/êîíöà öåïî÷êè
                    before->next = current->next;
                }
                delete current;
                return;
            }
            before = current;
            current = current->next;
        }
    }

    // Âûâîä ñîäåðæèìîãî òàáëèöû â êîíñîëü
    void print() const {
        for (int i = 0; i < table_size; i++) {
            for (Chain<t_key, t_value>* temp = table[i]; temp != nullptr; temp = temp->next) {
                std::cout << '[' << temp->key << ':' << temp->value << "]\n";
            }
        }
    }

    // Î÷èñòêà âñåé òàáëèöû
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

    // Ïîëó÷èòü êîëè÷åñòâî ýëåìåíòîâ â òàáëèöå
    int size() {
        return element_count;
    }

    // Ïðîâåðèòü, ïóñòà ëè òàáëèöà
    bool empty() {
        return element_count == 0;
    }
};
