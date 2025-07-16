#include <stdio.h>
#include <stdbool.h>

// returns posigion if array has val; -1 otherwise
bool contains(int arr[], int size, int val) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == val) return true;
  }
  return false;
}

// circularly insert values to array.
void insert(int arr[], int size, int* ptr, int val) {
  *ptr = (*ptr + 1) % size;
  arr[*ptr] = val;
}

int main() {
  int ref_size, frame_size;
  printf("Enter number of frames: ");
  scanf("%d", &frame_size);
  printf("Enter length of reference string: ");
  scanf("%d", &ref_size);

  int ref[ref_size], frames[frame_size], current_frame = -1;
  for(int i = 0; i < frame_size; i++) frames[i] = -1;

  printf("Enter reference string\n");
  for (int i = 0; i < ref_size; i++) {
    scanf("%d", &ref[i]);
  }

  int hits = 0;
  printf("req\tstatus     \tframes\n");
  for (int i = 0; i < ref_size; i++) {
    printf("%3d\t", ref[i]);
    if (contains(frames, frame_size, ref[i])) { // page hit
      printf("hit       \t");
      hits++;
    } else { // page fault
      printf("page-fault\t");
      insert(frames, frame_size, &current_frame, ref[i]);
    }
    for (int j = 0; j < frame_size; j++) {
      if (frames[j] < 0) printf("_, ");
      else printf("%d, ", frames[j]);
    }
    printf("\n");
  }

  printf("\nnumber of page hits = %d\n", hits);
  printf("number of page faults = %d\n", ref_size - hits);

  return 0;
}
