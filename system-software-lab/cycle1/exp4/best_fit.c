#include <stdio.h>

int main() {
  int b_count, p_count;
  printf("Enter number of blocks, process: ");
  scanf("%d%d", &b_count, &p_count);

  int blocks[b_count], proc[p_count], in_block[p_count];
  printf("Enter size of each block\n");
  for (int i = 0; i < b_count; i++) scanf("%d", &blocks[i]);
  printf("Enter size of each process\n");
  for (int i = 0; i < p_count; i++) {
    scanf("%d", &proc[i]);    
    in_block[i] = -1;
  }

  int minimum_hole, min_hole_pos, hole;
  for (int i = 0; i < p_count; i++) {
    minimum_hole = 99999;
    min_hole_pos = -1;
    for (int j = 0; j < b_count; j++) {
      if (in_block[i] != -1) break;
      hole = blocks[j] - proc[i];
      if (hole >= 0 && hole < minimum_hole) {
        minimum_hole = hole;
        min_hole_pos = j;
      }
    }
    if (min_hole_pos != -1) {
      blocks[min_hole_pos] -= proc[i];
      in_block[i] = min_hole_pos;
    }
  }

  printf("ProcessNo processSize blockNo\n");
  for (int i = 0; i < p_count; i++)
    printf("%9d %11d %7d\n", i, proc[i], in_block[i]);
  return 0;
}
