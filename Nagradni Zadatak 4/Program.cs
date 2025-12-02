using Nagradni_Zadatak_4.MergeSort;
using Nagradni_Zadatak_4.Utilities;
using System;
using System.Collections.Generic;

namespace Nagradni_Zadatak_4.QuickSort
{
    internal class Program
    {
        static void Main(string[] args)
        {
            List<IExample> examples = new List<IExample>()
            {
                new MergeSortExample(),
                new MergeSortMultiThreadedExample(),
                new QuickSortExample(),
                new QuickSortMultiThreadedExample()
            };
            foreach (IExample example in examples) {
                Console.WriteLine(example.Name);
                example.Run();
            }

        }
    }
}
