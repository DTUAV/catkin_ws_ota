import math

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

def GetDataSizeBuf(data_size):
    ret_buf = []
    tem_size = data_size
    if (data_size > 99999999):
        ret_buf = ''
        return ret_buf
    for i in range(0,8):
        d = tem_size % 10.0
        tem_size = (int)(tem_size / 10)
        print(tem_size)
        print(d)
        d_c = int2char(d)
        print(d_c)
        if (d_c == 's'):
            ret_buf = ''
            return ret_buf
        ret_buf.append(d_c)
    left = 0
    right = 7
    while(1):
        tem = ret_buf[left]
        ret_buf[left] = ret_buf[right]
        ret_buf[right] = tem
        left = left + 1
        right = right - 1
        print(left)
        print(right)
        if ((left == right) or (math.fabs(left - right) < 1.5)):
            break
    return ret_buf

file_path = './sketch_cmd.ino.bin'
 
file_data = open(file_path, 'rb')
send_data = file_data.read()

print(len(send_data))
data_size = len(send_data)
data_buf = GetDataSizeBuf(data_size)
print(data_buf)