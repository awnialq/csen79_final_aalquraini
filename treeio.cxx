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
    return HuntType();
}

// Make decision tree, recursively descend
void TreeData::makeTree() {
    // Student implement
}

// return a pair of TreeData separated by the split value on the given attribute
TreeData::TreePair TreeData::split(const int i, const Record::AttrType d) const {
    // Student implement
    return TreePair();
}

}   // namespace
