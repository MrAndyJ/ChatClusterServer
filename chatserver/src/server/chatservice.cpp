#include "public.hpp"
#include <muduo/base/Logging.h>
#include "chatservice.hpp"
#include <iostream>
#include <string>
using namespace muduo;
using namespace std;

// 注册消息以及对应的handler回调
// Andy: 在构造函数中，添加了编号对应的回调，服务端根据编号来回调相应的事件。
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});     //  Andy4:
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)}); //  Andy4:

    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});       //  Andy4:
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});     //  Andy4:
                                                                                                       // createGroup
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)}); //  Andy4:

    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)}); //  Andy4:

    // 3  Andy: 链接redis 服务器
    if (_redis.connect())
    {
        // Andy: 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatServer::handleRedisSubScribeMessage, this, _1, _2));
    }
}

// Andy: 给服务端提供getHandler接口，传递id 调用相应方法。
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认处理器，
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp)
        {
            LOG_ERROR << "MSGID:" << msgid << "can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// andy: 获取单例对象的接口
ChatService *ChatService::getinstance()
{
    static ChatService service;
    return &service;
}

// 等待网络模块回调
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int id = js["id"];
    string pwd = js["password"];

    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            std::cout << id << " 客户在线" << endl;
            // 该用户已经登录，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";
            conn->send(response.dump());
        } // Andy: 如果客户不在线，就直接登录
        else
        {
            // Andy: 保存用户的链接 , 加锁
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // 4 Andy: id用户登录后，向redis订阅channel(id)
            _redis.subscribe(id);

            // 登录成功，更新用户状态信息 state = > online
            user.setState("online");
            _userModel.updateState(user); // Andy5 : 访问一次数据库

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // Andy5: 查询是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(id);
            }

            // Andy5: 返回好友列表 返回
            vector<User> userVec = _friendModel.query(id);
            cout << "获取好友信息" << endl;
            if (!userVec.empty())
            {
                vector<string> vec2;
                for (User &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }

            conn->send(response.dump());
        }
    }
    else
    {
        // 登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

// Andy: 服务端如果传递2，则调用该注册方法； 处理注册业务： name password
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    //  LOG_INFO << "do reg server!!!";
    string name = js["name"];
    string pwd = js["password"];

    User user; // Andy 业务操作的User对象；
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user); // 向数据库中插入该方法，并返回插入结果；

    if (state)
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump()); // Andy: 发送给客户端
    }
    else
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{

    User user;
    {
        std::cout << "链接断开" << std::endl;
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 5 Andy: 取消通道 下线时候，取消
    _redis.unsubscribe(userid);

    // Andy3: 更新用户的状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 5 Andy: 取消通道 下线时候，取消
    _redis.unsubscribe(userid);

    // 更新用户状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}
// Andy4: 一对一聊天
/*
    msgid:
    id:
    from:
    to:
    msg:
    如果在线，直接发送给客户；
    如果不在线，保存到离线消息中，等上线使用通知。
*/
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int toid = js["to"].get<int>();
    std::cout << "void ChatService::oneChat" << endl;

    // Andy: 先查询是否在同一台服务器中链接，如果有，直接发送消息。
    {
        lock_guard<mutex> lock(_connMutex); // 操作临界数据，加锁
        auto it = _userConnMap.find(toid);  // 从在线用户中id是否在线。
        if (it != _userConnMap.end())
        {
            it->second->send(js.dump());
            return;
        }
    }
    // 查询toid 是否在线
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump()); // 如果在线，直接将消息发送到toid通道上；
        return;
    }

    // Andy： 如果 toid 不在线，直接写入离线数据库中
    _offlineMsgModel.insert(toid, js.dump());
}

void ChatService::reset()
{
    _userModel.resetState();
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    std::cout << "向数据库中添加还有" << std::endl;
    // 存储好友信息
    _friendModel.insert(userid, friendid);
}

// Andy6 : 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    Group group(-1, name, desc);        // 构造一个group对象
    if (_groupModel.createGroup(group)) // 将 groupname和groupdesc插入AllGroup表中；同时将该表的ID插入到group中。
    {
        _groupModel.addGroup(userid, group.getId(), "creator"); // 将 userid 组id和 “creator”
    }
}

// Andy6: 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// Andy6: 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid); // Andy: 1 查出groupid内，所有出了userid的人员userid;

    // 向本组内，所有人员发送信息。
    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _userConnMap.find(id); // 查看该用户是否在线，
        if (it != _userConnMap.end())    // 如果在线，则发送。
        {
            it->second->send(js.dump());
        }
        else // Andy: 如果不在线
        {
            // 发送离线消息
            User user = _userModel.query(id); // 从User表中查询id这个人的信息。
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    // Andy: 如果数据在上报过程中下线了，那就转发到离线消息中。
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}