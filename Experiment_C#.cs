using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

class Program
{
    private const int KeyCount = 1_000_000;

    static void Main(string[] args)
    {
        string filePath = @"C:\\Users\\PC\\OneDrive - vyatsu\\УЧЕБА\\2 курс 4 семестр\\Курсовой проект\\Тестирование\\"; 

        string[] filenames = { "random_keys.txt", "decreazing_str_key.txt", "increazing_str_key.txt", "shuffled_numbers.txt", "decreazing_int_key.txt", "increazing_int_key.txt" };

        for (int i = 2; i < 3; i++)
        {
            var keys = ReadKeysFromFile<string>(filePath + filenames[i], KeyCount);
            Console.WriteLine(filenames[i]);
            filenames[i] = filenames[i].Substring(0, filenames[i].IndexOf('.'));
            TestDictionary<Dictionary<string, int>, string>(keys, filePath + filenames[i] + "_DICT.txt");
            TestDictionary<SortedDictionary<string, int>, string>(keys, filePath + filenames[i] + "_SORT_DICT.txt");
        }

        for (int i = 3; i < 6; i++)
        {
            var keys = ReadKeysFromFile<int>(filePath + filenames[i], KeyCount);
            Console.WriteLine(filenames[i]);
            filenames[i] = filenames[i].Substring(0, filenames[i].IndexOf('.'));
            TestDictionary<Dictionary<int, int>, int>(keys, filePath + filenames[i] + "_DICT.txt");
            TestDictionary<SortedDictionary<int, int>, int>(keys, filePath + filenames[i] + "_SORT_DICT.txt");
        }

        Console.WriteLine("FINISH");
        
    }

    public static List<T> ReadKeysFromFile<T>(string filePath, int keyCount)
    {
        if (!File.Exists(filePath))
            throw new FileNotFoundException($"Файл '{filePath}' не найден.");

        var keys = new List<T>();

        using (var reader = new StreamReader(filePath))
        {
            string line;
            while ((line = reader.ReadLine()) != null && keys.Count < keyCount)
            {
                if (string.IsNullOrWhiteSpace(line))
                    continue;

                try
                {
                    // Преобразование строки к типу T
                    T key = (T)Convert.ChangeType(line.Trim(), typeof(T), CultureInfo.InvariantCulture);
                    keys.Add(key);
                }
                catch (Exception ex) when (ex is FormatException || ex is InvalidCastException || ex is OverflowException)
                {
                    Console.WriteLine($"Не удалось преобразовать строку в тип {typeof(T).Name}: {line}");
                }
            }
        }

        if (keys.Count < keyCount)
            throw new InvalidOperationException($"Файл содержит недостаточно ключей: {keys.Count}/{keyCount}");

        return keys;
    }

        
 
    public static void TestDictionary<TDict, TKey>(List<TKey> allKeys, string outputFile)
    where TDict : IDictionary<TKey, int>, new()
    {
        using (StreamWriter writer = new StreamWriter(outputFile))
        {
            writer.WriteLine("Elements | Insert (ns) | Find (ns) | Erase (ns) | Size After Insert | Size After Erase | Memory Used (B)");
            writer.WriteLine(new string('-', 96));
        }

        var sizes = new[] { 10, 100, 1000, 10_000, 100_000, 1_000_000 };

        foreach (var size in sizes)
        {
            var keys = allKeys.Take(size).ToList();

            double totalInsertTime = 0;
            double totalSearchTime = 0;
            double totalDeleteTime = 0;

            long memoryUsed = 0;
            int sizeAfterInsert = 0;
            int sizeAfterDelete = 0;

            int Iterations = size < 100 ? 1000 : size == 100 ? 100 : size <= 10_000 ? 10 : size == 100_000 ? 5 : 3;

            for (int i = 0; i < Iterations; i++)
            {
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();

                long memoryBefore = GetCurrentMemoryUsage();

                var dict = new TDict();
                GCHandle handle = GCHandle.Alloc(dict, GCHandleType.Normal); // Фиксируем объект
                var insertWatch = Stopwatch.StartNew();

                for (int j = 0; j < size; j++)
                {
                    dict.Add(keys[j], j);
                }

                insertWatch.Stop();
                totalInsertTime += insertWatch.Elapsed.TotalSeconds * 1_000_000_000;

                // Измеряем память и размер только на последней итерации
                if (i == Iterations - 1)
                {
                    long memoryAfterInsert = GetCurrentMemoryUsage();
                    memoryUsed = memoryAfterInsert - memoryBefore;
                    sizeAfterInsert = dict.Count;
                    handle.Free();
                }

                var searchWatch = Stopwatch.StartNew();
                int foundCount = 0;
                foreach (var key in keys)
                {
                    if (dict.ContainsKey(key))
                        foundCount++;
                }
                searchWatch.Stop();
                totalSearchTime += searchWatch.Elapsed.TotalSeconds * 1_000_000_000;

                if (foundCount != size)
                {
                    Console.WriteLine($"Ошибка: не все ключи найдены при поиске. Найдено: {foundCount}/{KeyCount}");
                }

                var deleteWatch = Stopwatch.StartNew();
                foreach (var key in keys)
                {
                    dict.Remove(key);
                }
                deleteWatch.Stop();
                totalDeleteTime += deleteWatch.Elapsed.TotalSeconds * 1_000_000_000;

                if (i == Iterations - 1)
                {
                    sizeAfterDelete = dict.Count;
                }

                dict.Clear();
            }

            double avgInsert = (double)totalInsertTime / Iterations;
            double avgSearch = (double)totalSearchTime / Iterations;
            double avgDelete = (double)totalDeleteTime / Iterations;

            string line = $"{size,10} | {avgInsert,12:F0} | {avgSearch,11:F0} | {avgDelete,10:F0} | {sizeAfterInsert,15} | {sizeAfterDelete,14} | {memoryUsed,12}";

            File.AppendAllText(outputFile, line + Environment.NewLine);
        }
    }

    private static long GetCurrentMemoryUsage()
    {
        GC.Collect(2, GCCollectionMode.Forced, true, true);
        GC.WaitForPendingFinalizers();
        GC.Collect(2, GCCollectionMode.Forced, true, true);
        return GC.GetTotalMemory(true);
    }
}
