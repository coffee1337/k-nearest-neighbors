<div align="center">

# 🧠 K-Nearest Neighbors from Scratch

### C++17 implementation of the k-NN classification algorithm with a WinAPI desktop interface

![C++](https://img.shields.io/badge/C++17-111827?style=for-the-badge&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-111827?style=for-the-badge&logo=cmake)
![Windows](https://img.shields.io/badge/Windows-111827?style=for-the-badge&logo=windows)

</div>

---

## 📖 About

Desktop application implementing the **k-Nearest Neighbors (k-NN)** classification algorithm from scratch in C++.

The project was created to explore how k-NN works internally without relying on machine-learning libraries.

The application provides a graphical interface for loading CSV datasets, configuring classification parameters and analysing prediction results.

---

## ✨ Features

- k-NN classification implemented from scratch
- CSV dataset loading
- configurable number of neighbors (`k`)
- multiple distance metrics
- simple and weighted voting
- feature normalization
- feature selection
- test sample generation
- graphical Windows interface
- classification result export

---

## 📐 Distance Metrics

The classifier supports several distance functions.

### Euclidean distance

```text
d(x, y) = √Σ(xᵢ - yᵢ)²
```

### Manhattan distance

```text
d(x, y) = Σ|xᵢ - yᵢ|
```

### Cosine distance

```text
d(x, y) = 1 - (x · y) / (||x|| ||y||)
```

This makes it possible to compare how different metrics affect classification results.

---

## 🗳 Voting Strategies

Two prediction strategies are supported.

### Simple voting

Every nearest neighbor contributes one vote to its class.

### Weighted voting

Closer neighbors have a stronger influence on the final prediction.

This allows the classifier to account for both class frequency and neighbor distance.

---

## 🛠 Tech Stack

- **C++17**
- **STL**
- **WinAPI**
- **CMake**
- **CSV data processing**

No external machine-learning framework is used for the k-NN algorithm itself.

---

## 🏗 Project Structure

```text
k-nearest-neighbors/
│
├── src/              # C++ implementation
├── include/          # header files
├── data/             # example datasets
├── docs/             # reports and documentation
├── screenshots/      # application screenshots
├── CMakeLists.txt
└── README.md
```

The core classifier, data processing and UI logic are separated into independent components.

---

## 🚀 Building

### Requirements

- Windows
- C++17-compatible compiler
- CMake
- Visual Studio / MSVC or compatible Windows C++ toolchain

Clone the repository:

```bash
git clone https://github.com/Coffee1337/k-nearest-neighbors.git
cd k-nearest-neighbors
```

Create a build directory:

```bash
cmake -S . -B build
```

Build the application:

```bash
cmake --build build --config Release
```

The resulting executable will be located inside the CMake build output directory.

---

## 🖥 Usage

Launch the application and load a CSV dataset.

Then:

1. Select the features used for classification.
2. Choose the target variable.
3. Set the number of neighbors (`k`).
4. Select a distance metric.
5. Choose simple or weighted voting.
6. Enable normalization if required.
7. Enter or generate a test sample.
8. Run classification.

The application displays the predicted class, confidence information and nearest neighbors.

---

## 🧪 Example

Example configuration using the Iris dataset:

```text
Dataset: Iris
k: 5
Distance: Euclidean
Voting: Simple
Features: 4
Training samples: 150
```

Example test sample:

```text
5.1, 3.5, 1.4, 0.2
```

Example output:

```text
Predicted Class: 0
Confidence: 95.0%

Nearest Neighbors:
1. Class 0 — Distance: 0.1414
2. Class 0 — Distance: 0.1732
3. Class 0 — Distance: 0.2000
4. Class 0 — Distance: 0.2236
5. Class 0 — Distance: 0.2449
```

---

## 📊 What This Project Demonstrates

The project focuses on implementing the algorithm rather than calling an existing ML library.

It demonstrates:

- understanding of the k-NN algorithm;
- distance calculations;
- data normalization;
- CSV parsing and data processing;
- C++ application architecture;
- desktop UI development with WinAPI;
- build configuration with CMake.

---

## 📄 Documentation

Additional project results and analysis are available in:

```text
docs/result.pdf
```

---


## 🔮 Possible Improvements

- train/test split evaluation
- confusion matrix
- accuracy metrics
- automatic `k` selection
- benchmark mode
- additional datasets
- unit tests for classifier and distance metrics
- GUI modernization

---

## 👨‍💻 Author

**Egor Trefilov / Coffee1337**

GitHub:  
https://github.com/Coffee1337

Portfolio:  
https://coffee1337.github.io

</div>
