#include "../include/network_interface/network_interface.h"
#include <fcntl.h>

//#define debug

NetworkInterface::NetworkInterface()
{
  if (!InitNetwork()) {
    ROS_ERROR("[NetworkInterface]: init network ---fail");
    return;
  }
    ros::NodeHandle nh("~");
        std::string network_msg_pub_topic = "/cur_pos";
        std::string network_msg_sub_topic = "/tgt_pos";

        nh.param<std::string>("network_msg_sub_topic", network_msg_sub_topic, network_msg_sub_topic);
        nh.param<std::string>("network_msg_pub_topic", network_msg_pub_topic, network_msg_pub_topic);

        nh.param<double>("recv_hz", network_recv_hz_, network_recv_hz_);
        nh.param<std::string>("ip", server_ip_, server_ip_);

        std::cout << "ip: " << server_ip_ << std::endl;

        nh.param<int>("port", port_, port_);

        network_msg_sub_ = nh.subscribe(network_msg_sub_topic, 10, &NetworkInterface::NetworkMsgCallback, this);
       
        network_msg_pub_ = nh.advertise<network_interface::network>(network_msg_pub_topic, 2);

        main_loop_timer_ = nh.createTimer(ros::Duration(1.0 / network_recv_hz_), &NetworkInterface::MainLoop, this);
        ROS_INFO("[NetworkInterface]: Init ROS successfully");
}

NetworkInterface::~NetworkInterface()
{
    if (sock_fd_ > 0) {
		close(sock_fd_);
		sock_fd_ = -1;
	}
}

void NetworkInterface::MainLoop(const ros::TimerEvent& event) {
    std::string buf;
    std::string temp_ip;
    uint16_t temp_port;

#ifdef debug
    ROS_INFO("[NetworkInterface]: MainLoop, recv start flag");

#endif
    // start flag
    if (!Recv(buf, 2, temp_ip, temp_port)) {
#ifdef debug
      std::cout << "start flag buf: " << buf << std::endl;
      ROS_ERROR("[NetworkInterface]: recv start flag --- fail");
#endif
      return;
    }
    if ((buf.size() < 2) || (buf.at(0) != 'r') && (buf.at(1) != 'r')) {
#ifdef debug
        ROS_ERROR("[NetworkInterface]: no start flag --- fail");
#endif
        return;
    }

#ifdef debug
    ROS_INFO("[NetworkInterface]: recv start flag");
#endif
   // data type
    if (!Recv(buf, 2, temp_ip, temp_port)) {
#ifdef debug
      std::cout << "data type buf: " << buf << std::endl;
      ROS_ERROR("[NetworkInterface]: recv data type --- fail");
#endif
      return;
    }
    if ((buf.size() < 2)) {

#ifdef debug
        ROS_ERROR("[NetworkInterface]: no data type --- fail");
#endif
        return;
    }
#ifdef debug
    ROS_INFO("[NetworkInterface]: recv data type");
#endif
    int message_type = (buf.at(0) - int('0')) * 10 + (buf.at(1) - int('0'));

#ifdef debug
    ROS_INFO("[NetworkInterface]: message type: %d", message_type);
#endif
   // message size
    if (!Recv(buf, 8, temp_ip, temp_port)) {
#ifdef debug
        std::cout << "message size buf: " << buf << std::endl;
        ROS_ERROR("[NetworkInterface]: recv message size --- fail");
#endif
        return;
    }

    if (buf.size() < 8) {
#ifdef debug
        ROS_ERROR("[NetworkInterface]: no message size --- fail");
#endif
        return;
    }

    long data_size = GetDataSize(buf, 8);
    if (data_size == -1) {
#ifdef debug
       ROS_ERROR("[NetworkInterface]: invalid data size, data size = -1");
#endif
       return;
    }
     // enter recv data
    int data_ref_buff = 1024;
    int recv_times = (int)(data_size / data_ref_buff) + 1;
#ifdef debug
    ROS_INFO("recv_times: %d", recv_times);
#endif
    std::string data = "";
     int cur_index = 0;
    for (int i = 1; i < recv_times + 1; ++i) {
        int tem_recv_size = data_ref_buff;
        if (i == recv_times) {
            tem_recv_size = data_size - (recv_times - 1) * data_ref_buff;
        }
        if (!Recv(buf, tem_recv_size, temp_ip, temp_port)) {
#ifdef debug
           ROS_ERROR("[NetworkInterface]: recv data --- fail in loop");
#endif
           return;
        }
        data += buf;
    }
#ifdef debug
        ROS_INFO("recv data successfully");
#endif
      // recv end flag
        if (!Recv(buf, 2, temp_ip, temp_port)) {
#ifdef debug
            ROS_ERROR("[NetworkInterface]: recv end flag --- fail");
#endif
           return;
        }

        if (buf.size() < 2 && buf.at(0) != 'n' && buf.at(1) != 'n') {
#ifdef debug
          ROS_ERROR("[NetworkInterface]: no end flag --- fail");
#endif
           return;
        }

#ifdef debug
        ROS_INFO("recv completed data successfully");
#endif
        network_interface::network network_msg;
        network_msg.message_type = message_type;
        network_msg.data = data;
        network_msg_pub_.publish(network_msg);
}

void NetworkInterface::NetworkMsgCallback(const network_interface::networkConstPtr& msg)
{
  ROS_INFO("[NetworkInterface]: recv message sent to server");
  ROS_INFO("[NetworkInterface]: msg: %s*, type: %i", msg->data.c_str(), msg->message_type);
  
  uint8_t start_flag_buf[2];
  start_flag_buf[0] = int('r');
  start_flag_buf[1] = int('r');

  if (!Send(start_flag_buf, 2, server_ip_, port_)) {
    ROS_ERROR("error --- send start flag");
    return;
  }
  ROS_INFO("send start flag successfully");

  std::vector<uint8_t> vec_buf;
  if (!GetDataSizeBuf(msg->message_type, 2, vec_buf)) {
    ROS_ERROR("error --- get data size buf --data_type");
    return;
  }
  uint8_t data_type_buf[2];
  data_type_buf[0] = vec_buf[0];
  data_type_buf[1] = vec_buf[1];

 if (!Send(data_type_buf, 2, server_ip_, port_)) {
     ROS_ERROR("error --- send data type");
    return;
  }

  ROS_INFO("send data type successfully");

  long data_size = msg->data.size();
  if (!GetDataSizeBuf(data_size, 8, vec_buf)) {
    ROS_ERROR("error --- get data size buf --data size buf");
    return;
  }

  uint8_t data_size_buf[8];
  for (int i = 0; i < 8; ++i) {
    data_size_buf[i] = vec_buf.at(i);
  }

if (!Send(data_size_buf, 8, server_ip_, port_)) {
    ROS_ERROR("error --- send data size");
    return;
  }

  ROS_INFO("send data size successfully");

uint8_t data_buf[data_size];
for (int i = 0; i < data_size; ++i) {
  data_buf[i] = msg->data.at(i);
}

if (!Send(data_buf, data_size, server_ip_, port_)) {
    ROS_ERROR("error --- send data");
    return;
  }

ROS_INFO("send data successfully");

  uint8_t end_flag_buf[2];
  end_flag_buf[0] = int('n');
  end_flag_buf[1] = int('n');
if (!Send(end_flag_buf, 2, server_ip_, port_)) {
    ROS_ERROR("error --- send end flag");
    return;
  }

  ROS_INFO("send end flag successfully");

}
 bool NetworkInterface::Send(uint8_t* data, int send_size, const std::string &ip, const uint16_t port) {
#ifdef debug
   ROS_INFO("[NetworkInterface]: ip: %s", ip.c_str());
   ROS_INFO("[NetworkInterface]: port: %d", port);
#endif
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    socklen_t len = sizeof(struct sockaddr_in);
    char buf[send_size];
    for (int i = 0; i < send_size; ++i) {
      buf[i] = data[i];
    }
    int ret = sendto(sock_fd_, buf, send_size, 0, (struct sockaddr *)&addr, len);
    if (ret < 0)
    {
        ROS_ERROR("sendto error %i", ret);
        return false;
    }
    return true;
 }
bool NetworkInterface::Send(const std::string &data, const std::string &ip, const uint16_t port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    socklen_t len = sizeof(struct sockaddr_in);
    int ret = sendto(sock_fd_, data.c_str(), data.size(), 0, (struct sockaddr *)&addr, len);
    if (ret < 0)
    {
        ROS_ERROR("sendto error");
        return false;
    }
    return true;
}
bool NetworkInterface::Recv(std::string &buf, int recv_size, std::string &ip, uint16_t &port)
{
    struct sockaddr_in peer_addr; // 用于接收发送端的地址信息
    socklen_t len = sizeof(struct sockaddr_in);
    char tmp[recv_size + 1] = {0};
#ifdef debug
    ROS_INFO("[NetworkInterface]: recv ...");
#endif
    int ret = recvfrom(sock_fd_, tmp, recv_size, 0, (struct sockaddr *)&peer_addr, &len);
#ifdef debug
    ROS_INFO("[NetworkInterface]: recv end");
#endif
    if (ret < 0)
    {
#ifdef debug
        ROS_ERROR("recvfrom error");
#endif
        return false;
    }
    buf = std::string(tmp);
    port = ntohs(peer_addr.sin_port);
    ip = inet_ntoa(peer_addr.sin_addr);
    return true;
}

bool NetworkInterface::InitNetwork()
{
    sock_fd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd_ < 0)
    {
        ROS_ERROR("[NetworkInterface]: sockfd < 0");
        return false;
    }

    ROS_INFO("[NetworkInterface]: get socket fd successfully");

        // 设置为非阻塞模式
    int flags = fcntl(sock_fd_, F_GETFL, 0);
    fcntl(sock_fd_, F_SETFL, flags | O_NONBLOCK);

/*
      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port_);
      addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());
      socklen_t len = sizeof(struct sockaddr_in);
      int ret = bind(sock_fd_, (struct sockaddr *)&addr, len);
      if (ret < 0)
      {
          ROS_ERROR("[NetworkInterface]: bind fail");
          return false;
      }

      ROS_INFO("[NetworkInterface]: bind port successfully");
*/
      return true;
}