
#ifndef PUBLIC_H
#define PUBLIC_H

/*
server 和 client 公共文件
*/
enum EnMsgType
{
    LOGIN_MSG = 1,  // Andy: 登录消息与login绑定，
    LOGIN_MSG_ACK,  // ANDY: 登录成功
    LOGINOUT_MSG,   // 注销消息
    REG_MSG,        // 注册信息
    REG_MSG_ACK,    // 注册成功
    ONE_CHAT_MSG,   // 一对一聊天
    ADD_FRIEND_MSG, // 添加好友消息

    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG,    // 加入群组
    GROUP_CHAT_MSG,   // 群聊天
};

#endif