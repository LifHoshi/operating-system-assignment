#!/bin/bash

####################
# Lab 1 Exercise 7
# Name:
# Student No:
# Lab Group:
####################

####################
# Strings that you may need for prompt (not in order)
####################
# Enter $N numbers: 
# Enter NEW prefix (only alphabets):
# Enter prefix (only alphabets):
# INVALID
# Number of files to create:
# Please enter a valid number [0-9]:
# Please enter a valid prefix [a-z A-Z]:

####################
# Strings that you may need for prompt (not in order)
####################

#################### Steps #################### 

# Fill in the code to request user for the prefix
read -p $"Enter prefix (only alphabets):" prefix

# Check the validity of the prefix #
while [[ $prefix =~ [^a-zA-Z] ]] || [ -z $prefix ]
do 
   echo "INVALID"
   read -p $"Please enter a valid prefix [a-z A-Z]: " prefix
done

# Enter numbers and create files #
read -p $"Number of files to create: " files
while ! [[ $files =~ ^[0-9]+$ ]] || [ -z $files ]
do 
   echo "INVALID"
   read -p $"Please enter a valid number [0-9]: " files
done   

# Fill in the code to request user for the new prefix

declare -A numbers
echo "Enter $N numbers:"
for (( i=0; i<$files; i++ ))
do 
   read number
   while ! [[ $number =~ ^[0-9]+$ ]] || [ -z $number ]
   do 
      echo "INVALID"
      read -p $"Please enter a valid number [0-9]: " number
   done 
   numbers[$i]=$number
done 

for j in ${numbers[@]}
do 
   touch ${prefix}_$j.txt
done

echo ""
echo "Files Created"
ls *.txt
echo ""

# Renaming to new prefix #

read -p $"Enter NEW prefix (only alphabets):" new_prefix

# Check the validity of the prefix #
while [[ $new_prefix =~ [^a-zA-Z] ]] || [ -z $new_prefix ]
do 
   echo "INVALID"
   read -p $"Please enter a valid prefix [a-z A-Z]: " new_prefix
done

for all_files in *.txt
do 
   
   mv "$all_files" "${all_files//$prefix/$new_prefix}"
done




echo ""
echo "Files Renamed"
ls *.txt
echo ""