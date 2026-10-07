#include "utils.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <cctype>

std::vector<std::pair<int, double>> Utils::sortDistances(const std::vector<double>& distances) {
    std::vector<std::pair<int, double>> indexed_distances;

    for (size_t i = 0; i < distances.size(); ++i) {
        indexed_distances.push_back({ static_cast<int>(i), distances[i] });
    }

    std::sort(indexed_distances.begin(), indexed_distances.end(),
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second < b.second;
        });

    return indexed_distances;
}

void Utils::showResult(int prediction, const std::vector<std::pair<int, double>>& neighbors) {
    std::cout << "Predicted Class: " << prediction << std::endl;
    std::cout << "Nearest Neighbors:" << std::endl;

    for (size_t i = 0; i < neighbors.size(); ++i) {
        std::cout << "  " << (i + 1) << ". Class: " << neighbors[i].first
            << ", Distance: " << std::fixed << std::setprecision(4)
            << neighbors[i].second << std::endl;
    }
}

std::string Utils::formatResult(int prediction,
    const std::vector<std::pair<int, double>>& neighbors,
    double confidence) {
    std::stringstream ss;

    ss << "Predicted Class: " << prediction << "\r\n";
    ss << "Confidence: " << std::fixed << std::setprecision(2)
        << (confidence * 100) << "%\r\n\r\n";
    ss << "Nearest Neighbors:\r\n";

    for (size_t i = 0; i < neighbors.size(); ++i) {
        ss << "  " << (i + 1) << ". Class: " << neighbors[i].first
            << ", Distance: " << std::fixed << std::setprecision(4)
            << neighbors[i].second << "\r\n";
    }

    return ss.str();
}

double Utils::calculateConfidence(const std::vector<std::pair<int, double>>& neighbors,
    int predicted_class) {
    if (neighbors.empty()) return 0.0;

    std::map<int, int> votes;
    for (const auto& neighbor : neighbors) {
        votes[neighbor.first]++;
    }

    int total_votes = static_cast<int>(neighbors.size());
    int class_votes = votes[predicted_class];

    return static_cast<double>(class_votes) / total_votes;
}

std::string Utils::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
    return result;
}

bool Utils::isNumeric(const std::string& str) {
    if (str.empty()) return false;

    try {
        std::stod(str);
        return true;
    }
    catch (...) {
        return false;
    }
}
