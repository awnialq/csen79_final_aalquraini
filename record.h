/*
 * Sin-Yaw Wang <swang24@scu.edu
 */
#ifndef RECORD_H
#define RECORD_H

namespace csen79 {

/*
 * A record with number of integral attributes and a category (RED or BLUE).
 * The number of attributes is defined by NATTRIBUTES set to be 3.
 * Each attribute is an integer in the range [MIN, MAX]
 */
class Record {
public:
    // Type definitions
    using AttrType = int;
    enum Category {RED, BLUE};

    static constexpr int NATTRIBUTES{3};
    static constexpr int NOATTR{NATTRIBUTES+1};
    static constexpr AttrType MAX{100};
    static constexpr AttrType MIN{0};

    Record(): attributes{0, 0, 0}, category(RED) {}
    Record(int attr1, int attr2, int attr3, Category cat):
        attributes{attr1, attr2, attr3}, category(cat) {}
    int getAttribute(int index) const { 
        if (index < 0 || index >= NATTRIBUTES)
            throw std::out_of_range("Index out of range");
        return attributes[index];
    }
    Category getCategory() const { return category; }

private:
    AttrType attributes[NATTRIBUTES];
    Category category;

friend std::istream& operator>>(std::istream& is, Record& record);
friend std::ostream& operator<<(std::ostream& os, const Record& record);
};

}
#endif // RECORD_H