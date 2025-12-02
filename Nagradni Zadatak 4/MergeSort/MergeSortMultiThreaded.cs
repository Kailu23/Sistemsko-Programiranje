using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Xml.Schema;

namespace Nagradni_Zadatak_4.MergeSort
{
    public class MergeSortMultiThreaded
    {
        private static int part = 0;
        private static int numberOfThreads;
        private static int[] numbers;
        private static readonly object _locker = new object();
        public static void MultiMergeSort(int[] array) {
            int high, low;
            lock (_locker) {
                if (numberOfThreads > array.Length) {
                    numberOfThreads = array.Length;
                }
                Interlocked.Increment(ref part);
                int  segmentSize = (int)Math.Ceiling((double)array.Length / MergeSortMultiThreaded.numberOfThreads);
                high = (segmentSize * part) - 1;
                low = high - segmentSize + 1;
                if (high >= array.Length)
                        high = array.Length - 1;
            }
            MergeSortSegments(array, low, high);
        }

        private static void MergeSortSegments(int[] array, int low, int high) {
            if (low >= high) return;

            int middle = (low + high) / 2;

            MergeSortSegments(array, low, middle);
            MergeSortSegments(array, middle + 1, high);
            Merge(array, low, high);

        }
        private static void Merge(int[] array, int low, int high) {
            int[] sortArray = new int[high - low + 1];
            int middle = (low + high) / 2;
            int leftIndex = low; int rightIndex = middle + 1;
            for (int i = 0; i <= high - low; i++) {
                if (leftIndex <= middle && rightIndex <= high) {
                    if (array[leftIndex] < array[rightIndex]) {
                        sortArray[i] = array[leftIndex++];
                    } else {
                        sortArray[i] = array[rightIndex++];
                    }
                } else {
                    if (leftIndex > middle) {
                        sortArray[i] = array[rightIndex++]; ;
                    } else {
                        sortArray[i] = array[leftIndex++];
                    }
                }
            }
            for (int i = low; i <= high; i++) {
                array[i] = sortArray[i - low];
            }
        }

        public static int Part {
            get => part;
            set {
                part = value;
            }
        }
        public static int[] Numbers {
            get => numbers;
            set => numbers = value;
        }
        public static int NumberOfThreads {
            get => numberOfThreads;
            set => numberOfThreads = value;
        }
    }
}
