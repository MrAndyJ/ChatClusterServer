#ifndef F9B4B9C2_CA8E_4BB4_AABE_05AD0AF2AD24
#define F9B4B9C2_CA8E_4BB4_AABE_05AD0AF2AD24

#include "user.hpp"
#include <string>

// 群组信息，多了一个role 角色信息，从User类直接继承，复用User其他信息。
// 2 要显示 User信息，还要显示Group信息，
class GroupUser : public User
{
public:
    void setRole(string role) { this->role = role; }
    string getRole() { return this->role; }

private:
    string role;
};

#endif /* F9B4B9C2_CA8E_4BB4_AABE_05AD0AF2AD24 */
