#include "metrics.h"
#include <cmath>
#include <algorithm>
#include <numeric>

double Metrics::euclideanDistance(const std::vector<double>& point1,
    const std::vector<double>& point2) {
    if (point1.size() != point2.size()) {
        return std::numeric_limits<double>::max();
    }

    double sum = 0.0;
    for (size_t i = 0; i < point1.size(); ++i) {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

double Metrics::manhattanDistance(const std::vector<double>& point1,
    const std::vector<double>& point2) {
    if (point1.size() != point2.size()) {
        return std::numeric_limits<double>::max();
    }

    double sum = 0.0;
    for (size_t i = 0; i < point1.size(); ++i) {
        sum += std::abs(point1[i] - point2[i]);
    }
    return sum;
}

double Metrics::cosineDistance(const std::vector<double>& point1,
    const std::vector<double>& point2) {
    if (point1.size() != point2.size()) {
        return std::numeric_limits<double>::max();
    }

    double dotProduct = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;

    for (size_t i = 0; i < point1.size(); ++i) {
        dotProduct += point1[i] * point2[i];
        norm1 += point1[i] * point1[i];
        norm2 += point2[i] * point2[i];
    }

    if (norm1 == 0.0 || norm2 == 0.0) {
        return 1.0;
    }

    double cosine = dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
    return 1.0 - cosine;
}

double Metrics::minkowskiDistance(const std::vector<double>& point1,
    const std::vector<double>& point2,
    double p) {
    if (point1.size() != point2.size()) {
        return std::numeric_limits<double>::max();
    }

    if (p == 1.0) {
        return manhattanDistance(point1, point2);
    }
    else if (p == 2.0) {
        return euclideanDistance(point1, point2);
    }

    double sum = 0.0;
    for (size_t i = 0; i < point1.size(); ++i) {
        sum += std::pow(std::abs(point1[i] - point2[i]), p);
    }
    return std::pow(sum, 1.0 / p);
}

double Metrics::calculateDistance(const std::vector<double>& point1,
    const std::vector<double>& point2,
    const std::string& metric) {
    if (metric == "euclidean") {
        return euclideanDistance(point1, point2);
    }
    else if (metric == "manhattan") {
        return manhattanDistance(point1, point2);
    }
    else if (metric == "cosine") {
        return cosineDistance(point1, point2);
    }
    else if (metric == "minkowski") {
        return minkowskiDistance(point1, point2, 3.0);
    }

    return euclideanDistance(point1, point2);
}

std::vector<double> Metrics::normalizeVector(const std::vector<double>& vector) {
    double magnitude = vectorMagnitude(vector);
    if (magnitude == 0.0) {
        return vector;
    }

    std::vector<double> normalized;
    for (double value : vector) {
        normalized.push_back(value / magnitude);
    }
    return normalized;
}

double Metrics::vectorMagnitude(const std::vector<double>& vector) {
    double sum = 0.0;
    for (double value : vector) {
        sum += value * value;
    }
    return std::sqrt(sum);
}

double Metrics::dotProduct(const std::vector<double>& vector1,
    const std::vector<double>& vector2) {
    if (vector1.size() != vector2.size()) {
        return 0.0;
    }

    double product = 0.0;
    for (size_t i = 0; i < vector1.size(); ++i) {
        product += vector1[i] * vector2[i];
    }
    return product;
}
