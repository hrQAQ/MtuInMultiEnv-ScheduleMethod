#encoding=utf-8
import os
# 读取csv文件
def read_csv(filename):
    data = []
    with open(filename, 'r') as f:
        for line in f:
            data.append(line.strip().split(','))
    return data

# 从data中选择第一列数据相同的行
def select(data):
    max = 0
    for row in data:   
        if int(row[0]) > max:
            max = int(row[0])
    max = int(max)
    print("max is %d" % (max))
    for flowid in range (1, max+1):
        FCT = 0
        is_probe = False
        for row in data:
            if int(row[0]) == flowid:
                is_probe = True
                FCT += int(row[2])
        if is_probe:
            print("flowid: %d, FCT: %d" % (flowid, FCT))
        else:
            print("flowid: %d, Not probed" % (flowid))
    
def main():
    data = read_csv('./fct/FCT(dc)_5us_40Gbps_0.005000_0.700000.csv')
    select(data)
    
if __name__ == '__main__':
    main()