//
// Created by Vetle Wegner Ingeberg on 20/10/2022.
//

#ifndef XIA2TREE_USERSORT_H
#define XIA2TREE_USERSORT_H

class Triggered_event;

class UserSort {
public:
    virtual ~UserSort() = default;
    virtual void FillEvent(const Triggered_event &event) = 0;
    virtual void Flush() = 0;
};



#endif //XIA2TREE_USERSORT_H
