#pragma once

template<typename T>
struct CTTI; // compile time type inspector; just generates a compile time error with a specified type T. Use:
             //                CTTI<T> a; 
             //                CTTI<decltype(t)> b;
