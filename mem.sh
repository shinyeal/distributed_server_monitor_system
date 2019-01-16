#!/bin/bash

#mem 内存信息获取

time_now=`date +"%Y-%m-%d__%H:%M:%S"`
#last=$1
last=10
if [ ! $last ];then
    echo "error"
else
    use=( `free -m | head -n 2 | tail -n 1 | awk '{printf("%s %s %s", $2, $2-$3, $3)}'` )
    present=`echo "scale=1;${use[2]} * 100 / ${use[0]}" | bc`
    equal=`echo "scale=1;0.7 * ${present} + 0.3 * ${last}" | bc`

    echo ${time_now} ${use[0]} ${use[1]} ${present}% ${equal}%
fi
