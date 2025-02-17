#pragma once

#include <iostream>
#include <random>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <ctime>
#include <atomic>
#include <cassert>


// automatically defined by the compiler when compile code on a Windows platform.
#ifdef _WIN32
    #include <Windows.h>
#elif defined(__unix__)  // For Unix-like systems (Linux, BSD, etc.)
    #include <unistd.h>
#elif defined(__APPLE__)  // For macOS systems
    #include <unistd.h>
#elif defined(__POSIX__)  // For POSIX-compliant systems
    #include <unistd.h>
#endif

namespace utils{

class Random {
private:
    // using std::default_random_engine = std::mt19937
    std::default_random_engine generator;

    // Static atomic variable for sequential seeding
    static std::atomic<unsigned int> sequentialSeed;

    // Helper to hash strings
    static unsigned int hashString(const std::string& str) {
        std::hash<std::string> hasher;
        return static_cast<unsigned int>(hasher(str));
    }

    // Helper to generate an environment-based seed (for Windows or Unix-based systems)
    static unsigned int generateEnvSeed() {
        unsigned int pid = 0;

#ifdef _WIN32
        pid = static_cast<unsigned int>(GetCurrentProcessId()); // Windows API for Process ID
#else
        pid = static_cast<unsigned int>(getpid()); // Unix/Linux: Process ID
#endif

        auto tid = static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        auto timeSeed = static_cast<unsigned int>(std::time(0)); // Use current time as part of the seed
        return pid ^ tid ^ timeSeed;
    }

    // Helper to generate a composite seed combining multiple sources
    static unsigned int generateCompositeSeed() {
        std::random_device rd;
        auto timeSeed = static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        auto pid = static_cast<unsigned int>(generateEnvSeed()); // Call the environment-based function
        auto tid = static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        return rd() ^ timeSeed ^ pid ^ tid;
    }

    // Helper to generate a cryptographically secure seed
    static unsigned int generateCryptoSeed() {
        std::random_device rd;
        return rd(); // High-entropy seed from hardware RNG
    }

public:
    enum class SeedMode {
        TimeBased,          // Time-based seed (std::time)
        ChronoSeconds,
        ChronoMicroseconds, // High-resolution time-based seed (chrono)
        RandomDevice,       // Entropy-based (std::random_device)
        EnvironmentBased,   // Process/thread/environment-based seed
        Composite,          // Combination of multiple sources
        Sequential,         // Incremental seed for reproducibility
        Cryptographic       // High-entropy cryptographically secure seed
    };


    // Constructor with custom SeedMode
    // Default constructor: Seeds the generator with ChronoSeconds
    Random(SeedMode mode = SeedMode::ChronoSeconds) {
        switch (mode) {
            case SeedMode::TimeBased:{
                generator.seed(static_cast<unsigned int>(std::time(0)));
                break;
            }
            case SeedMode::ChronoSeconds: {
                auto seed = std::chrono::system_clock::now().time_since_epoch().count();
                generator.seed(static_cast<unsigned int>(seed));
                break;
            }
            case SeedMode::ChronoMicroseconds:
                generator.seed(static_cast<unsigned int>(
                std::chrono::high_resolution_clock::now().time_since_epoch().count()));
                break;
            case SeedMode::RandomDevice: {
                std::random_device rd;
                generator.seed(rd());
                break;
            }
            case SeedMode::EnvironmentBased:
                generator.seed(generateEnvSeed());
                break;
            case SeedMode::Composite:
                generator.seed(generateCompositeSeed());
                break;
            case SeedMode::Sequential:
                generator.seed(sequentialSeed.fetch_add(1)); // Increment seed for each instance
                break;
            case SeedMode::Cryptographic:
                generator.seed(generateCryptoSeed());
                break;
        }
    }

    // Constructor with a custom seed value
    Random(unsigned int seed) {
        generator.seed(seed);
    }

    // Constructor with a hash-based seed (e.g., string input)
    Random(const std::string& seedString) {
        generator.seed(hashString(seedString));
    }

    // Constructor with a custom seed function
    Random(std::function<unsigned int()> customSeedFunc) {
        generator.seed(customSeedFunc());
    }

    // Static method to access the current value of sequentialSeed
    static unsigned int getSequentialSeed() {
        return sequentialSeed.load();
    }

    // Functions for code-readability
    bool getRandomBool(){
        return uniformRNG(0, 1);
    }

    int getRandomUInt(unsigned int min = 0, unsigned int max = 100) {
        return uniformRNG(min, max);
    }

    int getRandomInt(int min = -100, int max = 100) {
        return uniformRNG(min, max);
    }

    char getRandomChar(char min = 'a', char max = 'z') {
        return uniformRNG(min, max);
    }

    float generateRandomFloat(float min = 0.0f, float max = 1.0f) {
        return uniformRNG(min, max);
    }

    double generateRandomDouble(double min = 0.0, double max = 1.0) {
        return uniformRNG(min, max);
    }

    // General template function

    /*
        ------------------------------------------------------------------------------------------

        The reason for using std::uniform_int_distribution and std::uniform_real_distribution 
        in the generateRandom function is to generate random numbers uniformly within a specified
        range. These distributions ensure that each value within the range is equally likely to be
        chosen, which is typically desired behavior when generating random numbers.

        ------------------------------------------------------------------------------------------
     */
    template <typename T>
    T uniformRNG(T min, T max) {
        assert(min <= max && "Invalid range: min must be <= max.");

        // std::is_integral: check type T
        if constexpr (std::is_integral<T>::value) {
            // Treat chars as integral values (ASCII codes)
            std::uniform_int_distribution<int> distribution(static_cast<int>(min), static_cast<int>(max));
            return static_cast<T>(distribution(generator));
        } 
        else if constexpr (std::is_floating_point<T>::value) {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(generator);
        }
    }

    // Function using normal distribution
    template <typename T>
    T normalRNG(T mean, T stddev) {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");

        // Normal distribution for integral values (treats them as floating-point and rounds)
        if constexpr (std::is_integral<T>::value) {
            std::normal_distribution<double> distribution(static_cast<double>(mean), static_cast<double>(stddev));
            return static_cast<T>(std::round(distribution(generator)));
        } 
        else if constexpr (std::is_floating_point<T>::value) { // Normal distribution for floating-point values
            std::normal_distribution<T> distribution(mean, stddev);
            return distribution(generator);
        } 
    }

#ifdef HAVE_SFML
#include <SFML/Graphics.hpp>
// Function to generate a random color, allowing users to set ranges for each channel
sf::Color generateRandomColor(
    std::pair<int, int> rRange = {0, 255}, 
    std::pair<int, int> gRange = {0, 255}, // default ranges
    std::pair<int, int> bRange = {0, 255},
    std::pair<int, int> alphaRange = {255, 255}) 
{
    sf::Color color;
    color.r = uniformRNG<int>(rRange.first, rRange.second); // Red channel
    color.g = uniformRNG<int>(gRange.first, gRange.second); // Green channel
    color.b = uniformRNG<int>(bRange.first, bRange.second); // Blue channel
    color.a = uniformRNG<int>(alphaRange.first, alphaRange.second);
    return color;
}

    // sf::Color randomColor = generateRandomColor({1, 2}, {3, 4}, {5, 6});
#endif

#if defined(HAVE_OPENGL) || defined(HAVE_VULKAN)
#include <utility> // For std::pair

// Shared struct for RGBA colors
struct ColorRGBA {
    float r; // Red channel (0.0 to 1.0)
    float g; // Green channel (0.0 to 1.0)
    float b; // Blue channel (0.0 to 1.0)
    float a; // Alpha channel (0.0 to 1.0)
};

ColorRGBA generateRandomColor(
    std::pair<float, float> rRange = {0.0f, 1.0f}, 
    std::pair<float, float> gRange = {0.0f, 1.0f}, 
    std::pair<float, float> bRange = {0.0f, 1.0f}, 
    std::pair<float, float> alphaRange = {0.0f, 1.0f}) 
{
    ColorRGBA color;
    color.r = uniformRNG<float>(rRange.first, rRange.second);
    color.g = uniformRNG<float>(gRange.first, gRange.second);
    color.b = uniformRNG<float>(bRange.first, bRange.second);
    color.a = uniformRNG<float>(alphaRange.first, alphaRange.second);
    return color;
}
#endif


// OpenGL-Specific Setter
#ifdef HAVE_OPENGL
#include <GL/gl.h>

void setOpenGLColor(const ColorRGBA& color) {
    glColor4f(color.r, color.g, color.b, color.a); // Set color for OpenGL
}
#endif

// Vulkan-Specific Setter
#ifdef HAVE_VULKAN
#include <vulkan/vulkan.h> // Vulkan header

VkClearColorValue toVkClearColorValue(const ColorRGBA& color) {
    VkClearColorValue clearColor = {};
    clearColor.float32[0] = color.r;
    clearColor.float32[1] = color.g;
    clearColor.float32[2] = color.b;
    clearColor.float32[3] = color.a;
    return clearColor;
}
#endif
};


// Initialize the sequential seed
std::atomic<unsigned int> utils::Random::sequentialSeed(0);


// With private inheritance, all public and protected members of Random are treated as private members of RandomString.
class RandomString : private Random {
public:
    // Reused constructors
    using Random::Random;

    std::string generateRandomString(unsigned int size = 0) {
        if (size == 0) { size = getRandomUInt(1, 50);}

        assert(size > 0 && "Size must be greater than 0.");

        std::string output;
        output.reserve(size);

        for (size_t i = 0; i < size; i++) {
            output.push_back(getRandomChar());
        }
        return output;
    }
};

template <typename T> class RandomVector : private Random {
public:
    // Reused constructors
    using Random::Random;

    std::vector<T> generateRandomVector(unsigned int size, T min, T max) {
        if (size == 0) { size = getRandomUInt(1, 100);}

        assert(size > 0 && "Size must be greater than 0.");

        std::vector<T> output;
        output.reserve(size);

        for (size_t i = 0; i < size; i++) {
            output.push_back(generateRandom(min, max));
        }
        return output;
    }
};


}