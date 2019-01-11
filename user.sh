#!/bin/bash 

#用户信息统计(树莓派)
#时间 用户总数（非系统用户）近期活跃用户（3个） 具有ROOT权限用户 当前在线用户_登录IP_TTY

time_now=`date +"%Y-%m-%d__%H:%M:%S"`
user_count=`cat /etc/passwd | grep "[a-zA-Z0-9]" | grep -v "wtmp" | grep bash | grep -v "boot" | wc -l`
always_user=`last | cut -d " " -f 1 | tr -s " " "\n" | grep "[a-z]" | grep -v "wtmp" | sort | uniq -c | sort -n -r | head -n 3 | awk '{printf("%s\n", $2)}' | xargs | tr " " ","`
root=`cat /etc/group | sort | grep sudo | cut -d : -f 4`
tty=`w -h | awk '{printf("%s_%s_%s\n", $1,$3,$2)}' | xargs | tr " " ","`

echo $time_now [$user_count] [$always_user] [$root] [$tty]

