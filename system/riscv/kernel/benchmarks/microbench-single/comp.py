import os
import subprocess
import shutil

def execute_make(mainargs, sel):
    
    sel_name = ["qsort","queen",   "bf",  "fib","sieve", "15pz","dinic", "lzip","ssort",  "md5"]
    # 执行make命令
    command = f"make mainargs={mainargs} sel={sel}"
    subprocess.run(command, shell=True, check=True)

    # 检查build目录下的文件
    build_dir = "build"
    files = ['microbench.txt', 'microbench.elf', 'microbench.bin']
    for file in files:
        if not os.path.exists(os.path.join(build_dir, file)):
            print(f"Failed to generate {file}")
            return

    # 复制文件到gen目录下并按需重命名
    gen_dir = "gen"
    if not os.path.exists(gen_dir):
        os.makedirs(gen_dir)

    for file in files:
        dest_filename = f"microbench-{mainargs}-{sel_name[int(sel)]}{os.path.splitext(file)[1]}"
        # print(dest_filename)
        shutil.copy2(os.path.join(build_dir, file), os.path.join(gen_dir, dest_filename))

    print("Make completed successfully.")

mainargs_valid = ['test', 'train', 'ref', 'huge']
    
for mainargs in mainargs_valid:
    for i in range(0,10):
        execute_make(mainargs, i)