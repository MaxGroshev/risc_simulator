#include <asmjit/a64.h>
#include <iostream>

using namespace asmjit;

// Function type: int add_numbers(int a, int b)
typedef int (*AddFunc)(int, int);

int main() {
    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());
    
    a64::Assembler a(&code);

    // Simple function that adds two numbers
    // w0 = first argument (a), w1 = second argument (b)
    a.add(a64::w0, a64::w0, a64::w1);  // w0 = w0 + w1
    a.ret(a64::x30);            // return result in w0

    // Get the function pointer
    AddFunc addNumbers;
    rt.add(&addNumbers, &code);

    // Test it
    int result = addNumbers(5, 3);
    std::cout << "5 + 3 = " << result << std::endl;

    rt.release(addNumbers);
    return 0;
}