# APS: Adaptive Packet Sizing for Efficient End-to-End Network Transmission

在ns3模拟器中实现了基于环境的MTU调整算法，该算法在不同的网络环境中自动调整MTU大小，用以提高流完成时间。

## 1. Features

    - 应用层批量发包
        - 发包时计算当前网络环境的bset MTU大小
        - 传输过程中动态调整MTU大小
        - 支持优先队列，优先队列配置了不同的发送概率
        - 支持多种调度算法 (FCFS，RR，SRPT，SJF）

## 2. Usage

``` shell
# 编译
./waf configure
./waf build

# 运行(按需修改参数)
./waf --run "datacenter --DELAY=$delay --LOSS_RATE=$loss_rate --BANDWIDTH_LINK=$bandwidth --LOAD=$load --SCHEDULE_METHOD=$method"
./waf --run "mtu_wan --PROPOGATION_DELAY=$delay --LOSS_RATE=$loss_rate --ES_BANDWIDTH=$bandwidth --SS_BANDWIDTH=$bandwidth --LOAD=$load"
./waf --run "mtu_mix --DC_DELAY=$dc_delay --WAN_DELAY=$wan_delay --LOSS_RATE=$loss_rate --BANDWIDTH_LINK=$dc_bandwidth --ES_BANDWIDTH=$wan_bandwidth --SS_BANDWIDTH=$wan_bandwidth --LOAD=$load"

# debug
source env.sh
gdb ./build/scratch/<application-name> <args>
```

## 3. Data Analysis

由FlowMonitor统计的数据存放在`data`目录下，目录结构如下：

```shell
❯ tree data
data
├── dc
│   ├── FCFS
│   ├── RR
│   ├── SJF
│   └── SRPT
│       └── FCT(dc)_5us_40Gbps_0.050000_0.700000.xml
├── mix
│   └── SRPT
├── wan
│   └── SRPT
│       ├── FCT(wan)_2.5ms_10Mbps_0.005000_0.500000.xml
│       ├── FCT(wan)_2.5ms_10Mbps_0.005000_0.700000.xml
│       └── FCT(wan)_5ms_10Mbps_0.010000_0.700000.xml
```


## 4. Scripts

分析数据和批量执行的脚本存放在`script`目录下，目录结构如下：

```shell
❯ tree script
script
├── csv_parser.py
├── Datacenter.sh
└── xml_parser.py
```

- `Datacenter.sh`是批量执行的脚本，用以在数据中心环境下，设置不同的网络环境下执行不同的调度算法，执行结果存放在`data`目录下。

- `xml_parser.py`用以解析`data`目录下的xml文件，并计算相应的流完成时间

- （duplicate）`csv_parser.py`用以解析`data`目录下的csv文件

# APS: Adaptive Packet Sizing for Efficient End-to-End Network Transmission

在ns3模拟器中实现了基于环境的MTU调整算法，该算法在不同的网络环境中自动调整MTU大小，用以提高流完成时间。

## 1. Features

    - 应用层批量发包
        - 发包时计算当前网络环境的bset MTU大小
        - 传输过程中动态调整MTU大小
        - 支持优先队列，优先队列配置了不同的发送概率
        - 支持多种调度算法 (FCFS，RR，SRPT，SJF）

## 2. Usage

``` shell
# 编译
./waf configure
./waf build

# 运行(按需修改参数)
./waf --run "datacenter --DELAY=$delay --LOSS_RATE=$loss_rate --BANDWIDTH_LINK=$bandwidth --LOAD=$load --SCHEDULE_METHOD=$method"
./waf --run "mtu_wan --PROPOGATION_DELAY=$delay --LOSS_RATE=$loss_rate --ES_BANDWIDTH=$bandwidth --SS_BANDWIDTH=$bandwidth --LOAD=$load"
./waf --run "mtu_mix --DC_DELAY=$dc_delay --WAN_DELAY=$wan_delay --LOSS_RATE=$loss_rate --BANDWIDTH_LINK=$dc_bandwidth --ES_BANDWIDTH=$wan_bandwidth --SS_BANDWIDTH=$wan_bandwidth --LOAD=$load"

# debug
source env.sh
gdb ./build/scratch/<application-name> <args>
```

## 3. Data Analysis

由FlowMonitor统计的数据存放在`data`目录下，目录结构如下：

```shell
❯ tree data
data
├── dc
│   ├── FCFS
│   ├── RR
│   ├── SJF
│   └── SRPT
│       └── FCT(dc)_5us_40Gbps_0.050000_0.700000.xml
├── mix
│   └── SRPT
├── wan
│   └── SRPT
│       ├── FCT(wan)_2.5ms_10Mbps_0.005000_0.500000.xml
│       ├── FCT(wan)_2.5ms_10Mbps_0.005000_0.700000.xml
│       └── FCT(wan)_5ms_10Mbps_0.010000_0.700000.xml
```


## 4. Scripts

分析数据和批量执行的脚本存放在`script`目录下，目录结构如下：

```shell
❯ tree script
script
├── csv_parser.py
├── Datacenter.sh
└── xml_parser.py
```

- `Datacenter.sh`是批量执行的脚本，用以在数据中心环境下，设置不同的网络环境下执行不同的调度算法，执行结果存放在`data`目录下。

- `xml_parser.py`用以解析`data`目录下的xml文件，并计算相应的流完成时间

- **[duplicate]** `csv_parser.py`用以解析`data`目录下的csv文件

