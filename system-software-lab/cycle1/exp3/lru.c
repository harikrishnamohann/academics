#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

int find_min_pos(int arr[], int size) {
  int min_val = arr[0], pos = 0;
  for (int i = 1; i < size; i++) {
    if (arr[i] < min_val) {
      min_val = arr[i];
      pos = i;
    }
  }
  return pos;
}

int main() {
  int frame_size, ref_len;
  printf("Enter number of frames: ");
  scanf("%d", &frame_size);
  printf("Enter length of reference string: ");
  scanf("%d", &ref_len);

  int ref_string[ref_len], frames[frame_size], recency[frame_size];
  for (int i = 0; i < frame_size; i++) frames[i] = recency[i] = -1;

  printf("Enter ref string\n");
  for (int i = 0; i < ref_len; i++) {
    printf("ref[%d] = ", i);
    scanf("%d", &ref_string[i]);
  }

  int page_faults = 0;
  printf("\nreq\tstatus   \tframes\n");
  for (int i = 0; i < ref_len; i++) {
    printf("%d\t", ref_string[i]);
    bool is_page_fault = true;

    // check for page hit;
    for (int j = 0; j < frame_size; j++) {
      if (frames[j] == ref_string[i]) {
        is_page_fault = false;
        recency[j] = i;
        break;
      }
    }

    printf("%s\t", is_page_fault ? "page fault" : "hit     ");
    for (int j = 0; j < frame_size; j++) {
      if (frames[j] == -1) printf("_, ");
      else printf("%d, ", frames[j]);
    }

    if (is_page_fault) {
      page_faults++;
      int least_recent_frame = find_min_pos(recency, frame_size);
      frames[least_recent_frame] = ref_string[i];
      recency[least_recent_frame] = i;
    }
    printf("\n");
  }
  printf("\n#page faults = %d\n", page_faults);
  printf("#page hits = %d\n", ref_len - page_faults);
  return 0;
}
