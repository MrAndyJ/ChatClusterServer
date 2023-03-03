#ifndef CHARSERVER_H
#define CHARSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;
// using namespace placeholders;

 class ChatServer
 {
public:
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    // 启动服务器
    void start();
private:
    // Andy1: 当有链接时候，调用链接回调函数；
    // typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    
    void onConnection(const TcpConnectionPtr &);
    // Andy1:   上报读写事件相关信息的回调 
    void onMessage(const TcpConnectionPtr&,Buffer*, Timestamp);
    TcpServer _server;  // Andy1：使用TcpServer 库，实现服务器功能的类对象
    EventLoop *_loop;   // Andy1: 指向事件循环对象的指针
 };


#endif