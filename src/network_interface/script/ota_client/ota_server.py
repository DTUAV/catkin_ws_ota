import socket
def char2int(d):
    if d == '0':
        return 0
    elif d == '1':
        return 1
    elif d == '2':
        return 2
    elif d == '3':
        return 3
    elif d == '4':
        return 4
    elif d =='5':
        return 5
    elif d == '6':
        return 6
    elif d == '7':
        return 7
    elif d == '8':
        return 8
    elif d == '9':
        return 9
    else:
        return -1
# 创建UDP socket
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
 
# 绑定端口
udp_socket.bind(('127.0.0.1', 6060))
 
# 文件保存路径
file_path = 'ota.bin'
with open(file_path, 'wb') as file:
    while (1):
        start_flag_size = 2
        start_flag_buffer, addr = udp_socket.recvfrom(start_flag_size)
        print(len(start_flag_buffer))
        print(start_flag_buffer[0])
        print(start_flag_buffer[1])
        if (len(start_flag_buffer) <= 0):
            print("start_flag_buffer <= 0")
            continue
        if ((start_flag_buffer[0] == ord('r')) and (start_flag_buffer[1] == ord('r'))):
            data_type_size = 2
            data_type_buffer, addr = udp_socket.recvfrom(data_type_size)
            if ((data_type_buffer[0] == ord('9')) and (data_type_buffer[1] == ord('9'))):
                data_size_size = 8
                data_size_buffer, addr = udp_socket.recvfrom(data_size_size)
                data_size = 0
                print("data_size_buffer")
                data_size_buffer = data_size_buffer.decode()
                print(data_size_buffer)
                
                flag = 0
                for i in range(0,8):
                    d = char2int(data_size_buffer[i])
                    if (d == -1):
                        flag = 1
                        break
                    data_size = data_size * 10 + d
                if flag == 1:
                    print("recv fail, invalid data size")
                    break
                print(data_size)
                data_ref_buff = 1024
                recv_times = int(data_size / data_ref_buff) + 1
                all_data =b""
                for i in range(1, recv_times + 1):
                    if i == recv_times:
                        data, addr = udp_socket.recvfrom(data_size - (recv_times - 1) * data_ref_buff)
                        all_data = all_data + data
                    else:
                        data, addr = udp_socket.recvfrom(data_ref_buff)
                        all_data = all_data + data
                file.write(all_data)
                end_flag_size = 2
                end_flag_buffer, addr = udp_socket.recvfrom(end_flag_size)
                break
            else:
                print(data_type_buffer)
                print("(data_type_buffer[0] != '9') and (data_type_buffer[1] != '9')")
        else:
            print(start_flag_buffer)
            print("(start_flag_buffer[0] != '/r') and (start_flag_buffer[1] != '/r')")
# 关闭socket
udp_socket.close()