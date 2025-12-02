using Microsoft.Win32;
using Nagradni_Zadatak_4.MergeSort;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Nagradni_Zadatak_4.QuickSort {
    public class QuickSortMultiThreaded {
        private static int part = 0;
        private static int numberOfThreads;
        private static int[] numbers;
        private static readonly object _locker = new object();

        public static void MultiQuickSort() {
            int low, high;
            lock (_locker) {
                if (numberOfThreads > numbers.Length) {
                    numberOfThreads = numbers.Length;
                }
                Interlocked.Increment(ref part);
                double x = (double) numbers.Length / QuickSortMultiThreaded.numberOfThreads;
                int segmentSize = (int) Math.Ceiling(x);
                high = (segmentSize * part) - 1;
                low = high - segmentSize + 1;
                if (high >= numbers.Length)
                    high = numbers.Length - 1;
            }
            QuickSortSegments(low, high);
        }

        private static void QuickSortSegments(int low, int high) {
            if (low >= high) return;

            int middle = FindMiddle(low, high);

            QuickSortSegments(low, middle);
            QuickSortSegments(middle + 1, high);
        }

        public static int FindMiddle(int low, int high) {
            while (true) {
                int middle = low;
                while (numbers[high] >= numbers[middle] && high != middle) {
                    high--;
                }
                if (high == middle) return middle;
                Swap(middle, high);
                /*Console.Write("FM numbers:\n");
                foreach (int number in numbers) {
                    Console.Write(number + " ");
                }
                Console.WriteLine();*/
                middle = high;
                while (numbers[low] <= numbers[middle] && low != middle) {
                    low++;
                }
                if (low == middle) {
                    return middle;
                }
                Swap(middle, low);
            }
        }

        private static void Swap(int index1, int index2) {
            int temp = numbers[index1];
            numbers[index1] = numbers[index2];
            numbers[index2] = temp;
        }

        public static int[] Numbers {
            get => numbers;
            set => numbers = value;
        }
        public static int NumberOfThreads {
            get => numberOfThreads;
            set => numberOfThreads = value;
        }
        public static int Part {
            get => part;
            set => part = value;
        }
    }
}
