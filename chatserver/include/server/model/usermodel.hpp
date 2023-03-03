#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

class UserModel
{
public:
    // Andy: User表的增加方法
    bool insert(User &user);

    // Andy: 根据用户号码查询用户信息
    User query(int id);

    // Andy 跟新用户的状态
    bool updateState(User user);

    // Andy5 : 重置用户的状态信息
    void resetState();
};

#endif