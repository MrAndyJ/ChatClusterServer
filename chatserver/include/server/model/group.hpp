#ifndef B4420650_3D5C_4E9A_B03A_C4F635A12B1F
#define B4420650_3D5C_4E9A_B03A_C4F635A12B1F

#include "groupuser.hpp"
#include <vector>
#include <string>

// 1  AllGroup 表： zuid ， groupname  groupdesc
class Group
{
public:
    // 构造函数
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId(int id) { this->id = id; } // 获取组ID
    void setName(string name) { this->name = name; }
    void setDesc(string desc) { this->desc = desc; }

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getDesc() { return this->desc; }
    vector<GroupUser> &getUsers() { return this->users; }

public:
    int id;                  // Andy: 组id
    string name;             // Andy: z组name
    string desc;             // Andy: 描述
    vector<GroupUser> users; // Andy:  使用vector存放 组的groupUser组所有成员，包括 组id, userid,在组中的角色
    // 之所以这样组合，因为可以减少链接；
    // 这个GroupUser中继承了User，同时也包括了users类。
};

#endif /* B4420650_3D5C_4E9A_B03A_C4F635A12B1F */
