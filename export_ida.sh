#!/bin/bash
reccmp-roadmap --target REDLINE --csv offsets.csv
cat offsets.csv | grep ",fun\|,dat" | awk '!/\.dll/' | cut -d ',' -f 3,8 --output-delimiter "," | awk '{split($0,a,","); if (a[1] != "") {print "MakeName("a[1]", \""a[2]"\");"}}' | tr '~' 'D' | tr '`' '_' > ida_cmd
cat ida_cmd
