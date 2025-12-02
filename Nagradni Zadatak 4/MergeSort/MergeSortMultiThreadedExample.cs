using Nagradni_Zadatak_4.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Nagradni_Zadatak_4.MergeSort {
    public class MergeSortMultiThreadedExample : IExample {
        public void Run() {
            int numberOfThreads = 24;
            Thread[] threads = new Thread[numberOfThreads];

            for (int i = 0; i < numberOfThreads; i++) {
                int[] numbers = new int[] { 38, 27, 43, 3, 9, 82, 10 };
                MergeSortMultiThreaded.Numbers = numbers;
                Console.WriteLine("Numbers:");
                foreach (int number in numbers) {
                    Console.Write(number + " ");
                }
                Console.WriteLine();
                Console.WriteLine("i: " + i);
                int n = i + 1;
                while (n > 1) {
                    MergeSortMultiThreaded.NumberOfThreads = n;
                    for (int j = 0; j < n; j++) {
                        Console.WriteLine("j: " + j);
                        threads[j] = new Thread(() => MergeSortMultiThreaded.MultiMergeSort(numbers));
                        threads[j].Name = "Thread" + j;

                        for (int k = 0; k <= j; k++) {
                            if (threads[k].ThreadState == ThreadState.Unstarted) {
                                threads[k].Start();
                                Console.WriteLine(k + " " + threads[k].ThreadState);
                            }
                        }
                        Console.WriteLine();
                    }
                    for (int j = 0; j < n; j++) {
                        threads[j].Join();
                        Console.WriteLine(j + " " + threads[j].ThreadState);
                    }
                    Console.WriteLine($"Partially sorted numbers:\nn = {n}");
                    foreach (int number in numbers) {
                        Console.Write(number + " ");
                    }
                    Console.WriteLine();
                    n = (int)Math.Round((double)n / 2);
                    MergeSortMultiThreaded.Part = 0;
                }
                MergeSort.numbers = numbers;
                MergeSort.MergeSortSingleThread(0, numbers.Length - 1);
                Console.Write("Sorted numbers:\n");
                foreach (int number in numbers) {
                    Console.Write(number + " ");
                }
                Console.WriteLine();
                MergeSortMultiThreaded.Part = 0;
            }


        }

        void test(int a) {
            Console.WriteLine("x");
        }
    }
}
