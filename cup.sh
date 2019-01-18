#!/bin/bash

time_now=`date +"%Y-%m-%d__%H:%M:%S"`
## 时间
name=`uname -a | cut -d " " -f 2`
## 主机名
OS=`cat /etc/issue | cut -d " " -f 1-3 | tr " " "_"`
## OS 版本
kernel=`uname -a | cut -d " " -f 2-3`
## 内核版本
run_time=`uptime -p | tr " " "_"`
## 运行时间
ave_load=`uptime | tr " " "\n" | tail -n 3 | xargs | tr -d ","`
## 平均负载
#total_disk=$[ `df -m | grep "^/dev" | awk '{printf("%s ", $2)}' | echo $[ tr "\n" "+"` 0 ] ]    ## 不太懂的地方( 脚本内无法运行，回到终端就可以???)
disk=( `df -m | grep "^/dev" | awk '{printf("%s %s\n", $2, $3)}'` )
## 定义了一个数组，用来存放磁盘的分区信息
#echo ${#disk[@]}  ## 数组的长度
for (( i = 0; i < ${#disk[@]}; i += 2));do
	(( total_disk += ${disk[i]} ))
    (( disk_use += ${disk[i+1]} ))
done
## 磁盘总大小和使用大小
(( ave_disk = $disk_use * 100 / $total_disk ))
## 磁盘使用比例	
total_mem=`cat /proc/meminfo | head -n 1 | awk '{printf("%.f", $2 / 1024)}'`
## 内存总量
ave_mem=`free -m | head -n 2 | tail -n 1 | awk '{printf("%.f", $3 * 100 / $2)}'`
## 内存使用比例
cpu_temp=`cat /sys/class/thermal/thermal_zone0/temp | awk '{printf("%.2f", $1 /1000)}'`
## cpu温度
disk_warn=`echo $ave_disk | awk 'END{if($1 < 80) {printf("normal")} else if($1 >=80 && $1 < 90) {printf("note")} else {printf("warning")}}'`
## 磁盘警告
mem_warn=`echo $ave_mem | awk 'END{if($1 < 70) {printf("normal")} else if($1 >=70 && $1 <80) {printf("note")} else {printf("warning")}}'`
## 内存警告
cpu_warn=`echo $cpu_temp | awk 'END{if($1 < 50) {printf("normal")} else if($1 >=50 && $1 < 70) {printf("note")} else {printf("warning")}}'`
## 温度警告
echo $time_now $name $OS $kernel $run_time $ave_load $total_disk $ave_disk% $total_mem $ave_mem% $cpu_temp℃   $disk_warn $mem_warn $cpu_warn
