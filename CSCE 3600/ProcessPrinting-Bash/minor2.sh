#!/bin/bash

# Created by Matthew Fredericksen (mlf0220)
# CSCE 3600.003 - 02/19/2020
#
# This script prints a list of users and a count of 
# how many processes they are each running. Optionally, 
# a list of usernames may be passed from the command-line
# and only those users will be displayed

# catch ^C from user
trap 'confirm_exit' SIGINT

# handles ^C from user
confirm_exit() {
    echo " (SIGINT) received"
    
    # confirm that the user wishes to exit, continue if not
    read -p "Terminate Program? (Y/N): " confirm
    if [[ $confirm == 'Y' || $confirm == 'y' ]]; then
        echo "Terminating Program"; echo
        exit
    else
        echo "Continuing Execution"
        sleep 2
    fi
}

# Prints all users with >0 processes and their 
# number of processes. Optionally takes an 
# arbitrary number of usernames as arguments 
# and only lists those usernames.
print_processes() {
    # print column headers
    printf "%-30s %5s\n" "User ID" "Count"
    
    # Different ps flags depending on whether usernames
    # were supplied. Save results of ps command to
    # processes array
    if [ $# -gt 0 ]; then
        mapfile -t processes < <(
            ps -u "$*" -o user= --sort user | uniq -c)
    else
        mapfile -t processes < <(
            ps -eo user= --sort user | uniq -c)
    fi

    # explode the array
    # e.g. processes[0] = "6 mlf0220"
    # ---> processes[0] = "6"
    # ---> processes[1] = "mlf0220"
    processes=(${processes[*]})

    # processes array contents example:
    #   processes[0] = 6  (number of p for following user)
    #   processes[1] = mlf0220  (user for prev p count)
    #   processes[2] = 126
    #   processes[3] = root
    # array will always have even length
    
    # sum of all users' processes
    # local so it does not persist between calls
    local p_sum
    
    # loop through processes (two elements at a time)
    for ((i=0; i<${#processes[*]}; i+=2)); do
        # add this user's processes to sum
        let p_sum+=${processes[$i]}
        # print formatted username and process count
        printf "%-30s %5s\n" ${processes[$i+1]} ${processes[$i]}
    done
    
    # print number of users and total process count
    printf "%-30s %5s\n" \
           "USERS: $((${#processes[*]}/2)),  TOTAL PROCESSES:" $p_sum
}

# Loop until interrupted by ^C or killed.
# On ^C, user will be prompted to confirm exit.
while [ 1 ]; do
    echo
    # format: Sun 16 Feb 2020 11:19:06 PM CST
    date +"%a %d %b %Y %r %Z"
    
    # print processes (arguments are optional)
    print_processes $*
    sleep 5
done
