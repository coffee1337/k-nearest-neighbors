<!DOCTYPE html>
<html lang="ru">
<body>
    <div class="container">
        <header class="text-center mb-10">
            <h1 class="text-3xl font-bold mb-3">Реализация метода k-ближайших соседей (k-NN) на C++</h1>
            <p class="secondary-text text-lg">Графическое приложение для классификации данных</p>
        </header>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Описание проекта</h2>
            <p class="mb-4">
                Приложение реализует алгоритм k-ближайших соседей (k-NN) для классификации данных с графическим интерфейсом на базе WinAPI. Алгоритм использует евклидово, Манхэттенское и косинусное расстояния. Проект создан в рамках учебной практики, ориентирован на большие датасеты и предоставляет удобный интерфейс для настройки и анализа.
            </p>
            <h3 class="text-xl font-semibold mt-5 mb-2">Актуальность темы</h3>
            <p>
                Метод k-NN актуален благодаря простоте и универсальности. Он применяется в медицине, рекомендательных системах и обработке изображений. C++ с GUI упрощает работу с большими данными.
            </p>
        </div>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Цели и задачи</h2>
            <p class="mb-3">Цель — создание приложения для классификации с анализом эффективности. Задачи:</p>
            <ul class="list-disc list-inside">
                <li>Изучить основы k-NN и метрик расстояния.</li>
                <li>Реализовать k-NN с поддержкой метрик и голосования.</li>
                <li>Создать GUI для работы с данными.</li>
                <li>Добавить нормализацию и генерацию тестовых данных.</li>
                <li>Протестировать на датасетах.</li>
                <li>Оценить точность и производительность.</li>
            </ul>
        </div>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Объект и предмет исследования</h2>
            <p>
                Объект — классификация данных в машинном обучении. Предмет — k-NN, реализованный в C++ с GUI, с настройкой параметров (k, метрика, голосование) и анализом их влияния.
            </p>
        </div>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Технологии и зависимости</h2>
            <ul class="list-disc list-inside">
                <li><strong>Язык:</strong> C++ (C++17).</li>
                <li><strong>Библиотеки:</strong>
                    <ul class="list-circle list-inside ml-4">
                        <li>STL для данных.</li>
                        <li>WinAPI для интерфейса.</li>
                        <li>Модули: <code>knn_classifier.h</code>, <code>data_processing.h</code>, <code>utils.h</code>.</li>
                    </ul>
                </li>
                <li><strong>Сборка:</strong> CMake.</li>
                <li><strong>Компилятор:</strong> MSVC/GCC.</li>
                <li><strong>ОС:</strong> Windows.</li>
            </ul>
        </div>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Установка и запуск</h2>
            <ol class="list-decimal list-inside">
                <li>Склонируйте репозиторий:
                    <pre><code>git clone https://github.com/coffee1337/k-nearest-neighbors.git</code></pre>
                </li>
                <li>Перейдите в директорию:
                    <pre><code>cd k-nearest-neighbors</code></pre>
                </li>
                <li>Создайте и выполните CMake:
                    <pre><code>mkdir build && cd build
cmake ..</code></pre>
                </li>
                <li>Соберите проект:
                    <pre><code>msbuild knn.sln</code></pre>
                    <p class="secondary-text text-sm">(или <code>make</code> для GCC).</p>
                </li>
                <li>Запустите:
                    <pre><code>./knn.exe</code></pre>
                </li>
            </ol>
            <p class="secondary-text mt-3">Требуется Visual Studio или MinGW.</p>
        </div>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Использование</h2>
            <ol class="list-decimal list-inside">
                <li><strong>Загрузка данных:</strong> Нажмите <strong>Load Dataset</strong>, выберите CSV (например, <code>iris.csv</code>).</li>
                <li><strong>Выбор признаков:</strong> Используйте <strong>Features Selection</strong> с <strong>All</strong>, <strong>Clear</strong>, <strong>Select Range</strong>, <strong>Search</strong>.</li>
                <li><strong>Параметры:</strong>
                    <ul class="list-circle list-inside ml-4">
                        <li>k в <strong>Neighbors (k)</strong> (по умолчанию 3).</li>
                        <li>Метрика: <strong>Euclidean</strong>, <strong>Manhattan</strong>, <strong>Cosine</strong>.</li>
                        <li>Голосование: <strong>Simple</strong>, <strong>Weighted</strong>.</li>
                        <li>Нормализация: <strong>Normalize Features</strong>.</li>
                        <li>Целевая переменная: <strong>Target Variable</strong>.</li>
                    </ul>
                </li>
                <li><strong>Тестовые данные:</strong> Введите в <strong>Test Sample</strong> (например, <code>5.1, 3.5, 1.4, 0.2</code>) или нажмите <strong>Generate Sample</strong>.</li>
                <li><strong>Классификация:</strong> Нажмите <strong>CLASSIFY</strong>.</li>
                <li><strong>Сохранение:</strong> Используйте <strong>Save Results</strong>.</li>
            </ol>
        </div>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Пример работы</h2>
            <p class="mb-2">1. Загрузите <code>iris.csv</code>.</p>
            <p class="mb-2">2. Выберите столбцы 1–4.</p>
            <p class="mb-2">3. Укажите k=5, <strong>Euclidean Distance</strong>, <strong>Simple Voting</strong>.</p>
            <p class="mb-2">4. Введите: <code>5.1, 3.5, 1.4, 0.2</code>.</p>
            <p class="mb-2">5. Нажмите <strong>CLASSIFY</strong>.</p>
            <p class="mb-2">Результат:</p>
            <pre><code>=== CLASSIFICATION RESULTS ===
Predicted Class: 0
Confidence: 95.0%
Algorithm: k-NN (k=5)
Distance: euclidean
Features used: 4 of 5
Training samples: 150
Nearest Neighbors:
------------------------
  1. Class 0 - Distance: 0.1414
  2. Class 0 - Distance: 0.1732
  3. Class 0 - Distance: 0.2000
  4. Class 0 - Distance: 0.2236
  5. Class 0 - Distance: 0.2449</code></pre>
        </div>
        <div class="card">
            <h2 class="text-2xl font-semibold mb-3">Результаты и выводы</h2>
            <p>
                Тестирование проведено на Iris и синтетических данных. Выбор k, метрики и голосования влияет на точность. Нормализация улучшает результаты для разномасштабных данных. GUI облегчает работу с большими датасетами.
            </p>
        </div>
        <footer class="text-center secondary-text mt-10">
            <p>© 2025 coffee1337 | <a href="https://github.com/coffee1337/k-nearest-neighbors" class="accent-link">GitHub</a></p>
        </footer>
    </div>
</body>
</html>
