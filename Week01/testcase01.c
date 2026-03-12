int main() {

  char list[] = "1-10,20,30";
  char *token = strtok(list, ",");
  while (token != NULL) {
    printf("Token: %s\n", token);
    token = strtok(NULL, ",");
  }
  init();
  slot_owers[5] = 2; // Simulate slot 5 is allocated to channel 2
  // Test can_allocate function
  int result = can_allocate(1, 1, 10);
  printf("Can allocate slots 1-10 to channel 1? %s\n",
         (result == 1) ? "Yes" : "No");
  int result2 = can_allocate(2, 4, 6);
  printf("Can allocate slots 4-6 to channel 2? %s\n",
         (result2 == 1) ? "Yes" : "No");

  return 0;
}
