/*
 * Sin-Yaw Wang <swang24@scu.edu
 * Gini Impurity Test Program
 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include "record.h"
#include "treedata.h"

using namespace std;
using namespace csen79;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <training_file> <test_file>" << endl;
        cerr << "You can use the same file for both: " << argv[0] << " data1.txt data1.txt" << endl;
        return 1;
    }

    // Open training file and build tree
    ifstream trainFile(argv[1]);
    if (!trainFile) {
        cerr << "Error: Cannot open training file: " << argv[1] << endl;
        return 1;
    }

    TreeData giniData;
    trainFile >> giniData;
    trainFile.close();

    cout << "Set size: " << giniData.size() << endl;
    giniData.makeTree();
    cout << giniData;

    // Open test file (can be the same file)
    ifstream testFile(argv[2]);
    if (!testFile) {
        cerr << "Error: Cannot open test file: " << argv[2] << endl;
        return 1;
    }

    // Test each record using decide
    cout << "\nTesting decide function...\n" << endl;
    Record testRecord;
    int correct = 0;
    int total = 0;

    while (testFile >> testRecord) {
        Record::Category predicted = giniData.decide(testRecord);
        Record::Category actual = testRecord.getCategory();
        bool isCorrect = (predicted == actual);
        if (isCorrect) ++correct;
        ++total;

        cout << "Record: (" << testRecord.getAttribute(0) << ", " << testRecord.getAttribute(1) << ", " << testRecord.getAttribute(2) << ") - "
             << "Actual: " << (actual == Record::RED ? "Red" : "Blue") << ", "
             << "Predicted: " << (predicted == Record::RED ? "Red" : "Blue") << " "
             << (isCorrect ? "[CORRECT]" : "[WRONG]") << endl;
    }

    cout << "\nAccuracy: " << correct << "/" << total << " (" << fixed << setprecision(2) << (100.0 * correct / total) << "%)" << endl;


    return 0;
}
