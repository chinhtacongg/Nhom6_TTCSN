#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>

using namespace std;

// Định nghĩa cấu trúc để lưu tọa độ của một thành phố
struct City {
    double x, y;
};

// Hàm tính khoảng cách giữa hai thành phố sử dụng công thức khoảng cách Euclidean
double distance(const City& a, const City& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Hàm tính tổng khoảng cách của một hành trình (route)
double calculateRouteDistance(const vector<int>& route, const vector<City>& cities) {
    double totalDistance = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        totalDistance += distance(cities[route[i]], cities[route[i + 1]]); // Khoảng cách giữa các thành phố liền kề
    }
    totalDistance += distance(cities[route.back()], cities[route[0]]); // Quay về thành phố đầu tiên
    return totalDistance;
}

// Hàm tạo một quần thể ban đầu (population) chứa các hành trình ngẫu nhiên
vector<vector<int>> initializePopulation(int populationSize, int numCities) {
    vector<vector<int>> population; // Lưu quần thể
    vector<int> baseRoute(numCities); // Một hành trình cơ bản chứa các chỉ số của các thành phố
    for (int i = 0; i < numCities; ++i) {
        baseRoute[i] = i; // Khởi tạo hành trình: 0, 1, 2, ..., numCities-1
    }
    for (int i = 0; i < populationSize; ++i) {
        random_shuffle(baseRoute.begin(), baseRoute.end()); // Trộn ngẫu nhiên hành trình
        population.push_back(baseRoute); // Thêm vào quần thể
    }
    return population;
}

// Hàm chọn lọc dựa trên đánh giá (fitness) theo phương pháp Tournament Selection
vector<int> selectParent(const vector<vector<int>>& population, const vector<City>& cities) {
    int tournamentSize = 5; // Số lượng hành trình được chọn ngẫu nhiên để so sánh
    vector<int> best = population[rand() % population.size()]; // Chọn ngẫu nhiên một hành trình làm tốt nhất ban đầu
    double bestDistance = calculateRouteDistance(best, cities); // Tính khoảng cách của hành trình này

    for (int i = 1; i < tournamentSize; ++i) {
        vector<int> candidate = population[rand() % population.size()]; // Chọn một hành trình khác
        double candidateDistance = calculateRouteDistance(candidate, cities); // Tính khoảng cách
        if (candidateDistance < bestDistance) { // Nếu hành trình này tốt hơn
            best = candidate;
            bestDistance = candidateDistance;
        }
    }
    return best; // Trả về hành trình tốt nhất
}

// Hàm lai ghép (crossover) để tạo hành trình con từ hai cha mẹ
vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2) {
    int size = parent1.size();
    vector<int> child(size, -1); // Hành trình con, ban đầu là -1
    int start = rand() % size;
    int end = rand() % size;

    if (start > end) swap(start, end); // Đảm bảo start <= end

    // Sao chép một đoạn từ cha mẹ đầu tiên vào con
    for (int i = start; i <= end; ++i) {
        child[i] = parent1[i];
    }

    // Điền các thành phố còn thiếu từ cha mẹ thứ hai
    int currentIndex = 0;
    for (int i = 0; i < size; ++i) {
        if (find(child.begin(), child.end(), parent2[i]) == child.end()) { // Nếu thành phố chưa tồn tại trong con
            while (child[currentIndex] != -1) { // Tìm vị trí trống
                currentIndex++;
            }
            child[currentIndex] = parent2[i];
        }
    }

    return child;
}

// Hàm đột biến (mutation) để thay đổi ngẫu nhiên một hành trình
void mutate(vector<int>& route) {
    if (rand() % 100 < 10) { // 10% cơ hội đột biến
        int index1 = rand() % route.size();
        int index2 = rand() % route.size();
        swap(route[index1], route[index2]); // Hoán đổi hai thành phố
    }
}

// Hàm chính của thuật toán di truyền (Genetic Algorithm)
vector<int> geneticAlgorithm(vector<City>& cities, int populationSize, int generations) {
    vector<vector<int>> population = initializePopulation(populationSize, cities.size()); // Tạo quần thể ban đầu
    vector<int> bestRoute = population[0]; // Lưu hành trình tốt nhất
    double bestDistance = calculateRouteDistance(bestRoute, cities); // Tính khoảng cách tốt nhất

    for (int generation = 0; generation < generations; ++generation) {
        vector<vector<int>> newPopulation;

        for (int i = 0; i < populationSize; ++i) {
            vector<int> parent1 = selectParent(population, cities); // Chọn cha mẹ 1
            vector<int> parent2 = selectParent(population, cities); // Chọn cha mẹ 2
            vector<int> child = crossover(parent1, parent2); // Lai ghép để tạo con
            mutate(child); // Đột biến hành trình con
            newPopulation.push_back(child); // Thêm con vào quần thể mới

            // Cập nhật hành trình tốt nhất
            double childDistance = calculateRouteDistance(child, cities);
            if (childDistance < bestDistance) {
                bestDistance = childDistance;
                bestRoute = child;
            }
        }

        population = newPopulation; // Cập nhật quần thể

        // Hiển thị thông tin của thế hệ hiện tại
        cout << "Generation " << generation + 1 <<" "<< ": Best distance = " << bestDistance << endl;
        cout << "Route: ";
        for (int city : bestRoute) {
            cout << city <<" ";
        }
        cout << endl;
    }

    return bestRoute; // Trả về hành trình tốt nhất
}

int main() {
    srand(time(0)); // Khởi tạo seed cho số ngẫu nhiên

    int numCities = 10; // Số thành phố
    vector<City> cities(numCities);

    // Khởi tạo ngẫu nhiên tọa độ các thành phố
    for (int i = 0; i < numCities; ++i) {
        cities[i] = {rand() % 100, rand() % 100}; // Tọa độ trong phạm vi (0, 100)
        cout << "City " << i << ": (" << cities[i].x << ", " << cities[i].y << ")\n";
    }

    int populationSize = 100; // Kích thước quần thể
    int generations = 50; // Số thế hệ

    cout << "\nRunning Genetic Algorithm..." << endl;

    // Tìm hành trình tốt nhất
    vector<int> bestRoute = geneticAlgorithm(cities, populationSize, generations);

    // In kết quả tốt nhất
    cout << "\nBest route found:" << endl;
    for (int city : bestRoute) {
        cout << city << " -> ";
    }
    cout << bestRoute[0] << endl; // Quay về thành phố đầu tiên

    cout << "Total distance: " << calculateRouteDistance(bestRoute, cities) << endl;

    return 0;
}
