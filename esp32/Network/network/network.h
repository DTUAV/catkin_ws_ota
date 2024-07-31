#include <sys/_stdint.h>

#ifndef NETWORK_H
#define NETWORK_H

#include <Update.h>

#include <WiFi.h>

#include <string.h>
#include <vector>

//#define debug

const char *ssd = "R1";
const char *pasd = "zxcvbnma";

class Network
{

private:
    WiFiUDP Udp;
    bool is_recv_data = false;
    uint16_t udp_port = 6060; // 存储需要监听的端口号

    int char2int(char d)
    {
        if (d == '0')
        {
            return 0;
        }
        else if (d == '1')
        {
            return 1;
        }
        else if (d == '2')
        {
            return 2;
        }
        else if (d == '3')
        {
            return 3;
        }
        else if (d == '4')
        {
            return 4;
        }
        else if (d == '5')
        {
            return 5;
        }
        else if (d == '6')
        {
            return 6;
        }
        else if (d == '7')
        {
            return 7;
        }
        else if (d == '8')
        {
            return 8;
        }
        else if (d == '9')
        {
            return 9;
        }
        else
        {
            return -1;
        }
    }

    char int2char(int d)
    {
        if (d == 0)
        {
            return '0';
        }
        else if (d == 1)
        {
            return '1';
        }
        else if (d == 2)
        {
            return '2';
        }
        else if (d == 3)
        {
            return '3';
        }
        else if (d == 4)
        {
            return '4';
        }
        else if (d == 5)
        {
            return '5';
        }
        else if (d == 6)
        {
            return '6';
        }
        else if (d == 7)
        {
            return '7';
        }
        else if (d == 8)
        {
            return '8';
        }
        else if (d == 9)
        {
            return '9';
        }
        else
        {
            return 's';
        }
    }

    /**
     *
     */
    long GetDataSize(uint8_t data_size[8])
    {
        long ret_size = 0;
        for (int i = 0; i < 8; ++i)
        {
            int d = int(data_size[i] - int('0'));
            if (d == -1)
            {
                return -1; // error flag
            }
            ret_size = ret_size * 10 + d;
        }
        return ret_size;
    }

    bool GetDataSizeBuf(long data_size, int bit, std::vector<uint8_t> &buf)
    {
        buf.clear();
        buf.resize(bit, '0');
        long tem_size = data_size;
        long max_size = 0;
        for (int i = 0; i < bit; ++i)
        {
            max_size = max_size * 10 + 9;
        }
        if (data_size > max_size)
        {
            return false;
        }
        for (int i = bit - 1; i >= 0; --i)
        {
            int d = tem_size % 10;
            tem_size = tem_size / 10;
            char d_c = int2char(d);
            if (d_c == 's')
            {
                return false;
            }
            buf.at(i) = d_c;
            if (tem_size == 0)
            {
                break;
            }
        }
        return true;
    }

public:
    std::string str_msg;

    Network() {}

    void init()
    {

#ifdef debug
        Serial.print("开始连接WIFI:\n");
        Serial.print("SSD:\n");
        Serial.print(ssd);
        Serial.println();
#endif

        WiFi.begin(ssd, pasd);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
#ifdef debug
            Serial.print(".");
#endif
        }

#ifdef debug
        Serial.print("connected success!\n");
        Serial.print("IP:\n");
        Serial.print(WiFi.localIP());
        Serial.print("\n");
#endif

        Udp.begin(udp_port); // 启动UDP监听这个端口
    }

    bool SendData(IPAddress ip, uint16_t port, uint8_t *data, int send_size)
    {
        Udp.beginPacket(ip, port);
        int true_send_size = Udp.write(data, send_size);
        Udp.endPacket();
        if (true_send_size == send_size)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool SetNetworkData(const std::string &data, int message_type, IPAddress ip, uint16_t port)
    {

#ifdef debug
        Serial.print("ip: ");
        Serial.print(ip);
        Serial.print("\n");
        Serial.print("port: ");
        Serial.print(port);
#endif

        uint8_t start_flag_buf[2];
        start_flag_buf[0] = int('r');
        start_flag_buf[1] = int('r');

        if (!SendData(ip, port, start_flag_buf, 2))
        {
#ifdef debug
            Serial.print("error --- send start flag \n");
#endif
            return false;
        }

#ifdef debug
        Serial.print("\n");
        Serial.print("send start flag successfully \n");
#endif

        std::vector<uint8_t> vec_buf;
        if (!GetDataSizeBuf(message_type, 2, vec_buf))
        {
#ifdef debug
            Serial.print("error --- get data size buf --data_type \n");
#endif
            return false;
        }
        uint8_t data_type_buf[2];
        data_type_buf[0] = vec_buf[0];
        data_type_buf[1] = vec_buf[1];

#ifdef debug
        Serial.print("print data type buf: ");
        Serial.print(data_type_buf[0]);
        Serial.print(data_type_buf[1]);
#endif

        if (!SendData(ip, port, data_type_buf, 2))
        {

#ifdef debug
            Serial.print("error --- send data type \n");
#endif

            return false;
        }

#ifdef debug
        Serial.print("send data type successfully \n");
#endif

        long data_size = data.size();
        if (!GetDataSizeBuf(data_size, 8, vec_buf))
        {
#ifdef debug
            Serial.print("error --- get data size buf --data size buf \n");
#endif
            return false;
        }

        uint8_t data_size_buf[8];
        for (int i = 0; i < 8; ++i)
        {
            data_size_buf[i] = vec_buf.at(i);
        }

        if (!SendData(ip, port, data_size_buf, 8))
        {

#ifdef debug
            Serial.print("error --- send data size \n");
#endif
            return false;
        }

#ifdef debug
        Serial.print("send data size successfully \n");
#endif

        uint8_t data_buf[data_size];
        for (int i = 0; i < data_size; ++i)
        {
            data_buf[i] = data.at(i);
        }
        if (!SendData(ip, port, data_buf, data_size))
        {

#ifdef debug
            Serial.print("error --- send data \n");
#endif
            return false;
        }

#ifdef debug
        Serial.print("send data successfully \n");
#endif

        uint8_t end_flag_buf[2];
        end_flag_buf[0] = int('n');
        end_flag_buf[1] = int('n');
        if (!SendData(ip, port, end_flag_buf, 2))
        {

#ifdef debug
            Serial.print("error --- send end flag \n");
#endif
            return false;
        }

#ifdef debug
        Serial.print("send end flag successfully \n");
#endif
        return true;
    }

    bool GetNetworkData(std::string &recv_data, int &message_type)
    {
        // if (is_recv_data) {
        //   return true;
        // }
        uint8_t start_flag_buf[3];
        int start_data_len = Udp.parsePacket();
        if (start_data_len == 2)
        {
            // rr
            int tem_len = Udp.read(start_flag_buf, 2);
            if (tem_len == 2)
            {
#ifdef debug
                Serial.print(start_flag_buf[0]);
                Serial.print("\n");
                Serial.print(start_flag_buf[1]);
                Serial.print("\n");
                Serial.print(int('r'));
#endif
                if ((start_flag_buf[0] == int('r')) && (start_flag_buf[1] == int('r')))
                {
#ifdef debug
                    str_msg = "recv the start flag";
                    Serial.print("\n");
                    Serial.print(str_msg.c_str());
#endif
                    uint8_t data_type_buf[3];
                    int data_type_len = Udp.parsePacket();
                    if (data_type_len == 2)
                    {
                        int tem_len = Udp.read(data_type_buf, 2);
                        if (tem_len == 2)
                        {
                            if (((data_type_buf[0] >= int('0')) && (data_type_buf[0] <= int('9'))) && ((data_type_buf[1] >= int('0')) && (data_type_buf[1] <= int('9'))))
                            {
#ifdef debug
                                str_msg = "recv data type";
                                Serial.print("\n");
                                Serial.print(str_msg.c_str());
#endif
                                message_type = (data_type_buf[0] - int('0')) * 10 + (data_type_buf[1] - int('0'));
#ifdef debug
                                Serial.print("\n");
                                Serial.print("message_type: ");
                                Serial.print(message_type);
#endif
                                uint8_t data_size_buf[9];
                                int data_size_len = Udp.parsePacket();
                                if (data_size_len == 8)
                                {
                                    int tem_len = Udp.read(data_size_buf, 8);
                                    if (tem_len == 8)
                                    {
                                        long data_size = GetDataSize(data_size_buf);
#ifdef debug
                                        str_msg = "recv the data size";
                                        Serial.print("\n");
                                        Serial.print(str_msg.c_str());
                                        Serial.print("\n");
                                        Serial.print(data_size);
#endif
                                        if (data_size != -1)
                                        {
                                            // enter recv data
                                            int data_ref_buff = 1024;
                                            int recv_times = (int)(data_size / data_ref_buff) + 1;
#ifdef debug
                                            Serial.print("\n");
                                            Serial.print("recv_times: ");
                                            Serial.print(recv_times);
                                            Serial.print("\n");
#endif
                                            uint8_t data[data_size + 1];
                                            int cur_index = 0;
                                            for (int i = 1; i < recv_times + 1; ++i)
                                            {
                                                int tem_recv_size = data_ref_buff;
                                                if (i == recv_times)
                                                {
                                                    tem_recv_size = data_size - (recv_times - 1) * data_ref_buff;
                                                }
                                                uint8_t tem_data[tem_recv_size];
                                                int tem_data_size_len = Udp.parsePacket();
                                                int tem_len = Udp.read(tem_data, tem_recv_size);
                                                if (tem_len == 0)
                                                {
#ifdef debug
                                                    Serial.print("\n");
                                                    Serial.print(i);
                                                    Serial.print("\n");
                                                    Serial.print("re recv");
#endif
                                                    i--;
                                                    continue;
                                                }

                                                if (tem_len == tem_recv_size)
                                                {
                                                    for (int j = 0; j < tem_len; ++j)
                                                    {
                                                        data[cur_index] = tem_data[j];
                                                        cur_index++;
                                                    }
                                                }
                                                else
                                                {
#ifdef debug
                                                    str_msg = "tem_len != tem_recv_size, --data\n";
                                                    Serial.print("\n");
                                                    Serial.print(i);
                                                    Serial.print("\n");
                                                    Serial.print(str_msg.c_str());
                                                    Serial.print("\n");
                                                    Serial.print("tem_len: ");
                                                    Serial.print(tem_len);
                                                    Serial.print("\n");
                                                    Serial.print("tem_recv_size: ");
                                                    Serial.print(tem_recv_size);
#endif
                                                    return false;
                                                }
                                            }
#ifdef debug
                                            Serial.print("recv data successfully");
                                            Serial.print("\n");
#endif
                                            uint8_t end_flag_buf[3];
                                            int end_len = Udp.parsePacket();
                                            if (end_len == 2)
                                            {
                                                int tem_len = Udp.read(end_flag_buf, 2);
                                                if ((end_flag_buf[0] == int('n')) && (end_flag_buf[1] == int('n')))
                                                {
#ifdef debug
                                                    str_msg = "end recv message\n";
                                                    Serial.print("\n");
                                                    Serial.print(str_msg.c_str());
#endif
                                                    is_recv_data = true;
                                                    recv_data = std::string(data, data + data_size);
#ifdef debug
                                                    Serial.print("\n");
                                                    Serial.print("recv data: ");
                                                    Serial.print(recv_data.c_str());
                                                    Serial.print("\n");
#endif
                                                    SetNetworkData("recv", 0, Udp.remoteIP(), Udp.remotePort());
                                                    return true;
                                                }
                                                else
                                                {
#ifdef debug
                                                    Serial.print("\n");
                                                    str_msg = "recv data length != 2, --end flag\n";
                                                    Serial.print(str_msg.c_str());
#endif
                                                    return false;
                                                }
                                            }
                                            else
                                            {
#ifdef debug
                                                Serial.print("\n");
                                                str_msg = "current packet length != 2, --end flag\n";
                                                Serial.print(str_msg.c_str());
                                                Serial.print("\n");
                                                Serial.print("end_len: ");
                                                Serial.print(end_len);
                                                Serial.print("\n");
#endif
                                                return false;
                                            }
                                        }
                                        else
                                        {
#ifdef debug
                                            str_msg = "data size error, cannot read the size of data, --data size\n";
                                            Serial.print(str_msg.c_str());
#endif
                                            return false;
                                        }
                                    }
                                    else
                                    {
#ifdef debug
                                        str_msg = "recv data length != 8, --data size\n";
                                        Serial.print(str_msg.c_str());
#endif
                                        return false;
                                    }
                                }
                                else
                                {
#ifdef debug
                                    str_msg = "current packet length != 8, --data size\n";
                                    Serial.print(str_msg.c_str());
#endif
                                    return false;
                                }
                            }
                            else
                            {
#ifdef debug
                                str_msg = "not recv the data type of update system\n";
                                Serial.print(str_msg.c_str());
#endif
                                return false;
                            }
                        }
                        else
                        {
#ifdef debug
                            str_msg = "recv data length != 2, --data type\n";
                            Serial.print(str_msg.c_str());
#endif
                            return false;
                        }
                    }
                    else
                    {
#ifdef debug
                        str_msg = "current packet length != 2, --data type\n";
                        Serial.print(str_msg.c_str());
#endif
                        return false;
                    }
                }
                else
                {
#ifdef debug
                    str_msg = "not recv the start flag\n";
                    Serial.print(str_msg.c_str());
#endif
                    return false;
                }
            }
            else
            {
#ifdef debug
                str_msg = "recv data length != 2. --start flag\n";
                Serial.print(str_msg.c_str());
#endif
                return false;
            }
        }
        else
        {
#ifdef debug
            str_msg = "current packet length != 2, --start flag\n";
            Serial.print(str_msg.c_str());
            Serial.print(WiFi.localIP());
            Serial.print("\n");
#endif
            return false;
        }
    }
};
#endif