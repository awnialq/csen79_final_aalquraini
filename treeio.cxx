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

    // Collect and sort unique attribute values
    vector<Record::AttrType> sortedValues;
    for (const auto& rec : records) {
        sortedValues.push_back(rec->getAttribute(whichAttr));
    }
    sort(sortedValues.begin(), sortedValues.end());
    // Remove duplicates
    sortedValues.erase(unique(sortedValues.begin(), sortedValues.end()), sortedValues.end());

    if (sortedValues.size() < 2)
        return bestSplit; // All values are the same, cannot split

    // Use circular buffer to find inflection points (local minima)
    const int bufSize = 3;
    if (sortedValues.size() < 3) {
        // With only 2 unique values, just compute impurity for the first split point
        bestSplit.first = sortedValues[0];
        bestSplit.second = combineImpurity(whichAttr, sortedValues[0]);
        return bestSplit;
    }

    HuntType buffer[bufSize];
    // Initialize circular buffer with first 3 split points
    for (int i = 0; i < bufSize; ++i) {
        buffer[i].first = sortedValues[i];
        buffer[i].second = combineImpurity(whichAttr, sortedValues[i]);
        // Track the minimum as we go
        if (buffer[i].second < bestSplit.second) {
            bestSplit = buffer[i];
        }
    }

    int bufIndex = 0;
    // Scan through remaining sorted unique values
    for (size_t i = 3; i < sortedValues.size(); ++i) {
        // Check for inflection point (local minimum) at the middle position
        // buffer layout: [bufIndex] = oldest, [(bufIndex+1)%3] = middle, [(bufIndex+2)%3] = newest
        HuntType &prev = buffer[bufIndex];
        HuntType &curr = buffer[(bufIndex + 1) % 3];
        HuntType &next = buffer[(bufIndex + 2) % 3];
        
        if (curr.second < prev.second && curr.second < next.second) {
            // Found a local minimum
            if (curr.second < bestSplit.second) {
                bestSplit = curr;
            }
        }

        // Update circular buffer with new value
        Record::AttrType currentValue = sortedValues[i];
        double currentImpurity = combineImpurity(whichAttr, currentValue);
        buffer[bufIndex].first = currentValue;
        buffer[bufIndex].second = currentImpurity;
        
        // Track overall minimum
        if (currentImpurity < bestSplit.second) {
            bestSplit.first = currentValue;
            bestSplit.second = currentImpurity;
        }
        
        bufIndex = (bufIndex + 1) % 3;
    }

    // Check the last middle element for inflection
    HuntType &prev = buffer[bufIndex];
    HuntType &curr = buffer[(bufIndex + 1) % 3];
    HuntType &next = buffer[(bufIndex + 2) % 3];
    if (curr.second < prev.second && curr.second < next.second) {
        if (curr.second < bestSplit.second) {
            bestSplit = curr;
        }
    }

    return bestSplit;
}

// Make decision tree, recursively descend
void TreeData::makeTree() {
    // Compute self impurity
    try {
        selfImpurity = doImpurity();
    } catch (exception& e) {
        selfImpurity = 1.0;
    }

    // Base case: if impurity is 0 (pure node) or not enough records, stop
    if (selfImpurity == 0.0 || records.size() < 2) {
        return; // Leaf node - pure or too small
    }

    // Base case: no more attributes available (all culled on this path)
    if (availableAttr.empty()) {
        return; // Leaf node - exhausted all attributes
    }

    // Find the best attribute and split point among available attributes
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

    // If no improvement possible, stop
    if (bestAttr == -1 || bestSplit.second >= selfImpurity) {
        return; // Leaf node - no beneficial split found
    }

    // Store split information
    splitAttr = bestAttr;
    splitAt = bestSplit.first;
    improvedImpurity = bestSplit.second;

    // Split the data
    TreePair children = split(splitAttr, splitAt);

    // Don't create children if one side would be empty
    if (children.first.records.empty() || children.second.records.empty()) {
        splitAttr = Record::NOATTR;
        return; // Leaf node - split would create empty child
    }

    // Create child nodes
    left = make_shared<TreeData>(children.first);
    right = make_shared<TreeData>(children.second);

    // Set child levels
    left->level = level + 1;
    right->level = level + 1;

    // Pass ALL attributes to children - allow reuse with different split points
    // This achieves the lowest possible combined Gini impurity
    left->availableAttr = availableAttr;
    right->availableAttr = availableAttr;

    // Recursively build subtrees
    left->makeTree();
    right->makeTree();

    // After children are built, cull attributes that are exhausted
    // (i.e., can no longer provide beneficial splits in the subtree)
    // This happens naturally since huntSplit returns 1.0 for attributes
    // that can't split, and we skip splits that don't improve impurity
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
