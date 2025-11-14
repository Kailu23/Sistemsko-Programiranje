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
        public static int[] array;

        public MergeSort(int[] array)
        {
            MergeSort.array = array;
        }
        public static void MergeSortSingleThread(int[] array, int low, int high)
        {
            if(low >= high) return;

            int middle = (low + high) / 2;

            MergeSortSingleThread(array, low, middle);
            MergeSortSingleThread(array, middle + 1, high);
            Merge(array, low, high);
        }
        public static void Merge(int[] array, int low, int high)
        {
            int[] sortArray = new int[high - low + 1];
            int middle = (low + high) / 2;
            int leftIndex = low; int rightIndex = middle + 1;
            for (int i = 0; i <= high - low; i++)
            {
                if (leftIndex <= middle && rightIndex <= high)
                {
                    if (array[leftIndex] < array[rightIndex])
                    {
                        sortArray[i] = array[leftIndex++];
                    }
                    else
                    {
                        sortArray[i] = array[rightIndex++];
                    }
                }
                else
                {
                    if (leftIndex > middle)
                    {
                        sortArray[i] = array[rightIndex++];;
                    }
                    else
                    {
                        sortArray[i] = array[leftIndex++];
                    }
                }
            }
            for (int i = low; i <= high; i++)
            {
                array[i] = sortArray[i - low];
            }
        }
    }
}
