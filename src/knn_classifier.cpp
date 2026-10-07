#include "knn_classifier.h"
#include "data_processing.h"
#include <cmath>
#include <algorithm>
#include <map>
#include <iostream>

KNNClassifier::KNNClassifier(const std::vector<std::vector<double>>& data,
    const std::vector<int>& labels,
    int k,
    const std::string& metric,
    bool normalize,
    double p)
    : k(k), metric(metric), normalize(normalize), p(p) {

    if (normalize) {
        trainingData = DataProcessing::normalizeData(data);
    }
    else {
        trainingData = data;
    }

    trainingLabels = labels;
}

KNNClassifier::~KNNClassifier() {
    // Деструктор
}

double KNNClassifier::calculateDistance(const std::vector<double>& point1,
    const std::vector<double>& point2,
    const std::string& metric) const {
    if (point1.size() != point2.size()) {
        return std::numeric_limits<double>::max();
    }

    if (metric == "euclidean") {
        double sum = 0.0;
        for (size_t i = 0; i < point1.size(); ++i) {
            double diff = point1[i] - point2[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
    else if (metric == "manhattan") {
        double sum = 0.0;
        for (size_t i = 0; i < point1.size(); ++i) {
            sum += std::abs(point1[i] - point2[i]);
        }
        return sum;
    }
    else if (metric == "cosine") {
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

    return 0.0;
}

std::vector<std::pair<int, double>> KNNClassifier::findNeighbors(const std::vector<double>& newObject) const {
    std::vector<std::pair<int, double>> distances;

    std::vector<double> processedObject = newObject;
    if (normalize) {
        processedObject = normalizeNewObject(newObject);
    }

    for (size_t i = 0; i < trainingData.size(); ++i) {
        double distance = calculateDistance(processedObject, trainingData[i], metric);
        distances.push_back({ trainingLabels[i], distance });
    }

    std::sort(distances.begin(), distances.end(),
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second < b.second;
        });

    int actualK = std::min(k, static_cast<int>(distances.size()));
    return std::vector<std::pair<int, double>>(distances.begin(), distances.begin() + actualK);
}

int KNNClassifier::simpleVote(const std::vector<std::pair<int, double>>& neighbors) const {
    std::map<int, int> votes;

    for (const auto& neighbor : neighbors) {
        votes[neighbor.first]++;
    }

    int maxVotes = 0;
    int predictedClass = 0;

    for (const auto& vote : votes) {
        if (vote.second > maxVotes) {
            maxVotes = vote.second;
            predictedClass = vote.first;
        }
    }

    return predictedClass;
}

int KNNClassifier::weightedVote(const std::vector<std::pair<int, double>>& neighbors, double p) const {
    std::map<int, double> weightedVotes;

    for (const auto& neighbor : neighbors) {
        double weight = 1.0 / (std::pow(neighbor.second, p) + 1e-8);
        weightedVotes[neighbor.first] += weight;
    }

    double maxWeight = 0.0;
    int predictedClass = 0;

    for (const auto& vote : weightedVotes) {
        if (vote.second > maxWeight) {
            maxWeight = vote.second;
            predictedClass = vote.first;
        }
    }

    return predictedClass;
}

std::vector<double> KNNClassifier::normalizeNewObject(const std::vector<double>& newObject) const {
    return DataProcessing::normalizeVector(newObject, trainingData);
}

int KNNClassifier::predict(const std::vector<double>& newObject) {
    auto neighbors = findNeighbors(newObject);

    if (p == 1.0) {
        return simpleVote(neighbors);
    }
    else {
        return weightedVote(neighbors, p);
    }
}

std::vector<std::pair<int, double>> KNNClassifier::getNeighborsInfo(const std::vector<double>& newObject) {
    return findNeighbors(newObject);
}
