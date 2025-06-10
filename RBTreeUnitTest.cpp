#include "pch.h"
#include "CppUnitTest.h"
#include "C:\Users\PC\OneDrive - vyatsu\УЧЕБА\2 курс 4 семестр\Курсовой проект\Dict\RB_Dictionary.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RBTreeUnitTest
{
	TEST_CLASS(RBTreeUnitTest)
	{
	public:
		
        // Тест 1: Базовая вставка и поиск элемента
        TEST_METHOD(Test_Insert_And_Find)
        {
            RB_Dictionary<int, int> dict;
            Assert::IsTrue(dict.insert(1, 10));
            int* value = dict.find(1);
            Assert::IsNotNull(value);
            Assert::AreEqual(10, *value);
        }

        // Тест 2: Обновление значения существующего ключа
        TEST_METHOD(Test_Update_Value)
        {
            RB_Dictionary<int, int> dict;
            Assert::IsTrue(dict.insert(1, 10));
            Assert::IsFalse(dict.insert(1, 20));  // Обновление
            int* value = dict.find(1);
            Assert::IsNotNull(value);
            Assert::AreEqual(20, *value);
        }

        // Тест 3: Удаление элемента
        TEST_METHOD(Test_Erase)
        {
            RB_Dictionary<int, int> dict;
            dict.insert(1, 10);
            Assert::IsTrue(dict.erase(1));
            Assert::IsNull(dict.find(1));
        }

        // Тест 4: Удаление несуществующего элемента
        TEST_METHOD(Test_Erase_NonExisting)
        {
            RB_Dictionary<int, int> dict;
            Assert::IsFalse(dict.erase(1));
        }

        // Тест 5: Проверка размера после вставки и удаления
        TEST_METHOD(Test_Size_After_Insert_Erase)
        {
            RB_Dictionary<int, int> dict;
            Assert::AreEqual(static_cast<size_t>(0), dict.size());

            dict.insert(1, 10);
            dict.insert(2, 20);
            Assert::AreEqual(static_cast<size_t>(2), dict.size());

            dict.erase(1);
            Assert::AreEqual(static_cast<size_t>(1), dict.size());

            dict.erase(2);
            Assert::AreEqual(static_cast<size_t>(0), dict.size());
        }

        // Тест 6: Работа с operator[]
        TEST_METHOD(Test_Operator_Bracket)
        {
            RB_Dictionary<int, int> dict;
            dict[1] = 10;
            Assert::AreEqual(10, dict[1]);

            dict[1] = 20;
            Assert::AreEqual(20, dict[1]);
        }

        // Тест 7: Поиск несуществующего ключа
        TEST_METHOD(Test_Find_NonExisting)
        {
            RB_Dictionary<int, int> dict;
            Assert::IsNull(dict.find(1));
        }

        // Тест 8: Работа с строками как ключами
        TEST_METHOD(Test_String_Keys)
        {
            RB_Dictionary<std::string, int> dict;
            dict.insert("apple", 5);
            dict.insert("banana", 10);
            Assert::AreEqual(5, *dict.find("apple"));
            Assert::AreEqual(10, *dict.find("banana"));
        }

        // Тест 9: Вставка в порядке возрастания (тест на балансировку)
        TEST_METHOD(Test_Insert_Increasing_Order)
        {
            RB_Dictionary<int, int> dict;
            for (int i = 0; i < 10; ++i)
                dict.insert(i, i * 10);

            for (int i = 0; i < 10; ++i)
                Assert::AreEqual(i * 10, *dict.find(i));
        }

        // Тест 10: Вставка в обратном порядке
        TEST_METHOD(Test_Insert_Decreasing_Order)
        {
            RB_Dictionary<int, int> dict;
            for (int i = 9; i >= 0; --i)
                dict.insert(i, i * 10);

            for (int i = 0; i < 10; ++i)
                Assert::AreEqual(i * 10, *dict.find(i));
        }

        // Тест 11: Удаление корня
        TEST_METHOD(Test_Erase_Root)
        {
            RB_Dictionary<int, int> dict;
            dict.insert(10, 100);
            dict.insert(5, 50);
            dict.insert(15, 150);

            Assert::IsTrue(dict.erase(10));  // Удаление корня
            Assert::IsNull(dict.find(10));
            Assert::AreEqual(50, *dict.find(5));
            Assert::AreEqual(150, *dict.find(15));
        }

        // Тест 12: Удаление узла с двумя потомками
        TEST_METHOD(Test_Erase_Node_With_Two_Children)
        {
            RB_Dictionary<int, int> dict;
            dict.insert(10, 100);
            dict.insert(5, 50);
            dict.insert(15, 150);
            dict.insert(12, 120);  // Узел 12 — потомок 15, у него два ребенка

            Assert::IsTrue(dict.erase(15));  // Удаление узла с двумя потомками
            Assert::IsNull(dict.find(15));
            Assert::AreEqual(120, *dict.find(12));
        }

        // Тест 13: Очистка дерева
        TEST_METHOD(Test_Clear)
        {
            RB_Dictionary<int, int> dict;
            dict.insert(1, 10);
            dict.insert(2, 20);
            dict.clear();
            Assert::AreEqual(static_cast<size_t>(0), dict.size());
            Assert::IsNull(dict.find(1));
            Assert::IsNull(dict.find(2));
        }

        // Тест 14: Проверка пула узлов (повторное использование после удаления)
        TEST_METHOD(Test_Node_Pool_Reuse)
        {
            RB_Dictionary<int, int> dict;
            dict.insert(1, 10);
            dict.insert(2, 20);
            dict.erase(1);
            dict.erase(2);

            // Повторная вставка после удаления
            dict.insert(1, 10);
            Assert::AreEqual(10, *dict.find(1));
        }

        // Тест 15: Граничный случай: вставка и удаление одного элемента
        TEST_METHOD(Test_Single_Element_Insert_Delete)
        {
            RB_Dictionary<int, int> dict;
            dict.insert(1, 10);
            Assert::AreEqual(10, *dict.find(1));
            Assert::IsTrue(dict.erase(1));
            Assert::IsNull(dict.find(1));
            Assert::AreEqual(static_cast<size_t>(0), dict.size());
        }
	};
}
