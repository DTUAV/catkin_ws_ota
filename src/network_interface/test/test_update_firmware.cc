#include "ros/ros.h"
#include "network_interface/network.h"
ros::Publisher msg_pub;
ros::Subscriber msg_sub;
void MessageCallback(const network_interface::network& msg) {
    std::cout << "recv network data: " << msg.data << std::endl;
}

int main(int argc, char* argv[]) {
    ros::init(argc, argv, "test_update_firmware");
    ros::NodeHandle nh("~");
    msg_pub = nh.advertise<network_interface::network>("/network_interface/sub", 20);
    msg_sub = nh.subscribe("/network_interface/pub", 10, MessageCallback);
    network_interface::network msg;
    msg.message_type = 99;
    msg.data = "http://192.168.114.170:5500/Network/web/esp32/esp32_1_0_1.bin";

    for (int i = 0; i < 10; ++i)
    msg_pub.publish(msg);
    ROS_INFO("[test_update_firmware]: send message");
    ros::spin();

    return 0;
}