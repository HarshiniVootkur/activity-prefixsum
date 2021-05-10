#include <stdio.h>
#include <math.h>
#include <iostream>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <omp.h>


#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  
  int * arr = new int [n];
  generatePrefixSumData (arr, n);

  int * pr = new int [n+1];

  //insert prefix sum code here

  int *t = new int[nbthreads];
  pr[0] = 0;

  auto startTime = std::chrono::system_clock::now();

  #pragma omp parallel
  {
    int s = 0;
    int val = omp_get_thread_num();

#pragma omp for schedule(static)
    for (int i=0;i<n;i++)
      {
	s = s + arr[i];
	pr[i+1] = s;
      }

    #pragma omp critical
    t[val] = s;

    #pragma omp barrier
    int a = 0;
    for(int j=0;j<val;j++)
      {
	a=a+t[j];
      }

#pragma omp for schedule(static)
    for(int k=0;k<n;k++)
      {
	pr[k+1]=pr[k+1] + a;
      }
  }

  auto endTime = std::chrono::system_clock::now();

  std::chrono::duration<double> totalTime=endTime-startTime;
  
  checkPrefixSumResult(pr, n);
  std::cerr<<totalTime.count()<<std::endl;

  delete[] arr;

  return 0;
}
