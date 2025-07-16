#include <stdio.h>

void swap(int* a, int* b) {
  if (*a != *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
  }
}

int main() {
  int n;
  printf("Enter number of processes: ");
  scanf("%d", &n);
  int p[n], bt[n], wt[n], tat[n], total_wt = 0, total_tat = 0;
  printf("Enter bt of each process\n");
  for (int i = 0; i < n; i++) {
    printf("bt of p%d = ", i + 1);
    scanf("%d", &bt[i]);
    p[i] = i + 1;
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (bt[j] > bt[j + 1]) {
        swap(&bt[j], &bt[j + 1]);
        swap(&p[j], &p[j + 1]);
      }
    }
  }

  wt[0] = 0;
  for (int i = 1; i < n; i++) {
    wt[i] = wt[i - 1] + bt[i - 1];
    total_wt += wt[i];
  }
  for (int i = 0; i < n; i++) {
    tat[i] = wt[i] + bt[i];
    total_tat += tat[i];
  }

  printf("\nprocess bt wt tat\n");
  for (int i = 0; i < n; i++) {
    printf("%7d %2d %2d %3d\n", p[i], bt[i], wt[i], tat[i]);
  }
  printf("avg wt is %.2f\n", (float)total_wt / n);
  printf("avg tat is %.2f\n", (float)total_tat / n);
  return 0;
}
