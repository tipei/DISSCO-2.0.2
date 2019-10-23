#ifndef MEMORY_LEAK_CHECK_H
#define MEMORY_LEAK_CHECK_H

#include <iostream>
#include <typeinfo>

/*
Usage
=====
* Call MemoryLeak::Report() at the end of the program to get a report of leaks.
* new/delete should be mapped as desired to the following macros:

Old                                         New
---                                         ---
new Foo                                 --> New(Foo)
delete Foo                              --> Delete(Foo)
Foo* foo = new Foo(Arg1, Arg2)          --> Foo* foo = NewC(new Foo(Arg1, Arg2))
BaseFoo* foo = new SuperFoo(Arg1, Arg2) --> BaseFoo* foo = NewC((BaseFoo*)(new
SuperFoo(Arg1, Arg2)))
*/

// Turn off internal leak checker for Windows, VLD, or when optimizations are
// on.
#if defined(USE_MANUAL_MEMORY_LEAK_CHECK)
#define New(x) MemoryLeak::DebugNew<x>(__LINE__, __FILE__)
#define NewC(x) MemoryLeak::DebugNew(x, __LINE__, __FILE__)
#define Delete(x) MemoryLeak::DebugDelete(x, __LINE__, __FILE__)
#else
#define New(x) (new x)
template <class T>
T* NewC(T* x) {
    return x;
}
template <class T>
void Delete(T*& x) {
    delete x;
    x = 0;
}
#endif

/**A basic memory leak checker. When newing-deleting objects of the same type
(not deleting by base class), call New(type) and Delete(pointer). To new a
subclass and delete its base class, call base* p = NewC((base)new subclass) and
Delete(p). To get a report of memory leaks, call MemoryLeak::Report() near the
end of the program. Enable and disable the leak checker using the
USE_MANUAL_MEMORY_LEAK_CHECK define above.*/
class MemoryLeak {
    static int News[65536];
    static int Deletes[65536];
    static bool FirstNews[65536];
    static bool FirstDeletes[65536];
    static bool Initialized;

    /// Initialize the new and delete ints.
    static void Initialize() {
        for (int i = 0; i < 65536; i++) {
            News[i] = Deletes[i] = 0;
            FirstNews[i] = FirstDeletes[i] = true;
        }
        Initialized = true;
    }

    /// Calculates a hash code based on a given line number and class type
    /// string.
    static inline unsigned short GetCode(int LineNumber, const char* Type) {
        unsigned short Code = 0;
        while (*Type) {
            Code += ((unsigned short)*Type * (unsigned short)53);
            Type++;
        }
        return Code + (unsigned short)LineNumber;
    }

public:
    /**Reports memory leaks. False positives may be occurring if there are
    entries for more deletes than news. In this case, check to make sure NewC
    and Delete are being correctly used for subclass new-base class deletes.*/
    static void Report() {
#ifdef USE_MANUAL_MEMORY_LEAK_CHECK
        bool Detected = false;
        for (int i = 0; i < 65536; i++) {
            if (News[i] != Deletes[i]) Detected = true;
        }
        if (!Detected) {
            std::cout << "No memory leaks detected." << std::endl << std::endl;
            return;
        }

        std::cout << "---------------------------------------------------------"
                     "----------"
                  << std::endl;
        std::cout << "                       MEMORY LEAKS DETECTED" << std::endl;
        std::cout << "---------------------------------------------------------"
                     "----------"
                  << std::endl;
        for (int i = 0; i < 65536; i++) {
            if (News[i] != Deletes[i])
                std::cout << "Memory leak for #" << i << ": " << News[i] << " news and "
                          << Deletes[i] << " deletes" << std::endl;
        }
        std::cout << "---------------------------------------------------------"
                     "----------"
                  << std::endl;
        std::cout << std::endl;
#endif
    }

    /// New that allocates and registers memory by line number and file.
    template <class T>
    static T* DebugNew(int LineNumber, const char* Filename) {
        if (!Initialized) Initialize();
        if (FirstNews[GetCode(LineNumber, typeid(T).name())]) {
            std::cout << "New:    " << typeid(T).name() << " at " << Filename << ":" << LineNumber
                      << " = " << GetCode(0, typeid(T).name()) << std::endl;
            FirstNews[GetCode(LineNumber, typeid(T).name())] = false;
        }
        News[GetCode(0, typeid(T).name())]++;
        return new T;
    }

    /// New that registers existing memory allocation by line number and file.
    template <class T>
    static T* DebugNew(T* Object, int LineNumber, const char* Filename) {
        if (!Initialized) Initialize();
        if (FirstNews[GetCode(LineNumber, typeid(T).name())]) {
            std::cout << "New:    " << typeid(T).name() << " at " << Filename << ":" << LineNumber
                      << " = " << GetCode(0, typeid(T).name()) << std::endl;
            FirstNews[GetCode(LineNumber, typeid(T).name())] = false;
        }
        News[GetCode(0, typeid(T).name())]++;
        return Object;
    }

    /**Delete that registers deleting a memory allocation by line number and
    file. Also sets the object pointer (passed by reference) to zero.*/
    template <class T>
    static void DebugDelete(T*& Object, int LineNumber, const char* Filename) {
        if (!Object) return;  // Exclude null pointers from deletion tracking.
        delete Object;
        Object = 0;
        if (!Initialized) Initialize();
        if (FirstDeletes[GetCode(LineNumber, typeid(T).name())]) {
            std::cout << "Delete: " << typeid(T).name() << " at " << Filename << ":" << LineNumber
                      << " = " << GetCode(0, typeid(T).name()) << std::endl;
            FirstDeletes[GetCode(LineNumber, typeid(T).name())] = false;
        }
        Deletes[GetCode(0, typeid(T).name())]++;
    }
};
#endif
