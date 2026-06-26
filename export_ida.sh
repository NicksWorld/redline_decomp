#!/bin/bash
reccmp-roadmap --target REDLINE --csv offsets.csv
cat offsets.csv | grep ",fun\|,dat" | awk '!/\.dll/' | cut -d ',' -f 3,8 --output-delimiter " " | awk '{if ($2 != "") {print "MakeName("$1", \""$2"\");"}}' | tr '~' 'Destruct' > ida_cmd
cat ida_cmd | xsel -ib
