/*
 * Sin-Yaw Wang <swang24@scu.edu
 * Compute Impurity/Entropy of a dataset and find optimal split point
 */
#include <stdexcept>
#include <utility>
#include <map>
#include <vector>
#include <memory>
#include <cmath>       // std::fabs
#include "record.h"
#include "treedata.h"

namespace csen79 {
using namespace std;

// Compute Gini Impurity of the dataset
const double TreeData::doImpurity() const {
    if (records.empty())
        throw runtime_error("No records available to compute Gini impurity.");

    /*
     * Use std::map (associative array) to compute the classification population.
     * This method is independent of the number of categories.
     */
    map<Record::Category, int> counter;
    for (const auto & rec : records) 
        counter[rec->getCategory()]++;

    double total = static_cast<double>(records.size());
    double sum = 0.0;
    for (const auto & pair : counter) {
        if (pair.second == records.size())
            return 0.0; // All records in one category, impurity is zero
        sum += (pair.second / total) * (pair.second / total);
    }
    return 1.0 - sum;
}

// Impurity from two subsets, split by attribute whichAttr at value d
double TreeData::combineImpurity(const int whichAttr, const Record::AttrType d) const {
    double impurity;
    try {
        auto giniPair = split(whichAttr, d); 
        impurity = (giniPair.first.doImpurity() * static_cast<double>(giniPair.first.size()) +
            giniPair.second.doImpurity() * static_cast<double>(giniPair.second.size())) / static_cast<double>(this->size());
    } catch (exception& e) {
        impurity = 1.0; // If one side is empty, set impurity to maximum
    }
    return impurity;
}

// Remove one attribute from the next level
void TreeData::cullAttribute(shared_ptr<TreeData>t, Record::AttrType d) {
    for (int attr : availableAttr) {
        if (attr != d) t->availableAttr.push_back(attr);
    }
}

// make a prediction based on the record
Record::Category TreeData::decide(const Record &rec) const {
    // if you are at the end of the tree, return the category with the most records
    if (left == nullptr && right == nullptr) {
        int redCount = 0, blueCount = 0;
        for (const auto& r : records) {
            if (r->getCategory() == Record::RED)
                ++redCount;
            else
                ++blueCount;
        }
        return (redCount >= blueCount) ? Record::RED : Record::BLUE;
    }

    // if it isnt a leaf node, go to the side based on the value of the split attribute
    if (rec.getAttribute(splitAttr) <= splitAt) {
        return left->decide(rec);
    } else {
        return right->decide(rec);
    }
}

}   // namespace
