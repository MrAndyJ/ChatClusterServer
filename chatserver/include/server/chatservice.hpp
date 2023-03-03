#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include "usermodel.hpp"
#include "offlinemsg.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"

using namespace std;
using namespace muduo::net;
using namespace muduo;

#include "json.hpp"
using json = nlohmann::json;

// Andy: 处理消息的事件,回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

// Andy: 聊天服务器业务类
class ChatService
{
public:
    static ChatService *getinstance();
    void login(const TcpConnectionPtr &conn, json &js, Timestamp);

    void reg(const TcpConnectionPtr &conn, json &js, Timestamp);

    // Andy2: 获取消息对应处理器
    MsgHandler getHandler(int msgid);

    // Andy3: 如果客户端异常退出，删除id对应的链接
    void clientCloseException(const TcpConnectionPtr &conn);

    // Andy4: 一对一聊天
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp);

    // Andy5: 服务器异常，业务重置方法
    void reset();
    // 添加好友
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp);

    // Andy6 : 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // Andy6: 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // Andy6: 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // Andy : 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void handleRedisSubscribeMessage(int userid, string msg);

private:
    ChatService();

    unordered_map<int, MsgHandler> _msgHandlerMap;
    unordered_map<int, TcpConnectionPtr> _userConnMap; // Andy3: 保存用户的链接

    //   OfflineMsgModel _offlineMsgModel;

    mutex _connMutex; // Andy2:保存用户的链接时候，加锁

    // Andy:数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    // 2 Andy: 定义redis对象
    Redis _redis;
};

#endif