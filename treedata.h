/*
 * Sin-Yaw Wang <swang24@scu.edu
 * Decision Tree Data Structure
 * Meant to support both Gini Impurity ad ID3
 */
#ifndef TREEDATA_H
#define TREEDATA_H
namespace csen79 {

class TreeData {
    // convenient types for internal uses
    using TreePair = std::pair<TreeData, TreeData>; // Used to split the data into two
    using HuntType = std::pair<Record::AttrType, double>;

    std::vector<std::shared_ptr<Record>> records;   // no need to delete
    std::shared_ptr<TreeData> left{nullptr}, right{nullptr};

    int level{0};
    int splitAttr{Record::NOATTR};
    Record::AttrType splitAt{Record::MIN};
    std::vector<int> availableAttr;
    double selfImpurity{1.0};
    double improvedImpurity{1.0};

    TreePair split(const int, const Record::AttrType) const;    // Split data into two based on attribute and value
    void cullAttribute(std::shared_ptr<TreeData> t, Record::AttrType d);
    double combineImpurity(const int, const Record::AttrType) const;
    const double doImpurity() const;
    const HuntType huntSplit(const int) const;
public:
    TreeData() { records.clear(); availableAttr.clear();}
    ~TreeData() {};

    // Use size_t to match container::size()
    const std::size_t size() const { return records.size(); }
    void makeTree();

    friend std::istream & operator>>(std::istream &, TreeData &);
    friend std::ostream & operator<<(std::ostream &, const TreeData &);
}; 

}

#endif // TREEDATA_H