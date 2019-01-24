#!/bin/bash

time_now=`date +"%Y-%m-%d__%H:%M:%S"`
## 时间
ave_load=`uptime | tr " " "\n" | tail -n 3 | xargs | tr -d ","`
## 平均负载
ave_use1=( `cat /proc/stat | head -n 1 | awk '{printf("%s %s\n", $2+$3+$4+$5+$6+$7+$8+$9+$10+$11, $5)}'` )
sleep 0.5s
ave_use2=( `cat /proc/stat | head -n 1 | awk '{printf("%s %s\n", $2+$3+$4+$5+$6+$7+$8+$9+$10+$11, $5)}'` )
(( use1 = ${ave_use2[0]}-${ave_use1[0]} ))
(( use2 = ${ave_use2[1]}-${ave_use1[1]} ))
(( ave_use = 100 - $use2 * 100 / $use1 ))
## 占用率
cpu_temp=`cat /sys/class/thermal/thermal_zone0/temp | awk '{printf("%.2f",$1/1000)}'`
## cpu温度
cpu_warn=`echo $cpu_temp | awk 'END{if($1 < 50) {printf("normal")} else if($1 >=50 && $1 < 70) {printf("note")} else {printf("warning")}}'`

echo $time_now $ave_load $ave_use% $cpu_temp℃   $cpu_warn
