using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Nagradni_Zadatak_4.Utilities;

namespace Nagradni_Zadatak_4.QuickSort {
    public class QuickSortExample : IExample {
        public void Run() {
            int[] numbers = new int[] { 38, 27, 43, 3, 9, 82, 10 };

            Console.WriteLine("Numbers:");
            foreach (int number in numbers) {
                Console.Write($"{number} ");
            }
            Console.WriteLine();
            QuickSortSingleThreaded.Numbers = numbers;
            QuickSortSingleThreaded.QuickSort(0, numbers.Length - 1);

            Console.WriteLine("Sorted numbers:");
            foreach (int number in numbers) {
                Console.Write($"{number} ");
            }
            Console.WriteLine();
        }
    }
}
