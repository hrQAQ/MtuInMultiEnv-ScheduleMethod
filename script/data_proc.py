import os
import xml_parser
import matplotlib.pyplot as plt

DATA_DIR = "./data/dc/"
DATA_FILE = "fct_data.txt"
IMG_DIR = "./image/"
METHODS = ["RR", "SRPT", "SJF", "FCFS"]
METRICS = ["bandwidth", "loss_rate", "load"]
BANDWIDTH = ["10Gbps", "25Gbps", "40Gbps"]
LOSS_RATE = ["0.000000", "0.001000", "0.005000", "0.010000", "0.050000"]
LOAD = ["0.300000", "0.400000", "0.500000", "0.600000", "0.700000", "0.800000", "0.900000"]
css = {}
font = {}
experiments = {
    "RR": [],
    "SRPT": [],
    "SJF": [],
    "FCFS": []
}


class Experiment():
    def __init__(self):
        self.Method = ""
        self.delay = ""
        self.bandwidth = ""
        self.loss_rate = ""
        self.load = ""
        self.FCT =  ""       

class Line():
    def __init__(self, x, y, method):
        self.x = x
        self.y = y
        self.method = method
        
        
# 获取数据
def get_data():
    exps = []
    for method in METHODS:
        for file in os.listdir(DATA_DIR + method):
            if file.endswith(".xml"):
                exp = Experiment()
                exp.Method = method
                exp.delay = file.split("_")[1]
                exp.bandwidth = file.split("_")[2]
                exp.loss_rate = file.split("_")[3]
                exp.load = "0." + file.split("_")[4].split(".")[1]
                exp.FCT = xml_parser.main(["xml_parser.py", DATA_DIR + method + "/" + file, "1999"])
                exps.append(exp)
    with open(DATA_DIR + DATA_FILE, "w") as f:
        exps.sort(key=lambda x: (x.Method, x.bandwidth, x.loss_rate, x.load))
        for exp in exps:
            f.write(exp.Method + " " + exp.delay + " " + exp.bandwidth + " " + exp.loss_rate + " " + exp.load + " " + str(exp.FCT) + "\n")

def normalize_data(lines):
    # 将FCT数据归一化
    # 第一步：获取最大值、最小值
    # 第二步：将所有数据减去最小值
    # 第三步：将所有数据除以最大值减去最小值
    max_FCT = 0
    min_FCT = 0x3f3f3f3f
    for line in lines:
        for el_y in line.y:
            if float(el_y) > max_FCT:
                max_FCT = float(el_y)
            if float(el_y) < min_FCT:
                min_FCT = float(el_y)
    for line in lines:
        for i in range(len(line.y)):
            line.y[i] = (float(line.y[i]) - min_FCT) / (max_FCT - min_FCT)
    return lines
    

def read_data():
    if os.path.getsize(DATA_DIR + DATA_FILE) == 0:
            get_data()
    with open(DATA_DIR + DATA_FILE, "r") as f:
        for line in f.readlines():
            exp = Experiment()
            exp.Method = line.split(" ")[0]
            exp.delay = line.split(" ")[1]
            exp.bandwidth = line.split(" ")[2]
            exp.loss_rate = line.split(" ")[3]
            exp.load = line.split(" ")[4]
            exp.FCT = line.split(" ")[5].split("\n")[0]
            for exp_name in METHODS:
                if exp.Method == exp_name:
                    experiments[exp_name].append(exp)

def get_css():
    marker = ''
    line_width = 3
    mark_size = 5
    font = {'family': 'Times new roman', 'weight': 'bold', 'size': '15'}
    colors = ['#5060d0', '#ffb901', '#00a000', '#f25900']
    css = {
        'RR': [colors[0], '-', marker, 'RR', line_width, mark_size],
        'SRPT': [colors[1], '-', marker, 'SRPT', line_width, mark_size],
        'FCFS': [colors[2], '-', marker, 'FCFS', line_width, mark_size],
        'SJF': [colors[3], '-', marker, 'SJF', line_width, mark_size],
    }
    return css, font

def gen_figname(metric, scenario):
    figname = metric
    if metric == "bandwidth":
        figname += "_" + "loss_rate_" + scenario[0] + "_load_" + scenario[1]
    elif metric == "loss_rate":
        figname += "_" + "bandwidth_" + scenario[0] + "_load_" + scenario[1]
    elif metric == "load":
        figname += "_" + "bandwidth_" + scenario[0] + "_loss_rate_" + scenario[1]
    return IMG_DIR + metric + "/" + figname + ".png"

def gen_xlabel(metric):
    if metric == "bandwidth":
        return "Bandwidth (Gbps)"
    elif metric == "loss_rate":
        return "Loss Rate (%)"
    elif metric == "load":
        return "Load (%)"

def gen_scenarios(metric):
    scenarios = []
    if metric == "bandwidth":
        for i in LOSS_RATE:
            for j in LOAD:
                scenarios.append((i, j))
    elif metric == "loss_rate":
        for i in BANDWIDTH:
            for j in LOAD:
                scenarios.append((i, j))
    elif metric == "load":
        for i in BANDWIDTH:
            for j in LOSS_RATE:
                scenarios.append((i, j))
    return scenarios

def judge_scenario(exp, scenario, metric):
    if metric == "bandwidth":
        if exp.loss_rate == scenario[0] and exp.load == scenario[1]:
            return True
    elif metric == "loss_rate":
        if exp.bandwidth == scenario[0] and exp.load == scenario[1]:
            return True
    elif metric == "load":
        if exp.bandwidth == scenario[0] and exp.loss_rate == scenario[1]:
            return True
    return False

def gen_x_ticks(metric, x):
    if metric == "loss_rate":
        for i in range(len(x)):
            # 保留3位小数
            x[i] = "%.3f" % float(x[i])
    elif metric == "load":
        for i in range(len(x)):
            # 保留1位小数
            x[i] = "%.1f" % float(x[i])
    return x

def plot_fig():
    css, font = get_css()
    plt.figure(dpi=150) # 设置分辨率
    plt.rc("font", **font)  # pass in the font dict as kwargs
    for metric in METRICS:
        scenarios = gen_scenarios(metric)
        for scenario in scenarios:
            lines = []
            for method in METHODS:
                exps = experiments[method]
                x = []
                y = []
                for exp in exps:
                    if judge_scenario(exp, scenario, metric):
                        x = x + [getattr(exp, metric)]
                        y = y + [exp.FCT]
                gen_x_ticks(metric, x)
                line = Line(x, y, method)
                lines.append(line)
            lines = normalize_data(lines)
            for line in lines:
                plt.plot(line.x, line.y, color=css[line.method][0],
                        linestyle=css[line.method][1],
                        marker=css[line.method][2],
                        label=css[line.method][3],
                        linewidth=css[line.method][4],
                        markersize=css[line.method][5],
                        )
            # plt.tight_layout()  # 自动调整子图参数，使之填充整个图像区域
            # plt.ylim(0.5, 1.5) # 设置y轴范围
            plt.ylabel('Normalized FCT', font)
            plt.xlabel(gen_xlabel(metric), font)
            # 显示图例  设置图例靠左
            plt.legend(loc='upper left', ncol=1, fontsize=12)
            # plt.grid(linestyle='--', linewidth=0.5) # 显示网格
            plt.savefig(gen_figname(metric, scenario), bbox_inches='tight')
            plt.clf()
            

if __name__ == "__main__":
    read_data()
    if len(experiments) == 0:
        get_data()
    plot_fig()