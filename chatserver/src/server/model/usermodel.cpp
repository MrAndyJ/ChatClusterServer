#include "usermodel.hpp"
#include "db.h"
#include <iostream>

using namespace std;

/*
    // Andy: User表的增加方法
    bool insert(User &user);

    // Andy: 根据用户号码查询用户信息
    User query(int id);

    // Andy 跟新用户的状态
    bool updateState(User user);

    // Andy: 重置用户的状态信息
    void resetState();
*/

bool UserModel::insert(User &user)
{
    // Andy: 根据用户传入的组装sql语句, sprintf
    char sql[1024] = {0};
    sprintf(sql, "insert into User(name,password,state) values('%s','%s','%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL1 mysql;
    if (mysql.connect()) // Andy: 链接数据库，如果链接成功返回bool
    {
        std::cout << "新用户注册，向数据库中插入数据" << endl;
        if (mysql.update(sql)) // Andy: 发送一条更新语句，然后返回一条主键的id，并返回true;
        {
            // 获取插入成功的用户数据, 生成的主键id
            user.setId(mysql_insert_id(mysql.getConnection())); //
            return true;
        }
    }

    return false;
}

// Andy: 根据id查询信息,
User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id = %d", id);

    MySQL1 mysql;
    if (mysql.connect()) // Andy:  connect() 返回指针，如果不是空，则成功。
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res); // Andy 成功则返回一行；
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0])); // row[0] 相当于一个字段，通过atoi转为整型
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

// Andy: 更新用户状态
bool UserModel::updateState(User user)
{
    char sql[1024] = {0};
    sprintf(sql, "update User set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    std::cout << "用户登录成功，链接数据库，将offline 改为 online " << std::endl;
    MySQL1 mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// Andy5 : 重置用户的状态信息
void UserModel::resetState()
{
    // 1.组装sql语句
    char sql[1024] = "update User set state = 'offline' where state = 'online'";

    MySQL1 mysql;
    if (mysql.connect())
    {
        std::cout << "服务器异常更新数据库" << std::endl;
        mysql.update(sql);
    }
}
