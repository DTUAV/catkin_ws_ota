import socket
import math
import time

def int2char(d):
    if d == 0:
        return '0'
    elif d == 1:
        return '1'
    elif d == 2:
        return '2'
    elif d == 3:
        return '3'
    elif d == 4:
        return '4'
    elif d ==5:
        return '5'
    elif d == 6:
        return '6'
    elif d == 7:
        return '7'
    elif d == 8:
        return '8'
    elif d == 9:
        return '9'
    else:
        return 's'    

def GetDataSize(data_size_buf):
    data_size = 0
    for i in range(0, 8):
        data_size = data_size * 10 + (data_size_buf[i] - ord('0'))
    return data_size

def GetDataSizeBuf(data_size):
    ret_buf = []
    tem_size = data_size
    if (data_size > 99999999):
        ret_buf = ''
        return ret_buf
    for i in range(0,8):
        d = tem_size % 10.0
        tem_size = (int)(tem_size / 10)
        #print(tem_size)
        #print(d)
        d_c = int2char(d)
        #print(d_c)
        if (d_c == 's'):
            ret_buf = ''
            return ret_buf
        ret_buf.append(d_c)
    print("ret_buf")
    print(ret_buf)
    left = 0
    right = 7
    while(1):
        tem = ret_buf[left]
        ret_buf[left] = ret_buf[right]
        ret_buf[right] = tem
        if ((left == right) or (math.fabs(left - right) < 1.5)):
            break
        left = left + 1
        right = right - 1

        #print(left)
        #print(right)  
    return ret_buf

# 创建UDP socket
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
 
# 服务器IP和端口
server_ip = '192.168.114.138'#'192.168.4.1'
server_port = 6060

# 文件路径
#file_path = './sketch_cmd.ino.bin'
file_path = './master.ino.bin'
 
file_data = open(file_path, 'rb')
send_data = file_data.read()
send_data = b'http://192.168.114.170:5500/other_resources/Network/web/esp32/esp32_1_0_1.bin'
data_size = len(send_data)
print("data_size")
print(data_size)
data_size_buf = GetDataSizeBuf(data_size)
print(len(data_size_buf))
data_size_str = ""
for i in range(0, len(data_size_buf)):
    data_size_str = data_size_str + (data_size_buf[i])
print(data_size_str)

data_size_bytes = data_size_str.encode()
print(data_size_bytes)



start_flag = b"rr"


data_type = b"99"


end_flag = b"nn"


send_limit = 1
while(1):
    udp_socket.sendto(start_flag, (server_ip, server_port))
    udp_socket.sendto(data_type, (server_ip, server_port))

    udp_socket.sendto(data_size_bytes, (server_ip, server_port))
    try:
        data_ref_buff = 1024
        send_times = int(data_size / data_ref_buff) + 1
        for i in range(1, send_times + 1):
            if(i == send_times):
                udp_socket.sendto(send_data[data_ref_buff*(i-1): data_size], (server_ip, server_port))
            else:
                udp_socket.sendto(send_data[data_ref_buff*(i-1): data_ref_buff*i], (server_ip, server_port))
            #time.sleep(0.1)
    except socket.error as e:
        print(f"Error: {e}")
    udp_socket.sendto(end_flag, (server_ip, server_port))
    print("send")
    send_limit = send_limit + 1
    if send_limit >= 1:
        break

'''
# 发送文件
# 设置缓冲区大小
buffer_size = 1024
with open(file_path, 'rb') as file:
    data = file.read(buffer_size)
    while data:
        udp_socket.sendto(data, (server_ip, server_port))
        data = file.read(buffer_size)
# 发送结束标记
udp_socket.sendto(b'EOF', (server_ip, server_port))

'''
# 关闭socket
while(1):
    data, address = udp_socket.recvfrom(2)
    print("start_flag: " + str(data))
    print(data[0])
    print(data[1])
    if (len(data) == 2 and chr(data[0]) == 'r' and chr(data[1]) == 'r'):
        print("recv start flag")
        data, address = udp_socket.recvfrom(2)
        if (len(data) == 2):
            print("recv data type")
            print(data)
            print(data[0])
            print(ord('0'))
            data_type = (data[0] - ord('0')) * 10 + (data[1] - ord('0'))
            print(data_type)
            data, address = udp_socket.recvfrom(8)
            if (len(data) == 8):
                print("recv data size")
                print(data)
                data_size = GetDataSize(data)
                print("data size: ")
                print(data_size)
                data_ref_buff = 1024
                recv_times = (int)(data_size/data_ref_buff) + 1
                recv_data = b""
                for i in range(1, recv_times + 1):
                    tem_recv_size = data_ref_buff
                    if i == recv_times:
                        tem_recv_size = data_size - (recv_times - 1) * data_ref_buff
                    tem_data, address = udp_socket.recvfrom(tem_recv_size)
                    recv_data = recv_data + tem_data
                print("recv data: ")
                str_data = str(recv_data)
                print(recv_data)
                print(str_data)
                data, address = udp_socket.recvfrom(2)
                if (len(data) == 2 and chr(data[0]) == 'n' and chr(data[1]) == 'n'):
                    print("recv finish")
                else:
                    print("error -- recv end flag")
                    print(data)
            else:
                print("data size buf != 8")
                print(len(data))
        else:
            print("recv data type buf != 2")
            print(len(data))
    else:
        print("no start flag: ")
        print(data)
    
udp_socket.close()