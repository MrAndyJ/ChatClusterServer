#ifndef DB_H
#define DB_H
#include <muduo/base/Logging.h>
#include <string>
#include <mysql/mysql.h>

using namespace std;

static string server = "127.0.0.1";
static string user = "root";
static string password = "123123";
static string dbname = "chat";
// Andy: 数据库操作类
/* class MySQL
{
public:
    // Andy: 初始化数据库连接
    MySQL();
    // Andy: 释放数据库连接资源
    ~MySQL();
    // Andy:  连接数据库
    bool connect();
    // Andy:  更新操作
    bool update(string sql);
    // Andy:  查询操作
    MYSQL_RES *query(string sql);
    // Andy: 获取连接
    MYSQL* getConnection();
private:
    MYSQL *_conn;   // Andy: 理解为一个链接
};
 */

class MySQL1
{
public:
    // Andy:  初始化数据库连接
    MySQL1()
    {
        _conn = mysql_init(nullptr);
    }

    // Andy: 释放数据库连接资源
    ~MySQL1()
    {
        if (_conn != nullptr)
            mysql_close(_conn);
    }

    // 连接数据库
    bool connect()
    { // 用户名，密码等
        MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                      password.c_str(), dbname.c_str(), 3306, nullptr, 0);
        if (p != nullptr)
        {
            // C和C++代码默认的编码字符是ASCII，如果不设置，从MySQL上拉下来的中文显示？
            mysql_query(_conn, "set names gbk");
            LOG_INFO << "connect mysql success!";
        }
        else
        {
            LOG_INFO << "connect mysql fail!";
        }

        return p;
    }

    // Andy: 更新操作 mysql_query
    bool update(string sql)
    {
        if (mysql_query(_conn, sql.c_str())) // 传入链接和SQL语句，执行
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                     << sql << "更新失败!";
            return false;
        }

        return true;
    }

    // Andy: 查询操作: 返回这条链接的结果 MYSQL_RES 是一个结构体指针，访问内部元素。
    MYSQL_RES *query(string sql)
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                     << sql << "查询失败!";
            return nullptr;
        }

        return mysql_use_result(_conn);
    }

    // Andy: 获取连接
    MYSQL *getConnection()
    {
        return _conn;
    }

private:
    MYSQL *_conn; // Andy: 理解为一个链接
};

#endif