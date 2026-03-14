// Project to reproduce the issue described in
//   "C++ silently skipped object initialization with picolibc / starm-clang 21.1.1"
//
// see README.md for further information.

#include <cstdio>
#include <cstdint>

class Data {
  public:
    Data(uint16_t firstValue) : value(firstValue) {}; // Deleting the constructor results in an assignment in .data and masks the bug.
    const uint8_t ClsVers = 0x42;
    uint16_t value = 0;
};

class Operation {
  public:
    uint8_t id = 0;
    uint8_t cmd = 0;
};

class Foo {
  public:
    // uint8_t buffer[16] = {}; // Activating this line don't change the result.
    Data data{0xBABE};
    Operation op[2] = {}; // op[1] instead op[2] results in an assignment in .data and masks the bug.
    //uint8_t buffer[16] = {}; // Activating this line results in an assignment in .data and masks the bug.
};

Foo foo;

extern "C" void init_array_issue(void) {
    printf("foo after Start:\n");
    printf("================\n");
    printf("foo.data.ClsVers = 0x%02X    (expected: 0x42)\n", foo.data.ClsVers);
    printf("foo.data.value   = 0x%04X  (expected: 0xBABE)\n", foo.data.value);
    printf("foo.op[0].id     = %d       (expected: 0)\n", foo.op[0].id);
    printf("foo.op[0].cmd    = 0x%02X    (expected: 0x00)\n", foo.op[0].cmd);
    if (sizeof(foo.op) > sizeof(Operation)) {
        printf("foo.op[1].id     = %d       (expected: 0)\n", foo.op[1].id);
        printf("foo.op[1].cmd    = 0x%02X    (expected: 0x00)\n\n", foo.op[1].cmd);
    }
 
    foo.data.value = 0xCAFE;
    foo.op[0].id = 1;
    foo.op[0].cmd = 0xAA;
    if (sizeof(foo.op) > sizeof(Operation)) {
        foo.op[1].id = 2;
        foo.op[1].cmd = 0xBB;
    }
    printf("foo after writing values:\n");
    printf("=========================\n");
    printf("foo.data.ClsVers = 0x%02X    (expected: 0x42)\n", foo.data.ClsVers);
    printf("foo.data.value   = 0x%04X  (expected: 0xCAFE)\n", foo.data.value);
    printf("foo.op[0].id     = %d       (expected: 1)\n", foo.op[0].id);
    printf("foo.op[0].cmd    = 0x%02X    (expected: 0xAA)\n", foo.op[0].cmd);
    if (sizeof(foo.op) > sizeof(Operation)) {
        printf("foo.op[1].id     = %d       (expected: 2)\n", foo.op[1].id);
        printf("foo.op[1].cmd    = 0x%02X    (expected: 0xBB)\n\n", foo.op[1].cmd);
    }
      
    while (true) {}
}
