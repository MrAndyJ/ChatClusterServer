#ifndef DCA68445_2AD5_4D97_8551_B62D78FCD442
#define DCA68445_2AD5_4D97_8551_B62D78FCD442

#include "db.h"
#include <string>
#include <iostream>
#include <vector>

class OfflineMsgModel
{
public:
    void insert(int userid, string msg)
    {
        char sql[1024] = {0};
        sprintf(sql, "insert into OfflineMessage values(%d, '%s')", userid, msg.c_str());
        MySQL1 mysql;
        if (mysql.connect())
        {
            mysql.update(sql);
        }
    }

    void remove(int userid)
    {
        // 1.组装sql语句
        char sql[1024] = {0};
        sprintf(sql, "delete from OfflineMessage where userid=%d", userid);

        MySQL1 mysql;
        if (mysql.connect())
        {
            mysql.update(sql);
        }
    }

    vector<string> query(int userid)
    {
        // 1.组装sql语句
        char sql[1024] = {0};
        sprintf(sql, "select message from OfflineMessage where userid = %d", userid);

        vector<string> vec;
        MySQL1 mysql;
        if (mysql.connect())
        {
            MYSQL_RES *res = mysql.query(sql); // 查询数据库
            if (res != nullptr)
            {
                // 把userid用户的所有离线消息放入vec中返回
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res)) != nullptr) // 逐行读取，userid的消息，放入vec中；
                {
                    vec.push_back(row[0]);
                }
                mysql_free_result(res); // 释放链接
                return vec;
            }
        }
        return vec;
    }
};

#endif /* DCA68445_2AD5_4D97_8551_B62D78FCD442 */
