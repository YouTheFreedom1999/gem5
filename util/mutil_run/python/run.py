import os
import sys
import csv
import subprocess
import argparse

from concurrent.futures import ThreadPoolExecutor
from concurrent.futures import as_completed
from parse_sweep_config_file import *
from parse_statfile import *
import time

COLORS = {
    'default': '\033[0m',
    'black': '\033[30m',
    'red': '\033[31m',
    'green': '\033[32m',
    'yellow': '\033[33m',
    'blue': '\033[34m',
    'magenta': '\033[35m',
    'cyan': '\033[36m',
    'white': '\033[37m'
}

def print_color_text(text, color):
    colored_text = f"{COLORS[color]}{text}{COLORS['default']}"
    print(colored_text , end='')

def execute_command(command):
    subprocess.run(command, shell=True)
    return True

gem_bin = "../../build/RISCV/gem5.fast"

if __name__ == "__main__":
    
    # sweep_config_file = "/project/Develop/CPU/design/sunjiahao/gem5_work/gem5/mutil_run/config/config.json"
    # report_file = "/project/Develop/CPU/design/sunjiahao/gem5_work/gem5/mutil_run/report"

    parser = argparse.ArgumentParser()

    parser.add_argument( "--configFile" , type=str , help="input json file to config sim")
    parser.add_argument( "--reportPath" , type=str , help="report output dir , must dir")
    parser.add_argument( "--collection" , action="store_true" , help="only collect data")

    if not os.path.exists(gem_bin):
        print(gem_bin)
        print('='*50)
        print("[Error] gem5.fast not exist!!!")
        print("!!!!Please Complier gem5.fast !!!!")
        print('='*50)
        sys.exit(2)
    
    args = parser.parse_args()

    sweep_config_file = args.configFile
    report_file       = args.reportPath
    isonlyColl        = args.collection

    ret_list = parse_sweep_config_file(sweep_config_file)

    # print(ret_list)
    print("#"*100)

    for sweep in range(len(ret_list)):

        if not isonlyColl:
            executor = ThreadPoolExecutor(max_workers=28) 
        
        cmd_list    = ret_list[sweep][0]
        config_list = ret_list[sweep][1]
        stats_list  = ret_list[sweep][2]
                
        print_color_text(f"[mutil_run]" ,'red')
        print_color_text(f" process Sweep{sweep}/{len(ret_list)-1}" , 'magenta')
        print(" ")

        # print(cmd_list    )
        # print(config_list )
        # print(stats_list  )

        csv_header = ["processStatus"]
        csv_header.extend(list(config_list[0]))
        csv_header.extend(stats_list)

        # print(csv_header)
        if not os.path.exists(report_file):
            os.makedirs(report_file)
        
        if isonlyColl:
            csvfilename = f"collectSweep{sweep}.csv"
        else:
            csvfilename = f"Sweep{sweep}.csv"

        with open(os.path.join(report_file , csvfilename), 'w', newline='') as file:
            writer = csv.DictWriter(file, fieldnames=csv_header)
            writer.writeheader()
        
        i = 1
        task_list = []

        if not isonlyColl:
            for cmd in cmd_list:
                print_color_text("[mutil_run]" ,'red')
                print_color_text(f" PUSH task({i}/{len(cmd_list)}) to execute \033[2K\r" ,'magenta')
                time.sleep(0.001)
                # print(cmd)
                task_list.append(executor.submit(execute_command , cmd))
                i+=1
        
        # print(" ")
        if not isonlyColl:
            finish = 1
            for task in as_completed(task_list):
                result = task.result()
                if result:
                    print_color_text("[mutil_run]" ,'red')
                    print_color_text(f" FINISH task({finish}/{len(cmd_list)})\r" ,'magenta')
                    finish+=1
        print(" ")

        if not isonlyColl:
            executor.shutdown()

            # print(executor._counter)
        
        normalCounter = 0
        errorCounter  = 0

        with open(os.path.join(report_file , csvfilename), 'a', newline='') as file:
            writer = csv.DictWriter(file, fieldnames=csv_header)
            for config in config_list:
                config.update(parse_statfile(config["outputDir"] ,stats_list))
                # print(config)
                writer.writerow(config)
                if config['processStatus'] == "NormalStop":
                    normalCounter += 1
                elif config['processStatus'] == "*ErrorStop":
                    errorCounter += 1

        print_color_text(f"[mutil_run] " ,'red')
        print_color_text(f"Normal stop task({normalCounter}/{len(cmd_list)}) | ", 'yellow') 
        print_color_text(f"Error stop task({errorCounter}/{len(cmd_list)}) "  , 'yellow') 
        print(" ")

        print_color_text(f"[mutil_run] " ,'red')
        print_color_text(f"WRITE  {csvfilename} " , 'magenta')
        print(" ")
        print("#"*100)
    