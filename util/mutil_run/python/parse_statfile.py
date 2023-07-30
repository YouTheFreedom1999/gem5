import os
import re

pattern0 = r"Exiting @ tick \d+ because Normal Stop!"
pattern1 = r"Exiting @ tick \d+ because Error Stop!"
partern2 = r'--- BEGIN LIBC BACKTRACE ---'
pattern3 = r"Exiting @ tick \d+ because m5_exit instruction encountered"


def parse_statfile(file_path , find_list):

    data = {}

    with open(os.path.join(file_path,"terminal.stdout"), 'r') as file:
        for line in file:
            # print(line)
            if re.search(pattern0, line) or re.search(pattern3 , line):
                data["processStatus"] = "NormalStop"
            elif re.search(pattern1, line) or re.search(partern2 , line):
                data["processStatus"] = "*ErrorStop"
            else:
                data["processStatus"] = "**NotStop"
        
        if data["processStatus"] != "NormalStop":
            return data
    
    with open(os.path.join(file_path,"stats.txt"), "r") as file:
        for line in file:
            line = line.strip()  # 去除行首尾的空格或换行符
            if line and not line.startswith("#"):  # 忽略空行和以 "#" 开头的注释行
                key, value = line.split("#")[0].split()[:2]  # 使用空格分割行，并获取前两个元素
                if key in find_list:
                    data[key] = value  # 将键值对添加到字典中
        if "coremarkScore" in find_list :
            data["coremarkScore"] = 1e6/(int(data["simTicks"])/500/20)

    return data


# file_path="/project/Develop/CPU/design/sunjiahao/gem5_work/gem5_public/mutil_run/m5out/Sweep0_13"
# find_list=["finalTick" , "simTicks" , "simFreq" , "hostTickRate"]

# print(parse_statfile(file_path , find_list ))