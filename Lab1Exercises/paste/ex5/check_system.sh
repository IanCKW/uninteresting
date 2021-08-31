#!/usr/bin/bash

####################
# Lab 1 Exercise 5
# Name: ian chan
# Student No: a0197089j
# Lab Group: 11
####################

# Fill the below up
hostname= 
machine_hardware= 
max_user_process_count=
user_process_count=
user_with_most_processes=
mem_free_percentage=
swap_free_percentage=

echo "Hostname: $HOSTNAME"
echo "Machine Hardware: $(uname -s) $(uname -m)"
echo "Max User Processes: $(ulimit -u)"
echo "User Processes: $(ps -u ianchan | wc -l)"
echo "User With Most Processes: $( ps -eo user|sort|uniq -c| sort -r | head -n 1 | awk '{print $2}')"
echo "Memory Free (%): $(free -t | awk 'NR == 2 {printf("%.4f"), $4/$2*100}')"
echo "Swap Free (%): $(free -t | awk 'NR == 3 {printf("%.4f"), $4/$2*100}')"
