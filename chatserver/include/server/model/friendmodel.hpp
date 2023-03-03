#ifndef C0D96CE0_8082_47E9_B266_9744AB068806
#define C0D96CE0_8082_47E9_B266_9744AB068806

#include "user.hpp"
#include <vector>

class FriendModel
{
public:
    // andy: 添加好友关系
    void insert(int userid, int friendid);

    // 返回用户还有列表
    vector<User> query(int userid);
};

#endif /* C0D96CE0_8082_47E9_B266_9744AB068806 */
