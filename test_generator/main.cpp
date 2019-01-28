#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

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
    this->elements_count = 333;
    values.resize(static_cast<unsigned long>(elements_count));
    for (int i = 0; i < this->elements_count; ++i) {
        values[i] = rand() % 500 + 1;
        int choice = rand() % 2 + 1;
        if (choice == 1)
            values[i] *= -1;
    }
    this->queries_count = 499;
    queries.resize(static_cast<unsigned long>(queries_count));
    for (int i = 0; i < this->queries_count - 1; ++i) {
        Query query;
        int choice1 = 1;
        if (choice1 == 1) {
            query.choice = "add";
        }
        query.left = rand() % elements_count + 1;
        while (true) {
            query.right = rand() % elements_count + 1;
            if (query.right >= query.left)
                break;
        }
        query.value = rand() % 500 + 1;
        int choice = rand() % 2 + 1;
        if (choice == 1)
            query.value *= -1;
        queries[i] = query;
    }
    Query query;
    query.choice = "count";
    query.left = rand() % elements_count + 1;
    while (true) {
        query.right = rand() % elements_count + 1;
        if (query.right >= query.left)
            break;
    }
    query.value = rand() % 500 + 1;
    int choice = rand() % 2 + 1;
    if (choice == 1)
        query.value *= -1;
    queries[queries_count - 1] = query;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    ofstream output;
    output.open("input.rtf");
    Test* test = new Test();
    output << test->elements_count << ' ' << test->queries_count << endl;
    for (int i = 0; i < test->elements_count; ++i) {
        output << test->values[i] << ' ';
        if (i % 50 == 0)
            output << endl;
    }
    for (int i = 0; i < test->queries_count; ++i) {
        output << test->queries[i].choice << ' '
        << test->queries[i].left << ' ' << test->queries[i].right << ' ';
        if (test->queries[i].choice == "add" || test->queries[i].choice == "count")
            output << test->queries[i].value;
        output << endl;
    }
    output.close();

    return 0;
}