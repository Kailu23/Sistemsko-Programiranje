using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Nagradni_Zadatak_4.MergeSort
{
    public class MergeSort
    {
        public static int[] numbers;
        public static void MergeSortSingleThread(int low, int high)
        {
            if(low >= high) return;

            int middle = (low + high) / 2;

            MergeSortSingleThread(low, middle);
            MergeSortSingleThread(middle + 1, high);
            Merge(low, high);
        }
        public static void Merge(int low, int high)
        {
            int[] sortedNumbers = new int[high - low + 1];
            int middle = (low + high) / 2;
            int leftIndex = low; int rightIndex = middle + 1;
            for (int i = 0; i <= high - low; i++)
            {
                if (leftIndex <= middle && rightIndex <= high)
                {
                    if (numbers[leftIndex] < numbers[rightIndex])
                    {
                        sortedNumbers[i] = numbers[leftIndex++];
                    }
                    else
                    {
                        sortedNumbers[i] = numbers[rightIndex++];
                    }
                }
                else
                {
                    if (leftIndex > middle)
                    {
                        sortedNumbers[i] = numbers[rightIndex++];;
                    }
                    else
                    {
                        sortedNumbers[i] = numbers[leftIndex++];
                    }
                }
            }
            for (int i = low; i <= high; i++)
            {
                numbers[i] = sortedNumbers[i - low];
            }
        }
    }
}
