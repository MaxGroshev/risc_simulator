void print_u64(unsigned long x) {
    register volatile unsigned long a0 asm("a0") = x;
    register volatile unsigned long a7 asm("a7") = 1;
    asm volatile ("ecall");
}

int is_safe(long row, long col, long* board) {
    // print_u64(0xBEAF);
    for (long i = 0; i < row; i++) {
        long p = board[i];
        if (p == col || p - i == col - row || p + i == col + row) {
            return 0;
        }
    }
    return 1;
}

void solve(long row, long* board, unsigned long* count) {
    // print_u64(0xDEAD);
    if (row == 10) {
        (*count)++;
        // print_u64(*count);
        return;
    }
    // print_u64(0xDEAD);

    for (long long col = 0; col < 10; col++) 
    {
        // print_u64(0xBEAF);
        if (is_safe(row, col, board)) {
            board[row] = col;
            solve(row + 1, board, count);
        }
    }

    // print_u64(0xDEAD);
}

int main() {
    long board[10] = {0};
    unsigned long solution_count = 0;

    solve(0, board, &solution_count);

    print_u64(solution_count);

    return solution_count;  // 92
}

void _start() {
    int res = main();
    asm volatile ("mv a0, %0" : : "r"(res)); 
    register volatile unsigned long a7 asm("a7") = 93;
    asm volatile ("ecall");  /* Currently Halt in our simulator */
}