#pragma once
#include <ros/ros.h>
#include "network_interface/network.h"
#include <string>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ros/timer.h>
#include <vector>

// message_type = 99 => update firmware

class NetworkInterface {
    public:
    NetworkInterface();
    ~NetworkInterface();

    private:

    void NetworkMsgCallback(const network_interface::networkConstPtr& msg);
    bool InitNetwork();
    bool Send(const std::string &data, const std::string &ip, const uint16_t port);
    bool Send(uint8_t* data, int send_size, const std::string &ip, const uint16_t port);
    bool Recv(uint8_t* data, int recv_size, std::string &ip, uint16_t &port);
    bool Recv(std::string &buf, int recv_size, std::string &ip, uint16_t &port);
    void MainLoop(const ros::TimerEvent& event);

    int char2int(char d) {
    if (d == '0') {
        return 0;
    } else if (d == '1') {
        return 1;
    } else if (d == '2') {
        return 2;
    } else if (d == '3') {
        return 3;
    } else if (d == '4') {
        return 4;
    } else if (d == '5') {
        return 5;
    } else if (d == '6') {
        return 6;
    } else if (d == '7') {
        return 7;
    } else if (d == '8') {
        return 8;
    } else if (d == '9') {
        return 9;
    } else {
        return -1;
    }
}

char int2char(int d) {
    if (d == 0) {
        return '0';
    } else if (d == 1) {
        return '1';
    } else if (d == 2) {
        return '2';
    } else if (d == 3) {
        return '3';
    } else if (d == 4) {
        return '4';
    } else if (d == 5) {
        return '5';
    } else if (d == 6) {
        return '6';
    } else if (d == 7) {
        return '7';
    } else if (d == 8) {
        return '8';
    } else if (d == 9) {
        return '9';
    } else {
        return 's';
    }
}
long GetDataSize( uint8_t data_size[8]) {
    long ret_size = 0;
    for (int i = 0; i < 8; ++i) {
        int d = int(data_size[i]- int('0'));
        if (d == -1) {
            return -1; // error flag
        }
        ret_size = ret_size * 10 + d;
    }
    return ret_size;
}

long GetDataSize(const std::string& data_size_buf, int valid_size) {
    long ret_size = 0;
    if (data_size_buf.size() < valid_size) {
        return -1;
    }
    for (int i = 0; i < valid_size; ++i) {
        int d = int(data_size_buf[i]- int('0'));
        if (d == -1) {
            return -1; // error flag
        }
        ret_size = ret_size * 10 + d;
    }
    return ret_size;
}

bool GetDataSizeBuf(long data_size, int bit, std::vector<uint8_t>& buf) {
  buf.clear();
  buf.resize(bit, '0');
    long tem_size = data_size;
    long max_size = 0;
    for(int i = 0; i < bit; ++i) {
      max_size = max_size * 10 + 9;
    }
    if (data_size > max_size) {
        return false;
    }
    for (int i = bit - 1; i >= 0; --i) {
        int d = tem_size % 10;
        tem_size = tem_size / 10;
        char d_c = int2char(d);
        if (d_c == 's') {
            return false;
        }
        buf.at(i) = d_c;
        if (tem_size == 0) {
            break;
        }
    }
    return true;
}
    double network_recv_hz_{30.0};
    std::string server_ip_ {"127.0.0.1"};
    int32_t port_ {6060};

    ros::Publisher network_msg_pub_;
    ros::Subscriber network_msg_sub_;
    ros::Timer main_loop_timer_;

    int sock_fd_;
};