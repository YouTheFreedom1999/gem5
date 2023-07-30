import sys
import time

def print_progress_bar(iteration, total, prefix='', suffix='', decimals=1, length=50, fill='█'):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filled_length = int(length * iteration // total)
    bar = fill * filled_length + '-' * (length - filled_length)
    sys.stdout.write('\r%s |%s| %s%% %s' % (prefix, bar, percent, suffix))
    sys.stdout.flush()
    
# 示例用法：
total_items = 1000

for i in range(total_items):
    time.sleep(0.01)
    print_progress_bar(i+1, total_items, prefix='Progress:', suffix='Complete', length=50)
    # 执行任务代码...
    print("")
