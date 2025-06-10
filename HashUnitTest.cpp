#include "pch.h"
#include "CppUnitTest.h"
#include "C:\Users\PC\OneDrive - vyatsu\УЧЕБА\2 курс 4 семестр\Курсовой проект\Dict\Dictionary.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HashUnitTest
{

	TEST_CLASS(HashUnitTest)
	{
	public:
		
        // Тест 1: Вставка и поиск элемента
        TEST_METHOD(Test_Insert_And_Find)
        {
            Dictionary<int, int> dict;
            dict.insert(1, 10);
            int* value = dict.find(1);
            Assert::IsNotNull(value);
            Assert::AreEqual(10, *value);
        }

        // Тест 2: Обновление значения
        TEST_METHOD(Test_Update_Value)
        {
            Dictionary<int, int> dict;
            dict.insert(1, 10);
            dict.insert(1, 20); // Обновление
            int* value = dict.find(1);
            Assert::IsNotNull(value);
            Assert::AreEqual(20, *value);
        }

        // Тест 3: Удаление элемента
        TEST_METHOD(Test_Erase)
        {
            Dictionary<int, int> dict;
            dict.insert(1, 10);
            dict.erase(1);
            Assert::IsNull(dict.find(1));
        }

        // Тест 4: Коллизии (разные ключи с одинаковым хэшем)
        TEST_METHOD(Test_Hash_Collisions)
        {
            Dictionary<int, int> dict;
            dict.insert(2, 20);  // hash(2) % 16 = 2
            dict.insert(18, 180); // hash(18) % 16 = 2 (коллизия)
            Assert::AreEqual(20, *dict.find(2));
            Assert::AreEqual(180, *dict.find(18));
        }

        // Тест 5: Ресайз таблицы (увеличение размера)
        TEST_METHOD(Test_Resize)
        {
            Dictionary<int, int> dict;

            // Добавляем достаточно элементов для ресайза
            const int elements_to_resize = 16 * dict.get_max_load_factor() + 1; // ~12 + 1 = 13
            for (int i = 0; i < elements_to_resize; ++i) {
                dict.insert(i, i * 10);
            }

            // Проверяем, что таблица расширилась
            Assert::AreEqual(static_cast<size_t>(32), dict.get_size());

            // Проверяем, что все элементы доступны
            for (int i = 0; i < elements_to_resize; ++i) {
                Assert::IsTrue(dict.contains(i));
                int* value = dict.find(i);
                Assert::IsNotNull(value);
                Assert::AreEqual(i * 10, *value);
            }
        }

        // Тест 6: Очистка таблицы
        TEST_METHOD(Test_Clear)
        {
            Dictionary<int, int> dict;
            dict.insert(1, 10);
            dict.clear();
            Assert::IsTrue(dict.empty());
            Assert::IsNull(dict.find(1));
        }

        // Тест 7: Работа со строками
        TEST_METHOD(Test_String_Keys)
        {
            Dictionary<std::string, int> dict;
            dict.insert("apple", 5);
            dict.insert("banana", 10);
            Assert::AreEqual(5, *dict.find("apple"));
            Assert::AreEqual(10, *dict.find("banana"));
        }

        // Тест 8: Удаление из середины цепочки
        TEST_METHOD(Test_Erase_Middle)
        {
            Dictionary<int, int> dict;
            dict.insert(1, 10);
            dict.insert(17, 20); // hash(17) % 16 = 1 (коллизия)
            dict.insert(33, 30); // hash(33) % 16 = 1 (еще один элемент)
            dict.erase(17); // Удалить средний элемент
            Assert::AreEqual(10, *dict.find(1));
            Assert::AreEqual(30, *dict.find(33));
            Assert::IsNull(dict.find(17));
        }

        // Тест 9: Проверка пустоты
        TEST_METHOD(Test_Empty)
        {
            Dictionary<int, int> dict;
            Assert::IsTrue(dict.empty());
            dict.insert(1, 10);
            Assert::IsFalse(dict.empty());
        }

        // Тест 10: Проверка изменения размера таблицы
        TEST_METHOD(Test_Table_Size_Change)
        {
            Dictionary<int, int> dict;
            Assert::AreEqual(size_t(16), dict.get_size()); // Исходный размер
            for (int i = 0; i < 16 * 0.75 + 1; ++i) {
                dict.insert(i, i);
            }
            Assert::AreEqual(size_t(32), dict.get_size()); // После ресайза
        }

        // Тест 11: Разные ключи с одинаковым хэшем
        TEST_METHOD(Test_Different_Keys_Same_Hash)
        {
            Dictionary<int, int> dict;
            dict.insert(2, 20);  // hash(2) % 16 = 2
            dict.insert(18, 180); // hash(18) % 16 = 2 (коллизия)
            dict.insert(34, 340); // hash(34) % 16 = 2 (еще один элемент)
            Assert::AreEqual(20, *dict.find(2));
            Assert::AreEqual(180, *dict.find(18));
            Assert::AreEqual(340, *dict.find(34));
        }

        // Тест 12: Корректная работа метода size()
        TEST_METHOD(Test_Size_Method)
        {
            Dictionary<int, int> dict;
            Assert::AreEqual(0, dict.size());
            dict.insert(1, 10);
            Assert::AreEqual(1, dict.size());
            dict.insert(2, 20);
            Assert::AreEqual(2, dict.size());
            dict.erase(1);
            Assert::AreEqual(1, dict.size());
        }

        //Тест 13: Удаление несуществующего ключа
        TEST_METHOD(Test_Erase_NonExisting) {
            Dictionary<int, int> dict;
            dict.erase(1);  // Удаление несуществующего ключа
            Assert::AreEqual(0, dict.size());
        }
        //Тест 14: Повторная вставка элемента, после удаления 
        TEST_METHOD(Test_Insert_After_Erase) {
            Dictionary<int, int> dict;
            dict.insert(1, 10);
            dict.erase(1);
            dict.insert(1, 20);  // Повторная вставка
            Assert::AreEqual(20, *dict.find(1));
        }
        //Тест 15: Поиск несуществующего ключа
        TEST_METHOD(Test_Find_Nullptr) {
            Dictionary<int, int> dict;
            Assert::IsNull(dict.find(1));  // Поиск несуществующего ключа
        }
        //Тест 16: Корректная работа метода clear()
        TEST_METHOD(Test_Clear_Complete) {
            Dictionary<int, int> dict;
            for (int i = 0; i < 10; ++i) {
                dict.insert(i, i * 10);
            }
            dict.clear();
            for (int i = 0; i < 10; ++i) {
                Assert::IsNull(dict.find(i));  // Все элементы должны быть удалены
            }
            Assert::AreEqual(0, dict.size());
        }
        //Тест 17: Хэш-таблица из уникальных ключей, но с одинаковыми хэшами
        TEST_METHOD(Test_Long_Chain) {
            Dictionary<int, int> dict;
            const int same_hash = 10;  // Все элементы будут иметь один хэш 
            for (int i = 0; i < 12; ++i) { //если больше 12 элементов, то будет ресайз
                dict.insert(same_hash + i * dict.get_size(), i);  // Генерация уникальных ключей
            }
            for (int i = 0; i < 12; ++i) {
                int key = same_hash + i * dict.get_size();
                Assert::AreEqual(i, *dict.find(key));
            }
        }

        //Тест 18: Корректная работа метода contains() 
        TEST_METHOD(Test_Contains) {
            Dictionary<int, int> dict;
            dict.insert(1, 10);
            Assert::IsTrue(dict.contains(1));
            Assert::IsFalse(dict.contains(2));
        }
	};
}
