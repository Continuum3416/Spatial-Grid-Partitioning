#pragma once

namespace mathematical {
    constexpr float PI_f = 3.141593f;
    constexpr double PI = 3.141592653589793;

    constexpr float e_f = 2.718282f;
    constexpr double e = 2.718281828459045; 
}


namespace universal {
    // Planck's constant (h) in J·s
    constexpr float h_f = 6.62607015e-34f;
    constexpr double h = 6.62607015e-34;

    // Reduced Planck's constant (h-bar) in J·s
    constexpr float h_bar_f = 1.0545718e-34f;
    constexpr double h_bar = 1.0545718e-34;

    // Speed of light in vacuum (m/s)
    constexpr unsigned int c_uint = 299792458;
    constexpr int c_int = 299792458;
    constexpr float c_f = 299792458.0f;
    constexpr double c = 299792458.0;

    // Permittivity of free space (F/m)
    constexpr float epsilon_f = 8.85419e-12f;
    constexpr double epsilon = 8.54187817e-12;
}


namespace electromagnetic {
    // Nuclear magnetic moment (mu_n) in J·T^-1
    constexpr float mu_n_f = 5.050783e-27f;
    constexpr double mu_n = 5.050783e-27;
}

namespace earth{
    constexpr float g_f = 9.80665f;
    constexpr double g = 9.80665;
}
