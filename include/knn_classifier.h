#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include <vector>
#include <string>

class KNNClassifier {
private:
    std::vector<std::vector<double>> trainingData;
    std::vector<int> trainingLabels;
    int k;
    std::string metric;
    bool normalize;
    double p;

    double calculateDistance(const std::vector<double>& point1,
        const std::vector<double>& point2,
        const std::string& metric) const;

    std::vector<std::pair<int, double>> findNeighbors(const std::vector<double>& newObject) const;

    int simpleVote(const std::vector<std::pair<int, double>>& neighbors) const;

    int weightedVote(const std::vector<std::pair<int, double>>& neighbors, double p) const;

    std::vector<double> normalizeNewObject(const std::vector<double>& newObject) const;

public:
    KNNClassifier(const std::vector<std::vector<double>>& data,
        const std::vector<int>& labels,
        int k = 3,
        const std::string& metric = "euclidean",
        bool normalize = false,
        double p = 1.0);

    ~KNNClassifier();

    int predict(const std::vector<double>& newObject);

    std::vector<std::pair<int, double>> getNeighborsInfo(const std::vector<double>& newObject);

    int getK() const { return k; }
    std::string getMetric() const { return metric; }
    bool getNormalize() const { return normalize; }
    double getP() const { return p; }

    void setK(int newK) { k = newK; }
    void setMetric(const std::string& newMetric) { metric = newMetric; }
    void setNormalize(bool newNormalize) { normalize = newNormalize; }
    void setP(double newP) { p = newP; }
};

#endif
