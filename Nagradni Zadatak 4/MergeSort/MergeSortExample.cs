using Nagradni_Zadatak_4.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nagradni_Zadatak_4.MergeSort
{
    public class MergeSortExample : IExample
    {
        public void Run()
        {
            int[] numbers = new int[] { 38, 27, 43, 3, 9, 82, 10 };
            MergeSort.numbers = numbers;
            MergeSort.MergeSortSingleThread(0, numbers.Length - 1);
            Console.WriteLine("Sorted array:");
            for (int i = 0; i < numbers.Length; i++)
            {
                Console.Write(numbers[i] + " ");
            }
            Console.WriteLine();
        }
    }
}
