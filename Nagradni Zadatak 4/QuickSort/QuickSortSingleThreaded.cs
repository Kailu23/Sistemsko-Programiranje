using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Nagradni_Zadatak_4.QuickSort
{
    public class QuickSortSingleThreaded {
        private static int[] numbers;

        public static void QuickSort(int low, int high)
        {
            if (low >= high) return;

            int middle = FindMiddle(low, high);

            QuickSort(low, middle);
            QuickSort(middle + 1, high);

        }

        private static int FindMiddle(int low, int high) {
            while (true) {
                int middle = low;
                while (numbers[high] >= numbers[middle] && high != middle) {
                    high--;
                }
                if (high == middle) return middle;
                Swap(middle, high);
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
    }
}
