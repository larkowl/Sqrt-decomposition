#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <ctime>
#include <string>
#include <algorithm>

using namespace std;
const long long int MIN = -10000000000000001;

int main() {
    std::ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    ifstream input = ifstream("input.rtf");
    ofstream output = ofstream("answers.rtf");
    int elements_count, queries_count;
    input >> elements_count >> queries_count;
    vector<long long int> elements(static_cast<unsigned long>(elements_count));
    for (int i = 0; i < elements_count; ++i) {
        input >> elements[i];
    }
    for (int j = 0; j < queries_count; ++j) {
        string command;
        input >> command;
        if (command == "add") {
            int left, right, value;
            input >> left >> right >> value;
            for (int i = left - 1; i < right; ++i) {
                elements[i] += value;
            }
        } else if (command == "maximum") {
            int left, right;
            input >> left >> right;
            long long int answer = MIN;

            for (int i = left - 1; i < right; ++i) {
                answer = max(answer, elements[i]);
            }
            output << answer << endl;
        } else if (command == "sum") {
            int left, right;
            input >> left >> right;
            if (j == 93) {
                bool check = true;
            }
            long long int answer = 0;
            for (int i = left - 1; i < right; ++i) {
                answer += elements[i];
            }
            output << answer << endl;
        } else {
            int left, right, value;
            input >> left >> right >> value;
            long long int answer = 0;
            for (int i = left - 1; i < right; ++i) {
                if (elements[i] <= value)
                    ++answer;
            }
            output << answer << endl;
        }
    }
    input.close();
    output.close();
    return 0;
}