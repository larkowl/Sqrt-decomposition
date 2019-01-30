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

int string_to_int(const string &number) {
    stringstream ss;
    int result = 0;
    ss << number;
    ss >> result;
    return result;
}

bool building(int elements_count, ifstream& input) {
    elements.resize(static_cast<unsigned long>(elements_count));
    sortedblocks.resize(static_cast<unsigned long>(elements_count));
    blockSize = static_cast<int>(ceil(sqrt(elements_count)));
    blocksCount = static_cast<int>(ceil(elements_count / (double) blockSize));
    blocksMaximum.resize(static_cast<unsigned long>(blocksCount), MIN);
    blocksPromises.resize(static_cast<unsigned long>(blocksCount), 0);
    blocksSums.resize(static_cast<unsigned long>(blocksCount), 0);
    for (int i = 0; i < elements_count; ++i) {
        string element;
        input >> element;
        if (check_integer(element))
            elements[i] = string_to_int(element);
        else
            return false;
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
    return true;
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

void query_answer(long long int first_answer, ifstream& answers) {
    if (answers.is_open()) {
        long long int second_answer;
        answers >> second_answer;
        if (first_answer == second_answer)
            cout << "OK" << std::endl;
        else
            cout << "WRONG ANSWER" << std::endl;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    int elements_count, queries_count;
    string element, queries;
    ifstream input = std::ifstream("input.rtf");
    ifstream answers = std::ifstream("answers.rtf");
    if (!input.is_open()) {
        cout << "Some kind error with test file";
        return 0;
    }
    if (!answers.is_open()) {
        cout << "Some kind error with answers file";
        return 0;
    }
    input >> element >> queries;
    if (!check_integer(element) || !check_integer(queries)) {
        cout << "Input file error";
        return 0;
    }
    elements_count = string_to_int(element);
    if (elements_count < 1) {
        cout << "Input file error";
        return 0;
    }
    queries_count = string_to_int(queries);
    if (queries_count < 1) {
        cout << "Input file error";
        return 0;
    }
    if (!building(elements_count, input)) {
        cout << "Input file error";
        return 0;
    }

    for (int j = 0; j < queries_count; ++j) {
        string choice, left_s, right_s, value_s;
        input >> choice;
        if (!(choice == "add" || choice == "count" || choice == "sum" || choice == "maximum")) {
            cout << "Input file error";
            return 0;
        }
        input >> left_s >> right_s;
        int left, right, value;
        if (!check_integer(left_s) || !check_integer(right_s)) {
            cout << "Input file error";
            return 0;
        }
        left = string_to_int(left_s);
        if (left < 1 || left > elements_count) {
            cout << "Input file error";
            return 0;
        }
        right = string_to_int(right_s);
        if (right < 1 || right > elements_count) {
            cout << "Input file error";
            return 0;
        }
        if (choice == "add" || choice == "count") {
            input >> value_s;
            if (!check_integer(value_s)) {
                cout << "Input file error";
                return 0;
            }
            value = string_to_int(value_s);
        }
        if (choice == "add") {
            ChangeQuery(left - 1, right - 1, value);
        } else if (choice == "maximum") {
            long long answer = MaxQuery(left - 1, right - 1);
            query_answer(answer, answers);
        } else if (choice == "sum") {
            long long answer = SumQuery(left - 1, right - 1);
            query_answer(answer, answers);
        } else if (choice == "count") {
            long long answer = CountQuery(left - 1, right - 1, value);
            query_answer(answer, answers);
        }
    }
    input.close();
    answers.close();
    return 0;
}
