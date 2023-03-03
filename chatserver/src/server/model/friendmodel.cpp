#include "friendmodel.hpp"
#include "db.h"
#include <iostream>
/*

    // andy: 添加好友关系
    void insert(int userid, int friendid);

    // 返回用户还有列表
    vector<User> query(int userid);
    */
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into Friend values(%d,%d)", userid, friendid);

    MySQL1 mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// Andy5: 返回好友列表: 登录成功之后，如果有好友，则返回好友列表
vector<User> FriendModel::query(int userid)
{
    char sql[1024] = {0};
    // 联合查询出
    sprintf(sql, "select a.id,a.name,a.state from User a inner join Friend b on a.id = b.userid where b.userid=%d", userid);

    vector<User> vec;
    MySQL1 mysql;
    if (mysql.connect())
    {
        std::cout << "查询userid: " << userid << "好友信息" << std::endl;
        MYSQL_RES *res = mysql.query(sql); // res返回查询到的数据库的结果
        if (res != nullptr)
        {
            // row中存放每行记录，然后循环遍历
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}