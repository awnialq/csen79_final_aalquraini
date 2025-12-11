/*
 * Sin-Yaw Wang <swang24@scu.edu
 */
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>   // std::make_shared
#include <new>      // std::bad_alloc
#include <algorithm> // std::sort, std::unique
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

// Helper function to convert attribute index to name
static const char* attrName(int attr) {
    switch (attr) {
        case 0: return "x";
        case 1: return "y";
        case 2: return "z";
        default: return "?";
    }
}

ostream & operator<<(ostream &os, const TreeData &t) { 
    // Print decision tree in a hierarchical, human-readable format
    // Using indentation to show tree structure
    
    struct StackFrame {
        const TreeData* node;
        int depth;
        string prefix;      // For tree branches (e.g., "├── " or "└── ")
        string childPrefix; // For children's continuation lines
        bool isLeft;
        int state; // 0=print self, 1=left done, 2=right done
    };
    
    os << "\n=== Decision Tree ===\n\n";
    
    vector<StackFrame> stack;
    stack.push_back({&t, 0, "", "", true, 0});
    
    while (!stack.empty()) {
        StackFrame& frame = stack.back();
        const TreeData* node = frame.node;
        
        if (frame.state == 0) {
            // Print this node
            os << frame.prefix;
            
            // Determine the majority category and counts
            int redCount = 0, blueCount = 0;
            for (const auto& rec : node->records) {
                if (rec->getCategory() == Record::RED)
                    ++redCount;
                else
                    ++blueCount;
            }
            const char* category = (redCount >= blueCount) ? "Red" : "Blue";
            
            if (node->left == nullptr && node->right == nullptr) {
                // Leaf node
                os << "[" << category << "] "
                   << "(samples: " << node->records.size() 
                   << ", red: " << redCount << ", blue: " << blueCount
                   << ", gini: " << fixed << setprecision(3) << node->selfImpurity << ")\n";
                stack.pop_back();
                continue;
            } else {
                // Internal node - show split condition as a question
                os << "Is " << attrName(node->splitAttr) << " <= " << node->splitAt << "?"
                   << " (samples: " << node->records.size() 
                   << ", gini: " << fixed << setprecision(3) << node->selfImpurity << ")\n";
            }
            frame.state = 1;
        }
        
        if (frame.state == 1) {
            // Process left child (yes branch)
            if (node->left != nullptr) {
                frame.state = 2;
                string newPrefix = frame.childPrefix + "├── Yes: ";
                string newChildPrefix = frame.childPrefix + "│   ";
                stack.push_back({node->left.get(), frame.depth + 1, newPrefix, newChildPrefix, true, 0});
                continue;
            } else {
                frame.state = 2;
            }
        }
        
        if (frame.state == 2) {
            // Process right child (no branch)
            if (node->right != nullptr) {
                frame.state = 3;
                string newPrefix = frame.childPrefix + "└── No:  ";
                string newChildPrefix = frame.childPrefix + "    ";
                stack.push_back({node->right.get(), frame.depth + 1, newPrefix, newChildPrefix, false, 0});
                continue;
            } else {
                frame.state = 3;
            }
        }
        
        if (frame.state == 3) {
            // Done with this node
            stack.pop_back();
        }
    }
    
    os << "\n";
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
    if (nRecords < 2)
        return bestSplit; // not enough records to split

    // sort the records based on the value of the given attribute
    vector<Record::AttrType> sortedValues;
    for (const auto& rec : records) {
        sortedValues.push_back(rec->getAttribute(whichAttr));
    }
    sort(sortedValues.begin(), sortedValues.end());
    // get rid of any duplicate points for the purpose of finding split points
    sortedValues.erase(unique(sortedValues.begin(), sortedValues.end()), sortedValues.end());

    if (sortedValues.size() < 2)
        return bestSplit; // sanity check. if all values are the same, you cannot split

    // go thru each point and find a local minima using circular buffer
    const int bufSize = 3;
    HuntType buffer[bufSize];
    int bufIndex = 0;
    int filled = 0;  // track how many buffer slots are filled

    for (size_t i = 0; i < sortedValues.size(); ++i) {
        // compute impurity for current value
        Record::AttrType currentValue = sortedValues[i];
        double currentImpurity = combineImpurity(whichAttr, currentValue);
        
        // add to circular buffer
        buffer[bufIndex].first = currentValue;
        buffer[bufIndex].second = currentImpurity;
        
        // track overall minimum
        if (currentImpurity < bestSplit.second) {
            bestSplit.first = currentValue;
            bestSplit.second = currentImpurity;
        }
        
        if (filled < bufSize) {
            ++filled;
        }
        
        // once buffer is full, check for local minimum at the middle position
        if (filled == bufSize) {
            // buffer layout after adding at bufIndex:
            // oldest = (bufIndex+1)%3, middle = (bufIndex+2)%3, newest = bufIndex
            HuntType &prev = buffer[(bufIndex + 1) % 3];
            HuntType &curr = buffer[(bufIndex + 2) % 3];
            HuntType &next = buffer[bufIndex];
            
            if (curr.second < prev.second && curr.second < next.second) {
                // Found a local minimum
                if (curr.second < bestSplit.second) {
                    bestSplit = curr;
                }
            }
        }
        
        bufIndex = (bufIndex + 1) % 3;
    }

    return bestSplit;
}

// Make decision tree recursively from the top down
void TreeData::makeTree() {
    // compute impurity of the current sub-tree
    try {
        selfImpurity = doImpurity();
    } catch (exception& e) {    // doImpurity can throw if records is empty
        selfImpurity = 1.0;
    }

    // if impurity is 0 (pure node) or not enough records, stop
    if (selfImpurity == 0.0 || records.size() < 2) {
        return; // basically means you are a leaf node
    }

    // no more attributes available (all attributes have be used earlier in the tree at this point)
    if (availableAttr.empty()) {
        return; // basically means you have a leaf node
    }

    // hunt for the best split for each attribute available at that level
    HuntType bestSplit;
    bestSplit.second = 1.0;
    int bestAttr = -1;

    for (int attr : availableAttr) {
        HuntType attrSplit = huntSplit(attr);
        if (attrSplit.second < bestSplit.second) {
            bestSplit = attrSplit;
            bestAttr = attr;
        }
    }

    // check if there is no improvement
    if (bestAttr == -1 || bestSplit.second >= selfImpurity) {
        return; // no beneficial split found
    }

    // save the best split info
    splitAttr = bestAttr;
    splitAt = bestSplit.first;
    improvedImpurity = bestSplit.second;

    // do the actual ssplitting
    TreePair children = split(splitAttr, splitAt);

    // if one side is empty, do not split as you will not have any records on one side. Basically a leaf node
    if (children.first.records.empty() || children.second.records.empty()) {
        splitAttr = Record::NOATTR;
        return; 
    }

    // Create child nodes
    left = make_shared<TreeData>(children.first);
    right = make_shared<TreeData>(children.second);

    // Set child levels
    left->level = level + 1;
    right->level = level + 1;

    // Cull the split attribute from available attributes to prevent overfitting
    // Each attribute can only be used once per path from root to leaf
    vector<int> remainingAttr;
    for (int attr : availableAttr) {
        if (attr != splitAttr) {
            remainingAttr.push_back(attr);
        }
    }
    left->availableAttr = remainingAttr;
    right->availableAttr = remainingAttr;

    // recurisvely build subtrees
    left->makeTree();
    right->makeTree();

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
