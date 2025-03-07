
#include "RandomUtils.h"
#include <string>
#include <random>
using std::default_random_engine;
using std::random_device;
using std::string;

QString makeRandomString(int length)
{
    char tmp;      // tmp: 暂存一个随机数
    string buffer; // buffer: 保存返回值

    // 下面这两行比较重要:
    random_device rd;                   // 产生一个 std::random_device 对象 rd
    default_random_engine random(rd()); // 用 rd 初始化一个随机数发生器 random

    for (int i = 0; i < length; i++)
    {
        tmp = random() % 36; // 随机一个小于 36 的整数，0-9、A-Z 共 36 种字符
        if (tmp < 10)
        { // 如果随机数小于 10，变换成一个阿拉伯数字的 ASCII
            tmp += '0';
        }
        else
        { // 否则，变换成一个大写字母的 ASCII
            tmp -= 10;
            tmp += 'A';
        }
        buffer += tmp;
    }
    return QString::fromStdString(buffer);
}