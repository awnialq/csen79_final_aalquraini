/*
 * Sin-Yaw Wang <swang24@scu.edu
 * Gini Impurity Test Program
 */
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include "record.h"
#include "treedata.h"

using namespace std;
using namespace csen79;

int main(int argc, char *argv[]) {
    TreeData giniData;
    cin >> giniData;
    cout << "Set size: " << giniData.size() << endl;
    giniData.makeTree();
    cout << giniData;
    return 0;
}
