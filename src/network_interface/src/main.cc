#include "../include/network_interface/network_interface.h"

int main(int argc, char* argv[]) {
    ros::init(argc, argv, "network_interface_node");
    NetworkInterface node;
    ros::MultiThreadedSpinner spinner(2);
    spinner.spin();
    return 0;
}