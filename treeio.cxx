/*
 * Sin-Yaw Wang <swang24@scu.edu
 */
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>   // std::make_shared
#include <new>      // std::bad_alloc
#include "record.h"
#include "treedata.h"
namespace csen79 {
using namespace std;

// This is the real "constructor".
// Read data from stream and initialize the tree structure to start
istream & operator>>(istream &is, TreeData &t) {
    // Reset container state in case this TreeData is reused
    t.records.clear();
    t.availableAttr.clear();

    Record buf;
    while (is >> buf)  {
        // Student implement
        try {
            t.records.push_back(make_shared<Record>(buf));
        } catch (bad_alloc& e) {
            cerr << "Memory failed to allocate" << e.what() << endl;
            break;
        }
    }

    if (!t.records.empty()) {
        for (int i = 0; i < Record::NATTRIBUTES; ++i)
            t.availableAttr.push_back(i);
    }
    return is;
}

ostream & operator<<(ostream &os, const TreeData &t) { 
    // Student implement
    return os;
}

/*
 * Hunt for optimal split point on attribute whichAttr
 * Return a pair of (split value, impurity)
 * 
 * To find a inflection point, we need to observe a downward tick followed by an upward.
 * This means we must have 3 data points seperated, each, by a step difference.
 * We use a circular buffer to store the 3 points, and keep replacing the oldest point
 * with a new point until we find an inflection.  Continue for more, in case of local optima
 * until the end of the range.
 */
const TreeData::HuntType TreeData::huntSplit(const int whichAttr) const {
    // Student implement
    HuntType bestSplit;
    bestSplit.second = 1.0; // worst impurity
    const int nRecords = static_cast<int>(records.size());
    if (nRecords < 3)
        return bestSplit; // not enough records to split
    struct Point {
        Record::AttrType value;
        double impurity;
    };
    Point buffer[3];
    int bufIndex = 0;
    // Initialize circular buffer with first 3 points
    for (int i = 0; i < 3; ++i) {
        buffer[i].value = records[i]->getAttribute(whichAttr);
        buffer[i].impurity = combineImpurity(whichAttr, buffer[i].value);
    }
    // Scan through remaining records
    for (int i = 3; i < nRecords; ++i) {
        Record::AttrType currentValue = records[i]->getAttribute(whichAttr);
        double currentImpurity = combineImpurity(whichAttr, currentValue);
        // Check for inflection point
        Point &prev = buffer[(bufIndex + 2) % 3];
        Point &curr = buffer[bufIndex];
        Point &next = buffer[(bufIndex + 1) % 3];
        if (curr.impurity < prev.impurity && curr.impurity < next.impurity) {
            // Found a local minimum
            if (curr.impurity < bestSplit.second) {
                bestSplit.first = curr.value;
                bestSplit.second = curr.impurity;
            }
        }
        // Update circular buffer
        buffer[bufIndex].value = currentValue;
        buffer[bufIndex].impurity = currentImpurity;
        bufIndex = (bufIndex + 1) % 3;
    }
    

    return bestSplit;
}

// Make decision tree, recursively descend
void TreeData::makeTree() {
    // Student implement
}

// return a pair of TreeData separated by the split value on the given attribute
TreeData::TreePair TreeData::split(const int i, const Record::AttrType d) const {
    TreeData left, right;

    for (const auto & rec : records) {  //split each record into a right or left tree depending on attribute i
        if (rec->getAttribute(i) <= d)
            left.records.push_back(rec);
        else
            right.records.push_back(rec);
    } 

    return TreePair(left, right);
}

}   // namespace
