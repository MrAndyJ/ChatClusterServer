#ifndef EE7169F7_3F2A_44BD_BEA2_59E2D4104C94
#define EE7169F7_3F2A_44BD_BEA2_59E2D4104C94

#include "group.hpp"

#include <string>
#include <vector>

// Andy:  3 然后是groupmodel
class GroupModel
{
public:
    // Andy  创建群组
    bool createGroup(Group &group); // 给出组group信息，创建组

    // Andy加入群组 ： userid 要加入到groupid中；
    void addGroup(int userid, int groupid, string role); //

    // Andy 查询用户所有群组信息：返回群里所有的信息，也符合实际
    vector<Group> queryGroups(int userid);

    // Andy  根据groupid 查询群组用户id列表，出userid自己，给其他所有群成员发送消息
    vector<int> queryGroupUsers(int userid, int groupid); // 找到id 然后从service中保存的用户连接，发出去
};

#endif /* EE7169F7_3F2A_44BD_BEA2_59E2D4104C94 */
