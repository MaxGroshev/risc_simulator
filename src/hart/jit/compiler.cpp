#include <asmjit/a64.h>
#include <iostream>

using namespace asmjit;

// Function type: int add_numbers(int a, int b)
typedef int (*AddFunc)(int, int);

static void foo() {
    // Generated from IR
    std::cout << "fooo" << std::endl;
}

int main() {
    JitRuntime rt;
    CodeHolder code;
    StringLogger logger; 
    code.init(rt.environment());
    code.set_logger(&logger);
    
    a64::Assembler a(&code);

    // Simple function that adds two numbers
    // w0 = first argument (a), w1 = second argument (b)
    a.add(a64::w0, a64::w0, a64::w1);  // w0 = w0 + w1
    uintptr_t func_ptr = (uintptr_t)&foo;
    
    a.mov(a64::x2, func_ptr);  // safe, correct, simple
    a.blr(a64::x2);
    a.ret(a64::x30);            // return result in w0
    // Get the function pointer
    AddFunc addNumbers;
    rt.add(&addNumbers, &code);
    printf("The same content: %s\n", logger.content().data());

    // Test it
    int result = addNumbers(5, 3);
    std::cout << "5 + 3 = " << result << std::endl;

    rt.release(addNumbers);
    return 0;
}