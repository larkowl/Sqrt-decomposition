#include <iostream>
#include <istream>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

using namespace std;
long long start = clock();

struct Query {
    string choice;
    int left;
    int right;
    int value;
};

class Test {
public:
    int elements_count;
    int queries_count;
    vector<Query> queries;
    vector<int> values;
    Test();
};

Test::Test() {
    this->elements_count = rand() % 100000 + 1;
    values.resize(elements_count);
    for (int i = 0; i < this->elements_count; ++i) {
        values[i] = rand() % 10000000 + 1;
    }
    this->queries_count = rand() % 1000000 + 1;
    queries.resize(queries_count);
    for (int i = 0; i < this->queries_count; ++i) {
        Query query;
        int command = rand() % 4 + 1;
        if (command == 1)
            query.choice = "sum";
        else if (command == 2)
            query.choice = "maximum";
        else if (command == 3)
            query.choice = "add";
        else
            query.choice = "count";
        query.left = rand() % elements_count + 1;
        while (true) {
            query.right = rand() % elements_count + 1;
            if (query.right >= query.left)
                break;
        }
        query.value = rand() % 10000000 + 1;
        queries[i] = query;
    }
}

vector<long long> elements;
vector<long long> copied;
vector<long long> sortedblocks;
vector<long long> blocksMaximum;
vector<long long> blocksSums;
vector<long long> blocksPromises;
int blocksCount;
int blockSize;

void building(int elements_count) {
    elements.resize(elements_count);
    copied.resize(elements_count);
    sortedblocks.resize(elements_count);
    blockSize = static_cast<int>(ceil(sqrt(elements_count)));
    blocksCount = static_cast<int>(ceil(elements_count / (double) blockSize));
    blocksMaximum.resize(static_cast<unsigned long>(blocksCount), -1000000001);
    blocksPromises.resize(static_cast<unsigned long>(blocksCount), 0);
    blocksSums.resize(static_cast<unsigned long>(blocksCount), 0);
    for (int i = 0; i < elements_count; ++i) {
        cin >> elements[i];
        copied[i] = elements[i];
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
    long long answer = -1;
    for (int i = left; i < right + 1; ++i) {
        int ind = i / blockSize;
        if (i % blockSize == 0 && (i + blockSize <= right)) {
            answer = max(answer, blocksMaximum[ind]);
            i += blockSize - 1;
            continue;
        }
        answer = max(answer, elements[i] + blocksPromises[ind]);
    }
    return answer;
}

long long SumQuery(int left, int right) {
    long long answer = 0;
    for (int i = left; i < right + 1; ++i) {
        int ind = i / blockSize;
        if (i % blockSize == 0 && (i + blockSize <= right)) {
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
        if (i % blockSize == 0 && i + blockSize <= right) {
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
    blocksMaximum[ind] = -1;
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
        if (i % blockSize == 0 && (i + blockSize <= right || (ind == blocksCount - 1
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
    cin >> elements_count >> queries_count;
    building(elements_count);
    for (int j = 0; j < queries_count; ++j) {
        string choice;
        cin >> choice;
        if (choice == "add") {
            int left, right, value;
            cin >> left >> right >> value;
            ChangeQuery(left - 1, right - 1, value);
            for (int i = left - 1; i < right; ++i) {
                copied[i] += value;
            }
            cout << "change" << endl;
        } else if (choice == "maximum") {
            int left, right;
            cin >> left >> right;
            long long answer = MaxQuery(left - 1, right - 1);
            long long answer2 = -1;
            for (int i = left - 1; i < right; ++i) {
                answer2 = max (answer2, copied[i]);
            }
            if (answer == answer2)
                cout << "OK" << endl;
            else {
                cout << "ERROR" << endl;
            }
        } else if (choice == "sum") {
            int left, right;
            cin >> left >> right;
            long long answer = SumQuery(left - 1, right - 1);
            long long answer2 = 0;
            for (int i = left - 1; i < right; ++i) {
                answer2 += copied[i];
            }
            if (answer == answer2)
                cout << "OK" << endl;
            else
                cout << "ERROR" << ' ' << endl;
        } else {
            int left, right, value;
            cin >> left >> right >> value;
            long long answer = CountQuery(left - 1, right - 1, value);
            long long answer2 = 0;
            for (int i = left - 1; i < right; ++i) {
                if (copied[i] <= value)
                    ++answer2;
            }
            if (answer == answer2)
                cout << "OK" << endl;
            else
                cout << "ERROR" << ' ' << endl;
        }
    }
    cout << elements_count << ' ' << queries_count << ' ' << start;
    return 0;
}
