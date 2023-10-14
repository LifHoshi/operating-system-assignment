#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Wang Lifu
# Student No: A0245880X
# Lab Group: 
####################

# Fill the below up
hostname=$(uname -n)
machine_hardware=$(uname -sm)
max_process_id=$(cat /proc/sys/kernel/pid_max)
user_process_count=$(ps -U $(whoami) | wc -l)
user_with_most_processes=$(ps -eo user|sort|uniq -c|sort -n|tail -1|awk '{print $2}')
mem_free_percentage=$(free | grep "Mem"| awk '{print ($4/$2) * 100.0}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max Process ID: $max_process_id"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
