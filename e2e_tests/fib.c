int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

int main() {
    int res = fib(29);
    return res; 
}

void _start() {
    int res = main();
    asm volatile ("mv a0, %0" : : "r"(res)); 
    asm volatile ("ecall");  /* Currently Halt in our simulator */
}