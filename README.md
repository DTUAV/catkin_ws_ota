# catkin_ws_ota
=========================================================================================================================
S1: build ros packet: network_interface

catkin_make --pkg network_interface

=========================================================================================================================
=========================================================================================================================
S2: configure the launch file in network_interface

<node pkg = "network_interface" type = "network_interface_node" name = "network_interface_node" output = "screen">
     <param name = "network_msg_sub_topic" value = "/network_interface/sub" />    // messages sent to esp32
     <param name = "network_msg_pub_topic" value = "/network_interface/pub" />    // messages from esp32
     <param name = "ip" value = "192.168.114.138" type = "string" />              // ip of esp32
     <param name = "port" value = "6060" />                                       // udp port
     <param name = "recv_hz" value = "10.0" />                                    // receive message frequency
</node>

=========================================================================================================================
=========================================================================================================================
S3: download esp32 code: esp32/Network/network/network.ino

network.h: udp server

a typical message: message_type = 99 => update system firmware

some codes need to be modified: 

in network.h:

const char *ssd = "R1";   // wifi name

const char *pasd = "zxcvbnma";  // wifi password

The pc and esp32 connect to the same wifi or are in a local network
=========================================================================================================================
=========================================================================================================================
S4: open the local web server

--open vscode

--select web/index.html

--open with Live Server

http://127.0.0.1:5500/other_resources/Network/web/index.html

firmware path:

http://192.168.114.170:5500/esp32/Network/web/esp32/esp32_1_0_1.bin

=========================================================================================================================
=========================================================================================================================
S5: update firmware

--open rqt:

--publish a message to network: /network_interface/sub

-message_type: 99

-data: 'http://192.168.114.170:5500/esp32/Network/web/esp32/esp32_1_0_1.bin'


=========================================================================================================================
=========================================================================================================================
other files:

python codes + esp32

/script/ota_client/ota_client.py  ---udp client

/script/ota_client/ota_server.py  ---udp server

=========================================================================================================================
// results
��ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x40376e10
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x4bc
load:0x403c9700,len:0xbd8
load:0x403cc700,len:0x2a0c
entry 0x403c98d0















