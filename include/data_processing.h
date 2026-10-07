#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <vector>
#include <string>
#include <limits>

class DataProcessing {
public:
    static std::vector<std::string> getColumnNames(const std::string& filename);

    static std::vector<std::vector<std::string>> loadData(const std::string& filename);

    static bool checkData(const std::vector<std::vector<std::string>>& data);

    static std::vector<std::vector<double>> convertToNumeric(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<int>& selectedColumns);

    static std::vector<int> extractLabels(
        const std::vector<std::vector<std::string>>& data,
        int classColumn);

    static std::vector<std::vector<double>> normalizeData(
        const std::vector<std::vector<double>>& data);

    static std::vector<double> normalizeVector(
        const std::vector<double>& vector,
        const std::vector<std::vector<double>>& referenceData);

    static bool isNumeric(const std::string& str);

    static void printDataInfo(const std::vector<std::vector<std::string>>& data);
};

#endif
