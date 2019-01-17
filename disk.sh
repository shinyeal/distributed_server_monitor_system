#!/bin/bash

time_now=`date +"%Y-%m-%d__%H:%M:%S"`
disk=( `df -m | grep "^/dev" | awk '{printf("%s %s %s %s\n", $2, $4, $5, $6)}'` )
#echo ${#disk[@]}

for (( i = 0; i < ${#disk[@]}; i += 4 ));do
	(( disk_sum += ${disk[i]} ))
	(( disk_sum_use += ${disk[i + 1]} ))
	echo $time_now 1 ${disk[i+3]} ${disk[i]} ${disk[i+1]} ${disk[i+2]}
done 

(( ave_disk = $disk_sum_use * 100 / $disk_sum  ))

echo $time_now 0 disk $disk_sum $disk_sum_use $ave_disk%

