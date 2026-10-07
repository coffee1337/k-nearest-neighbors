#include "data_processing.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cctype>
#include <set>
#include <map>
#include <string>

std::vector<std::string> DataProcessing::getColumnNames(const std::string& filename) {
    std::vector<std::string> columnNames;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удается открыть файл " << filename << std::endl;
        return columnNames;
    }

    std::string line;
    if (std::getline(file, line)) {
        // Убираем BOM если есть
        if (line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line = line.substr(3);
        }

        // Убираем символы возврата каретки и переносы строк
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        std::stringstream ss(line);
        std::string column;

        while (std::getline(ss, column, ',')) {
            // Убираем пробелы в начале и конце
            column.erase(0, column.find_first_not_of(" \t\r\n"));
            column.erase(column.find_last_not_of(" \t\r\n") + 1);

            if (!column.empty()) {
                columnNames.push_back(column);
            }
        }
    }

    file.close();

    std::cout << "Найдено столбцов: " << columnNames.size() << std::endl;
    for (size_t i = 0; i < columnNames.size(); ++i) {
        std::cout << "Столбец " << i << ": '" << columnNames[i] << "'" << std::endl;
    }

    return columnNames;
}

std::vector<std::vector<std::string>> DataProcessing::loadData(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удается открыть файл " << filename << std::endl;
        return data;
    }

    std::string line;
    bool isFirstLine = true;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        // Убираем символы возврата каретки
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        // Пропускаем заголовок
        if (isFirstLine) {
            isFirstLine = false;
            std::cout << "Пропускаем заголовок: " << line << std::endl;
            continue;
        }

        // Пропускаем пустые строки
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            // Убираем пробелы в начале и конце
            cell.erase(0, cell.find_first_not_of(" \t\r\n"));
            cell.erase(cell.find_last_not_of(" \t\r\n") + 1);

            row.push_back(cell);
        }

        if (!row.empty()) {
            data.push_back(row);
        }
    }

    file.close();

    std::cout << "Всего загружено строк данных: " << data.size() << std::endl;
    if (!data.empty()) {
        std::cout << "Столбцов в первой строке: " << data[0].size() << std::endl;

        // Показываем первые несколько строк для отладки
        for (size_t i = 0; i < std::min(data.size(), size_t(5)); ++i) {
            std::cout << "Строка " << i << ": ";
            for (size_t j = 0; j < data[i].size(); ++j) {
                std::cout << "'" << data[i][j] << "'";
                if (j < data[i].size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        }

        // Анализируем последний столбец (Y)
        if (data[0].size() >= 5) {
            std::set<std::string> uniqueValues;
            for (const auto& row : data) {
                if (row.size() >= 5) {
                    uniqueValues.insert(row[4]);
                }
            }

            std::cout << "Уникальные значения в столбце Y: ";
            for (const auto& val : uniqueValues) {
                std::cout << "'" << val << "' ";
            }
            std::cout << std::endl;
        }
    }

    return data;
}

bool DataProcessing::checkData(const std::vector<std::vector<std::string>>& data) {
    std::cout << "Проверка данных..." << std::endl;

    if (data.empty()) {
        std::cerr << "Ошибка: Данные пусты" << std::endl;
        return false;
    }

    // Проверяем, что есть хотя бы 5 столбцов (X1, X2, X3, X4, Y)
    if (data[0].size() < 5) {
        std::cerr << "Ошибка: Недостаточно столбцов. Найдено: " << data[0].size() << ", ожидается: 5" << std::endl;
        return false;
    }

    size_t expectedColumns = data[0].size();
    std::cout << "Ожидается столбцов: " << expectedColumns << std::endl;

    // Подсчитываем строки с корректным количеством столбцов
    int validRows = 0;
    int rowsWithValidFeatures = 0; // Строки где X1-X4 числовые
    int rowsWithValidLabels = 0;   // Строки где Y не пустое

    for (size_t i = 0; i < data.size(); ++i) {
        const auto& row = data[i];

        if (row.size() == expectedColumns) {
            validRows++;

            // Проверяем, что X1-X4 числовые
            bool featuresValid = true;
            for (int j = 0; j < 4; ++j) {
                if (!isNumeric(row[j])) {
                    featuresValid = false;
                    break;
                }
            }

            if (featuresValid) {
                rowsWithValidFeatures++;

                // Проверяем, что Y не пустое и не "?"
                if (!row[4].empty() && row[4] != "?" && row[4] != "NULL" && row[4] != "null") {
                    rowsWithValidLabels++;
                }
            }
        }
    }

    std::cout << "Статистика данных:" << std::endl;
    std::cout << "- Всего строк: " << data.size() << std::endl;
    std::cout << "- Строк с правильным количеством столбцов: " << validRows << std::endl;
    std::cout << "- Строк с числовыми признаками (X1-X4): " << rowsWithValidFeatures << std::endl;
    std::cout << "- Строк с валидными метками классов (Y): " << rowsWithValidLabels << std::endl;

    // Для обучения нужно хотя бы несколько строк с валидными метками
    if (rowsWithValidLabels >= 2) {
        std::cout << "Проверка данных прошла успешно" << std::endl;
        return true;
    }
    else {
        std::cerr << "Ошибка: Недостаточно строк с валидными метками классов для обучения" << std::endl;
        std::cerr << "Найдено строк с метками: " << rowsWithValidLabels << ", минимум нужно: 2" << std::endl;
        return false;
    }
}

std::vector<std::vector<double>> DataProcessing::convertToNumeric(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<int>& selectedColumns) {

    std::vector<std::vector<double>> numericData;

    std::cout << "Преобразование в числовой формат..." << std::endl;
    std::cout << "Выбранные столбцы: ";
    for (int col : selectedColumns) {
        std::cout << col << " ";
    }
    std::cout << std::endl;

    for (size_t rowIndex = 0; rowIndex < data.size(); ++rowIndex) {
        const auto& row = data[rowIndex];
        std::vector<double> numericRow;
        bool validRow = true;

        for (int colIndex : selectedColumns) {
            if (colIndex >= 0 && colIndex < static_cast<int>(row.size())) {
                try {
                    std::string cellValue = row[colIndex];

                    // Пропускаем строки с пустыми значениями или знаками вопроса
                    if (cellValue.empty() || cellValue == "?" || cellValue == "NULL" || cellValue == "null") {
                        validRow = false;
                        break;
                    }

                    double value = std::stod(cellValue);
                    numericRow.push_back(value);
                }
                catch (const std::exception&) {
                    validRow = false;
                    break;
                }
            }
            else {
                validRow = false;
                break;
            }
        }

        if (validRow && !numericRow.empty()) {
            numericData.push_back(numericRow);
        }
    }

    std::cout << "Преобразовано строк в числовой формат: " << numericData.size() << std::endl;
    return numericData;
}

std::vector<int> DataProcessing::extractLabels(
    const std::vector<std::vector<std::string>>& data,
    int classColumn) {

    std::vector<int> labels;

    std::cout << "Извлечение меток классов из столбца " << classColumn << std::endl;

    // Сначала собираем все уникальные непустые значения
    std::set<std::string> uniqueStringLabels;
    for (const auto& row : data) {
        if (classColumn >= 0 && classColumn < static_cast<int>(row.size())) {
            std::string cellValue = row[classColumn];
            if (!cellValue.empty() && cellValue != "?" && cellValue != "NULL" && cellValue != "null") {
                uniqueStringLabels.insert(cellValue);
            }
        }
    }

    std::cout << "Найденные уникальные метки: ";
    for (const auto& label : uniqueStringLabels) {
        std::cout << "'" << label << "' ";
    }
    std::cout << std::endl;

    // Создаем маппинг строковых меток в числовые
    std::map<std::string, int> labelMapping;
    int labelIndex = 0;
    for (const auto& label : uniqueStringLabels) {
        labelMapping[label] = labelIndex++;
    }

    std::cout << "Маппинг меток:" << std::endl;
    for (const auto& pair : labelMapping) {
        std::cout << "'" << pair.first << "' -> " << pair.second << std::endl;
    }

    // Преобразуем метки
    for (size_t rowIndex = 0; rowIndex < data.size(); ++rowIndex) {
        const auto& row = data[rowIndex];

        if (classColumn >= 0 && classColumn < static_cast<int>(row.size())) {
            std::string cellValue = row[classColumn];

            if (!cellValue.empty() && cellValue != "?" && cellValue != "NULL" && cellValue != "null") {
                if (labelMapping.find(cellValue) != labelMapping.end()) {
                    labels.push_back(labelMapping[cellValue]);
                }
            }
        }
    }

    std::cout << "Извлечено меток классов: " << labels.size() << std::endl;
    return labels;
}

std::vector<std::vector<double>> DataProcessing::normalizeData(
    const std::vector<std::vector<double>>& data) {

    if (data.empty()) return data;

    size_t numFeatures = data[0].size();
    std::vector<double> minValues(numFeatures, std::numeric_limits<double>::max());
    std::vector<double> maxValues(numFeatures, std::numeric_limits<double>::lowest());

    // Находим минимальные и максимальные значения для каждого признака
    for (const auto& row : data) {
        for (size_t j = 0; j < numFeatures && j < row.size(); ++j) {
            minValues[j] = std::min(minValues[j], row[j]);
            maxValues[j] = std::max(maxValues[j], row[j]);
        }
    }

    // Нормализуем данные
    std::vector<std::vector<double>> normalizedData;
    for (const auto& row : data) {
        std::vector<double> normalizedRow;
        for (size_t j = 0; j < numFeatures && j < row.size(); ++j) {
            double range = maxValues[j] - minValues[j];
            if (range > 0) {
                double normalizedValue = (row[j] - minValues[j]) / range;
                normalizedRow.push_back(normalizedValue);
            }
            else {
                normalizedRow.push_back(0.0);
            }
        }
        normalizedData.push_back(normalizedRow);
    }

    return normalizedData;
}

std::vector<double> DataProcessing::normalizeVector(
    const std::vector<double>& vector,
    const std::vector<std::vector<double>>& referenceData) {

    if (referenceData.empty() || vector.empty()) return vector;

    size_t numFeatures = vector.size();
    std::vector<double> minValues(numFeatures, std::numeric_limits<double>::max());
    std::vector<double> maxValues(numFeatures, std::numeric_limits<double>::lowest());

    // Находим минимальные и максимальные значения из референсных данных
    for (const auto& row : referenceData) {
        for (size_t j = 0; j < numFeatures && j < row.size(); ++j) {
            minValues[j] = std::min(minValues[j], row[j]);
            maxValues[j] = std::max(maxValues[j], row[j]);
        }
    }

    // Нормализуем вектор
    std::vector<double> normalizedVector;
    for (size_t j = 0; j < numFeatures; ++j) {
        double range = maxValues[j] - minValues[j];
        if (range > 0) {
            double normalizedValue = (vector[j] - minValues[j]) / range;
            normalizedVector.push_back(normalizedValue);
        }
        else {
            normalizedVector.push_back(0.0);
        }
    }

    return normalizedVector;
}

bool DataProcessing::isNumeric(const std::string& str) {
    if (str.empty()) return false;

    try {
        std::stod(str);
        return true;
    }
    catch (...) {
        return false;
    }
}

void DataProcessing::printDataInfo(const std::vector<std::vector<std::string>>& data) {
    std::cout << "=== Информация о данных ===" << std::endl;
    std::cout << "Загружено строк: " << data.size() << std::endl;
    if (!data.empty()) {
        std::cout << "Столбцов: " << data[0].size() << std::endl;

        // Анализ каждого столбца
        for (size_t col = 0; col < data[0].size(); ++col) {
            int numericCount = 0;
            int emptyCount = 0;
            std::set<std::string> uniqueValues;

            for (const auto& row : data) {
                if (col < row.size()) {
                    if (row[col].empty() || row[col] == "?" || row[col] == "NULL") {
                        emptyCount++;
                    }
                    else {
                        uniqueValues.insert(row[col]);
                        if (isNumeric(row[col])) {
                            numericCount++;
                        }
                    }
                }
            }

            std::cout << "Столбец " << col << ": числовых=" << numericCount
                << ", пустых=" << emptyCount
                << ", уникальных значений=" << uniqueValues.size() << std::endl;
        }
    }
    std::cout << "=========================" << std::endl;
}
