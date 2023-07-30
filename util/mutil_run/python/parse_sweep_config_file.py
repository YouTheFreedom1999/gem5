import json5
import os
import sys

def generate_command(parameter_dict):

    # print (parameter_dict )

    command = "../../build/RISCV/gem5.fast --silent-redirect -r --stdout-file=terminal.stdout "  
    command += "--outdir=" + parameter_dict['outputDir'] 
    command += " ../../configs/example/riscv/fs_linux_user.py "
    command += " --cpu-type=RiscvO3CPU --caches "

    for parameter in parameter_dict:
        if parameter != "outputDir":
            if(type(parameter_dict[parameter]) is bool):
                if(parameter_dict[parameter]):
                    command = command + " --" + parameter
            else:
                command = command + " --" + \
                    parameter + "=" + str(parameter_dict[parameter])
    return command

def parse_range_dict(range_dict):

    if "start" in range_dict:
        start = range_dict["start"]
    else:
        start = 1

    if "end" in range_dict:
        end = range_dict["end"]+1
    else:
        end = 2

    if "step" in range_dict:
        step = range_dict["step"]
    else:
        step = 1

    if "unit" in range_dict:
        unit = range_dict["unit"]
    else:
        unit = ""

    value_list = []
    for i in range(start, end, step):
        value_list.append(str(i)+unit)
    return value_list


def search_file(folder_path):

    # 初始化两个列表
    absolute_paths = []
    # file_names = []

    # 遍历目标文件夹中的所有文件
    for file in os.listdir(folder_path):
        
        file_name, file_extension = os.path.splitext(file)
            
        # 检查是否没有扩展名
        if file_extension == '.elf':
            # 添加路径到列表
            absolute_paths.append(os.path.join(folder_path, file))
            
            # 添加文件名到列表
            # file_names.append(file)
    
    return absolute_paths


def parse_sweep_config_file(sweep_config_file):
    return_list = []
    config_json = json5.load(open(sweep_config_file, 'r', encoding='utf-8'))
    for Sweep in config_json:
        outputDir = str(Sweep)
        command_list = []
        keystats_list = []
        parameter_unfold = []

        # 解析json dict
        parameter_dict = {}
        for config in config_json[Sweep]:
            if config == "outputDir":
                outputDir = config_json[Sweep]["outputDir"]

            elif config == "workloadConfig":
                tmp_list = []
                for wkld in config_json[Sweep]["workloadConfig"]:
                    if not os.path.exists(wkld):
                        print(wkld)
                        print("Error file not exist!!")
                        sys.exit(2)

                    if os.path.isfile(wkld):
                        tmp_list.append(wkld)
                    elif os.path.isdir(wkld):
                        tmp_list.extend(search_file(wkld))
                parameter_dict["kernel"] = tmp_list
                # print(parameter_dict["kernel"])

            elif "Config" in config:
                for config_item in config_json[Sweep][config]:
                    if type(config_json[Sweep][config][config_item]) is dict:
                        parameter_dict[config_item] = parse_range_dict(config_json[Sweep][config][config_item])
                    elif type(config_json[Sweep][config][config_item]) is list:
                        parameter_dict[config_item] = config_json[Sweep][config][config_item]
                    else:
                        parameter_dict[config_item] = [
                            config_json[Sweep][config][config_item]]

            elif config == "keyStats":
                keystats_list = config_json[Sweep]["keyStats"]
        
                # print(parameter_dict)

        # 软件模拟栈，遍历生成扫频参数
        parameter_list = list(parameter_dict.keys())
        parameter_num = len(parameter_list)

        pointer = [0 for _ in range(len(parameter_list))]
        target = [len(parameter_dict[i])-1 for i in parameter_list]
        counter = 0
        # print(pointer)
        # print(target)

        while 1:
            # print(pointer)
            # 生成单个参数
            parameter_dict_slice = {}
            for i in range(parameter_num):
                parameter_dict_slice[parameter_list[i]] = parameter_dict[parameter_list[i]][pointer[i]]
            parameter_unfold.append(parameter_dict_slice)
            parameter_dict_slice["outputDir"] = outputDir + "_" + str(counter)
            command_list.append(generate_command(parameter_dict_slice))
            # print(generate_command(parameter_dict_slice))

            # 维护栈
            if pointer == target:
                break
            for i in range(parameter_num):
                if pointer[i+1:parameter_num] == target[i+1:parameter_num]:
                    pointer[i] = pointer[i] + 1
                    pointer[i+1:parameter_num] = [0] * (parameter_num-i-1)
                    break
            counter = counter + 1

        return_list.append([command_list, parameter_unfold, keystats_list])
    
    return return_list



# sweep_config_file = "/home/axe/gem5_work/gem5/util/mutil_run/config/config_template.json"
# print(parse_sweep_config_file(sweep_config_file))
