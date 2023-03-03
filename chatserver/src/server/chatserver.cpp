
#include "chatserver.hpp"
#include "json.hpp"
#include <functional>
#include <iostream>
#include "chatservice.hpp"
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}
// Andy1: 当有链接时候，调用链接回调函数；
// typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // Andy: 客户端断开链接
    cout << "链接信息" << endl;
    if (!conn->connected())
    {
        // Andy3: 如果异常退出：
        ChatService::getinstance()->clientCloseException(conn); // Andy3:如果客户端异常关闭
        cout << "链接信息" << endl;
        conn->shutdown();
    }
}

// Andy1:   服务器上报读写事件相关信息的回调
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString(); // 接收到客户的消息
    json js = json::parse(buf);                 // Andy1: 数据反序列化
    // Andy:  通过 js["msgid"] 获取业务handler
    // 解耦：网络模块和业务模块，
    auto msgHandler = ChatService::getinstance()->getHandler(js["msgid"].get<int>()); // 将msgid 值转为in类型；
    // Andy: 回调消息绑定好的事件，来执行相应的业务处理
    msgHandler(conn, js, time);

    std::cout << "aaa" << std::endl;
}
