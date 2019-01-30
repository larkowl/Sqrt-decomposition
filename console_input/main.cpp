#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;
const long long int MIN = -10000000000000001;

vector<long long int> elements;
vector<long long int> sortedblocks;
vector<long long int> blocksMaximum;
vector<long long int> blocksSums;
vector<long long int> blocksPromises;
int blocksCount;
int blockSize;

bool check_integer(string number) {
    for (int i = 0; i < number.length(); ++i) {
        if (!i && number[i] == '-')
            continue;
        if (number[i] < 48 || number[i] > 57)
            return false;
    }
    return true;
}

int input_integer(string to_check) {
    bool check = false;
    string str;
    int number = 0;
    while (!check) {
        std::cin >> str;
        if (check_integer(str)) {
            stringstream ss;
            ss << str;
            ss >> number;
            if ((to_check == "size" && number < 0) || ((to_check == "left" || to_check == "right") && number < 1)
                            || ((to_check == "left" || to_check == "right") && number > elements.size())) {
                std::cout << "Wrong input" << std::endl;
                continue;
            }
            check = true;
        }
        else
            std::cout << "Wrong input" << std::endl;
    }
    return number;
}

void building(int elements_count) {
    elements.resize(elements_count);
    sortedblocks.resize(elements_count);
    blockSize = static_cast<int>(ceil(sqrt(elements_count)));
    blocksCount = static_cast<int>(ceil(elements_count / (double) blockSize));
    blocksMaximum.resize(static_cast<unsigned long>(blocksCount), MIN);
    blocksPromises.resize(static_cast<unsigned long>(blocksCount), 0);
    blocksSums.resize(static_cast<unsigned long>(blocksCount), 0);
    std::cout << "Elements input " << std::endl;
    for (int i = 0; i < elements_count; ++i) {
        std::cout << "Enter element " << i + 1 << ": ";
        elements[i] = input_integer("element");
        sortedblocks[i] = elements[i];
        int ind = i / blockSize;
        blocksSums[ind] += elements[i];
        blocksMaximum[ind] = max(blocksMaximum[ind], elements[i]);
    }
    for (int i = 0; i < blocksCount; ++i) {
        auto it = sortedblocks.begin() + i * blockSize;
        if (i == blocksCount - 1 && elements_count % blockSize) {
            sort(it, it + elements_count % blockSize);
        } else {
            sort(it, it + blockSize);
        }
    }
}

long long MaxQuery(int left, int right) {
    long long answer = MIN;
    for (int i = left; i < right + 1; ++i) {
        int ind = i / blockSize;
        if (i % blockSize == 0 && (i + blockSize - 1 <= right)) {
            answer = max(answer, blocksMaximum[ind]);
            i += blockSize - 1;
            continue;
        }
        answer = max(answer, elements[i] + blocksPromises[ind]);
    }
    return answer;
}

long long SumQuery(int left, int right) {
    long long int answer = 0;
    for (int i = left; i < right + 1; ++i) {
        int ind = i / blockSize;
        if (i % blockSize == 0 && (i + blockSize - 1 <= right)) {
            answer += blocksSums[ind];
            i += blockSize - 1;
            continue;
        }
        answer += elements[i] + blocksPromises[ind];
    }
    return answer;
}

int BinarySearch(int left, int right, int value, int ind) {
    int block = left / blockSize;
    if (left == right) {
        return sortedblocks[left] + blocksPromises[block] <= value ? 1 : 0;
    }
    int middle = (left + right) / 2;
    if (sortedblocks[middle] + blocksPromises[block] <= value) {
        int answer = middle - ind + 1;
        answer += BinarySearch(middle + 1, right, value, middle + 1);
        return answer;
    }
    return BinarySearch(left, middle, value, ind);
}

int CountQuery(int left, int right, int value) {
    int answer = 0;
    for (int i = left; i < right + 1; ++i) {
        int ind = i / blockSize;
        if (i % blockSize == 0 && i + blockSize - 1 <= right) {
            answer += BinarySearch(i, i + blockSize - 1, value, i / blockSize * blockSize);
            i += blockSize - 1;
            continue;
        }
        if (elements[i] + blocksPromises[ind] <= value)
            ++answer;
    }
    return answer;
}

void BruteForceMaxChanging(int ind, int value) {
    if (value >= 0)
        return;
    blocksMaximum[ind] = MIN;
    for (int j = 0; j < blockSize && j + ind * blockSize < elements.size(); ++j) {
        blocksMaximum[ind] = max(blocksMaximum[ind], elements[j + ind * blockSize] + blocksPromises[ind]);
    }
}

void SortedBlocksChanging(int ind) {
    for (int i = 0; i < blockSize && i + ind * blockSize < elements.size(); ++i) {
        sortedblocks[i + ind * blockSize] = elements[i + ind * blockSize];
    }
    auto it = sortedblocks.begin() + ind * blockSize;
    if (ind == blocksCount - 1 && elements.size() % blockSize != 0) {
        sort(it, it + elements.size() % blockSize);
    } else {
        sort(it, it + blockSize);
    }
}

void ChangeQuery(int left, int right, int value) {
    int first_tail = -1, second_tail = -1;
    bool flag = false;
    for (int i = left; i < right + 1; ++i) {
        int ind = i / blockSize;
        if (i % blockSize == 0 && (i + blockSize - 1 <= right || (ind == blocksCount - 1
                                                                  && right + 1 == elements.size()))) {
            blocksSums[ind] += value * blockSize;
            blocksPromises[ind] += value;
            blocksMaximum[ind] += value;
            i += blockSize - 1;
            continue;
        }
        if (!flag) {
            first_tail = ind;
            flag = true;
        } else {
            second_tail = ind;
        }
        blocksSums[ind] += value;
        elements[i] += value;
        if (value > 0)
            blocksMaximum[ind] = max(blocksMaximum[ind], elements[i] + blocksPromises[ind]);
    }
    if (first_tail != -1) {
        SortedBlocksChanging(first_tail);
        BruteForceMaxChanging(first_tail, value);
    }
    if (second_tail != -1) {
        SortedBlocksChanging(second_tail);
        BruteForceMaxChanging(second_tail, value);
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    int elements_count, queries_count;
    std::cout << "Enter elements amount: ";
    elements_count = input_integer("size");

    std::cout << "Enter queries amount: ";
    queries_count = input_integer("size");
    building(elements_count);

    for (int query = 0; query < queries_count; ++query) {
        string command;
        int left, right, value;
        std::cout << "Enter command: ";
        std::cin >> command;
        std::cout << "Enter left border: ";
        left = input_integer("left");
        std::cout << "Enter right border: ";
        right = input_integer("right");
        if (command == "add") {
            std::cout << "Enter value: ";
            value = input_integer("value");
            ChangeQuery(left - 1, right - 1, value);
        } else if (command == "maximum") {
            std::cout << MaxQuery(left - 1, right - 1) << std::endl;
        } else if (command == "count") {
            std::cout << "Enter value: ";
            value = input_integer("value");
            std::cout << CountQuery(left - 1, right - 1, value) << std::endl;
        } else if (command == "sum") {
            std::cout << SumQuery(left - 1, right - 1) << std::endl;
        } else {
            std::cout << "Wrong command" << std::endl;
        }
    }
    return 0;
}