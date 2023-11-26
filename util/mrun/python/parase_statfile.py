import os
import re
import json5
from TMA import *

pattern0 = r"Exiting @ tick \d+ because Normal Stop!"
pattern1 = r"Exiting @ tick \d+ because Error Stop!"
decimal_num_pattern = r"\d+"


def update_stats(stats_list):
    append_list = []
    if "coremarkScore" in stats_list:
        append_list.append("system.cpu.numCycles")
    if "TMA" in stats_list or "TMAPlot" in stats_list:
        append_list.extend(TMA_statas(stats_list))
    return append_list


def parse_statfile(config, stats_find_list):

    output_dir = config["outputDir"]
    stats_dict = {}

    TMA_find_list = []
    TMA_stats_dict = {}
    if "TMA" in stats_find_list or "TMAPlot" in stats_find_list:
        TMA_find_list = TMA_additional_find(output_dir)

    with open(os.path.join(output_dir, "terminal.stdout"), "r") as file:
        for line in file:
            if re.search(pattern0, line):
                stats_dict["processStatus"] = "NormalStop"
            elif re.search(pattern1, line):
                stats_dict["processStatus"] = "*ErrorStop"
            else:
                # print(line)
                stats_dict["processStatus"] = "**NotStop"

    if stats_dict["processStatus"] != "NormalStop":
        return stats_dict

    with open(os.path.join(output_dir, "stats.txt"), "r") as file:
        for line in file:
            line = line.strip()  # 去除行首尾的空格或换行符
            if line and not line.startswith("#"):  # 忽略空行和以 "#" 开头的注释行
                key, value = line.split("#")[0].split()[:2]  # 使用空格分割行，并获取前两个元素
                if key in stats_find_list:
                    stats_dict[key] = value  # 将键值对添加到字典中
                if key in TMA_find_list:
                    TMA_stats_dict[key] = float(value)  # 将键值对添加到字典中

        if (
            "coremarkScore" in stats_find_list
            and "coremark" in config["kernel"].lower()
        ):
            coremark_exe_name = config["kernel"].split("/")[-1]
            coremark_iteration_times = int(
                re.findall(decimal_num_pattern, coremark_exe_name)[-1]
            )
            # print("coremark_iteration_times: ", coremark_iteration_times)
            stats_dict["coremarkScore"] = 1e6 / (
                int(stats_dict["system.cpu.numCycles"])
                / coremark_iteration_times
            )
        elif "coremarkScore" in stats_find_list:
            stats_dict["coremarkScore"] = -1

        if "TMA" in stats_find_list or "TMAPlot" in stats_find_list:
            for key in TMA_find_list:
                if key not in TMA_stats_dict:
                    TMA_stats_dict[key] = 0
            compute_TMA(stats_dict, TMA_stats_dict)

            output_path = os.path.join(output_dir, "TMA.png")
            TMA_plot(config, stats_dict, output_path)

    return stats_dict
