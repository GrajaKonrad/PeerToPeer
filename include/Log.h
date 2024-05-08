#pragma once

#include <iostream>

/// @brief Klasa do logowania wiadomości do konsoli
class Log{

public:
    /// @brief Kolor zielony
    static constexpr char ColorGreen[] = "\033[1;32m";

    /// @brief Kolor niebieski
    static constexpr char ColorBlue[] = "\033[1;34m";

    /// @brief Kolor czerwony
    static constexpr char ColorRed[] = "\033[1;31m";

    /// @brief Kolor zółty
    static constexpr char ColorYellow[] = "\033[1;33m";

    /// @brief Czyszczenie koloru
    static constexpr char ResetColor[] = "\033[0m";


    /// @brief Wypisuje wiadomość w kolorze normalnym
    /// @param message wiadomość do wypisania
    inline static void Normal(const std::string& message) {
        std::cout << message << std::endl;
    }

    /// @brief Wypisuje wiadomość w kolorze zielonym
    /// @param message wiadomość do wypisania
    inline static void Green(const std::string& message) {
        std::cout << ColorGreen << message << ResetColor << std::endl;
    }

    /// @brief Wypisuje wiadomość w kolorze niebieskim
    /// @param message wiadomość do wypisania
    inline static void Blue(const std::string& message) {
        std::cout << ColorBlue << message << ResetColor << std::endl;
    }

    /// @brief Wypisuje wiadomość w kolorze czerwonym
    /// @param message wiadomość do wypisania
    inline static void Red(const std::string& message) {
        std::cout << ColorRed << message << ResetColor << std::endl;
    }

    /// @brief Wypisuje wiadomość w kolorze żółtym
    /// @param message wiadomość do wypisania
    inline static void Yellow(const std::string& message) {
        std::cout << ColorYellow<< message << ResetColor << std::endl;
    }

};
