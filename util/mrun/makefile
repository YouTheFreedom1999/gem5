
date_str=$(shell date +'%Y_%m_%d_%H_%M')

JSON?=config/k800Config.json
REPORT_DIR?=./report
PYTHON?=
UBUNTU=1
NEEDCLEAN=0
ifeq (${UBUNTU},1)
PYTHON=/project/Develop/CPU/design/sunjiahao/DontRemove/anaconda/bin/python3
else
PYTHON=/opt/anaconda3.7/bin/python3
endif

VISION=@

t:
    ${VISION}echo "Run date: ${date_str}"

run:t clean
	${VISION}mkdir -p m5out
	${VISION}mkdir -p report
	${VISION}${PYTHON} python/run.py --configFile=$(JSON) --reportPath=$(REPORT_DIR)

collect:
	${VISION}${PYTHON} python/run.py --configFile=$(JSON) --reportPath=$(REPORT_DIR) --collection

backup_m5out:
	${VISION}if [ -d "m5out" ]; then \
        mv m5out backup/m5out_$(date_str); \
    fi

backup_report:
	${VISION}if [ -d "report" ]; then \
        mv report backup/report_$(date_str); \
    fi

mk_backup:
	${VISION}mkdir -p backup

clean:mk_backup backup_report backup_m5out
	${VISION}rm -rf m5out report

.PHONY: clean mk_backup backup_m5out backup_report t
