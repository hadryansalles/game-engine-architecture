#pragma once

#include <chrono>
#include <iostream>
#include <string>

class ScopedInstrument {
public:
    ScopedInstrument(std::string title)
        : m_Title(title)
        , m_Begin(std::chrono::high_resolution_clock::now()) 
    {}

    ~ScopedInstrument() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_Begin).count();
        std::cout << m_Title << duration << " ms\n";
    }

private:
    std::string m_Title;
    std::chrono::steady_clock::time_point m_Begin;
};
