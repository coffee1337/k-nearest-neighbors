#ifndef METRICS_H
#define METRICS_H

#include <vector>
#include <string>
#include <limits>

class Metrics {
public:
    // Основные метрики расстояния
    static double euclideanDistance(const std::vector<double>& point1,
        const std::vector<double>& point2);

    static double manhattanDistance(const std::vector<double>& point1,
        const std::vector<double>& point2);

    static double cosineDistance(const std::vector<double>& point1,
        const std::vector<double>& point2);

    static double minkowskiDistance(const std::vector<double>& point1,
        const std::vector<double>& point2,
        double p = 2.0);

    // Универсальная функция для вычисления расстояния
    static double calculateDistance(const std::vector<double>& point1,
        const std::vector<double>& point2,
        const std::string& metric);

    // Вспомогательные функции
    static std::vector<double> normalizeVector(const std::vector<double>& vector);

    static double vectorMagnitude(const std::vector<double>& vector);

    static double dotProduct(const std::vector<double>& vector1,
        const std::vector<double>& vector2);
};

#endif // METRICS_H
