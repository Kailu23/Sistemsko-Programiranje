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
            };
            foreach (IExample example in examples)
            {
                example.Run();
            }
        }
    }
}
