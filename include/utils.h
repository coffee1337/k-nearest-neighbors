#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <utility>

class Utils {
public:
    static std::vector<std::pair<int, double>> sortDistances(const std::vector<double>& distances);

    static void showResult(int prediction, const std::vector<std::pair<int, double>>& neighbors);

    static std::string formatResult(int prediction,
        const std::vector<std::pair<int, double>>& neighbors,
        double confidence);

    static double calculateConfidence(const std::vector<std::pair<int, double>>& neighbors,
        int predicted_class);

    static std::string toLowerCase(const std::string& str);

    static bool isNumeric(const std::string& str);
};

#endif
