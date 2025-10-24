int sum(int a) {
    if (a == 1) {
        return a;
    }
    return a + sum(a - 1);
}

int main() {
    int res = sum(10);
    return res; 
}

void _start() {
    int res = main();
    asm volatile ("mv a0, %0" : : "r"(res)); 
    asm volatile ("ecall");  /* Currently Halt in our simulator */
}