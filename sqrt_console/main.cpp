#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include <ctime>
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

void building(int elements_count, ifstream& input) {
    elements.resize(elements_count);
    sortedblocks.resize(elements_count);
    blockSize = static_cast<int>(ceil(sqrt(elements_count)));
    blocksCount = static_cast<int>(ceil(elements_count / (double) blockSize));
    blocksMaximum.resize(static_cast<unsigned long>(blocksCount), MIN);
    blocksPromises.resize(static_cast<unsigned long>(blocksCount), 0);
    blocksSums.resize(static_cast<unsigned long>(blocksCount), 0);
    for (int i = 0; i < elements_count; ++i) {
        input >> elements[i];
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
    ifstream input = std::ifstream("input.rtf");
    ifstream answers = std::ifstream("answers.rtf");
    int elements_count, queries_count;
    input >> elements_count >> queries_count;
    building(elements_count, input);
    for (int j = 0; j < queries_count; ++j) {
        string choice;
        input >> choice;
        if (choice == "add") {
            int left, right, value;
            input >> left >> right >> value;
            ChangeQuery(left - 1, right - 1, value);
        } else if (choice == "maximum") {
            int left, right;
            input >> left >> right;
            long long answer = MaxQuery(left - 1, right - 1);
            long long answer2;
            answers >> answer2;
            if (answer == answer2)
                cout << "OK" << endl;
            else {
                cout << "WRONG ANSWER"<< endl;
            }
        } else if (choice == "sum") {
            int left, right;
            input >> left >> right;
            if (j == 93) {
                bool check = true;
            }
            long long answer = SumQuery(left - 1, right - 1);
            long long answer2;
            answers >> answer2;
            if (answer == answer2)
                cout << "OK" << endl;
            else {
                bool check = false;
                cout << "WRONG ANSWER" << endl;
            }
        } else {
            int left, right, value;
            input >> left >> right >> value;
            long long answer = CountQuery(left - 1, right - 1, value);
            long long answer2;
            answers >> answer2;
            if (answer == answer2)
                cout << "OK" << endl;
            else
                cout << "WRONG ANSWER"<< endl;
        }
    }
    input.close();
    answers.close();
    return 0;
}
