#pragma once

#include <chrono>
#include <iostream>
#include <format>

namespace soso {


    class Timer {
    public:
        Timer() { Reset(); }

        void Reset() { 
            m_Start = std::chrono::high_resolution_clock::now(); 
        }

        float ElapsedMillis() { 
            return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 1e-6f;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };


    class ScopedTimer {
    public:
        ScopedTimer(const std::string& name)
            : m_Name(name) {
        }
        ~ScopedTimer() {
            float time = m_Timer.ElapsedMillis();
            std::cout << std::format("{} - {}ms", m_Name, time) << std::endl;
        }
    private:
        std::string m_Name;
        Timer m_Timer;
    };
}