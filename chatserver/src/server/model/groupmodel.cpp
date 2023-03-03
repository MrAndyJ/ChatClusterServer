
#include "groupmodel.hpp"

#include "db.h"

/*

    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在群组信息 ： 返回群里所有的信息，也符合实际
    vector<Group> queryGroups(int userid);
    // 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
    vector<int> queryGroupUsers(int userid, int groupid);
    */
// Andy: 创建群组
bool GroupModel::createGroup(Group &group1)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into AllGroup(groupname,groupdesc) values('%s','%s')", group1.getName().c_str(), group1.getDesc().c_str());

    MySQL1 mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group1.setId(mysql_insert_id(mysql.getConnection())); // 生成主键id后，使用mysql_insert_id取出来。
            return true;
        }
    }
    return false; // 如果已经有组消息，就不要重复添加，直接返回。
}

//  Andy : 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into GroupUser values(%d,%d,'%s')", groupid, userid, role.c_str());

    MySQL1 mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// Andy :  查询用户都加入了 有哪些组
/*
    所以查询时候，根据用户id 在 Allgroup表中和GroupUser表中联合查询。
*/
vector<Group> GroupModel::queryGroups(int userid)
{
    /*
        1 先根据userid 在 groupuser表中查询出该用户所属的群组信息
        2 再根据群组信息，查询属于该群组的素有用户的userid,并且和user表进行多表联合查询，查出用户的详细信息
    */
    // 查询时候，不能直接根据组进行查，而是根据
    char sql[1024] = {0};
    // 联合查询，userid 所在的组 groupname 和 groupdesc
    // 一次查询两个表，
    // 假设该用户在两个组，这个两个组的id groupname groupdesc
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from AllGroup a inner join GroupUser b on a.id=b.groupid where b.userid=%d", userid);
    vector<Group> groupVec;

    MySQL1 mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            // 查处userid所有的群组信息
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
        }
    }

    // 再次查询这两个组的成员信息。
    for (Group &group : groupVec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from User a \
            inner join GroupUser b on b.userid = a.id where b.groupid=%d",
                group.getId());

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表(查询出groupid这个组除了userid之外的所有人员)，除userid自己，主要用户群聊业务给群组其它成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    // 查出组内
    sprintf(sql, "select userid from GroupUser where groupid = %d and userid != %d", groupid, userid);

    vector<int> idVec; // 保存groupid 群内的所有人员id;
    MySQL1 mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}
