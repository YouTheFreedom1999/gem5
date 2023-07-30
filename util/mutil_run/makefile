
date_str=$(shell date +'%Y_%m_%d_%H_%M')

JSON?=./config/config_template.json
REPORT_DIR?=./report

VISION=@

t:
    ${VISION}echo "Run date: ${date_str}"


run:t clean
	${VISION}mkdir -p m5out
	${VISION}mkdir -p report
	${VISION}python python/run.py --configFile=$(JSON) --reportPath=$(REPORT_DIR)

collect:
	${VISION}python python/run.py --configFile=$(JSON) --reportPath=$(REPORT_DIR) --collection

backup_m5out:
	${VISION}if [ -f "m5out" ]; then \
        mv m5out backup/m5out_$(date_str); \
    fi

backup_report:
	${VISION}if [ -f "report" ]; then \
        mv report report backup/report_$(date_str); \
    fi

mk_backup:
	${VISION}mkdir -p backup

clean:mk_backup backup_report backup_m5out
	${VISION}rm -rf m5out report

.PHONY: clean mk_backup backup_m5out backup_report t