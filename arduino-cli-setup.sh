#!/bin/sh


#download arduino cli api
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

sketchFolderPath="/home/pi/Arduino"

yamlFile='~/.arduino15/arduino-cli.yaml'
bashFile="~/.bashrc"


#add the arduino-cli command to the bash file
echo ${exportCommand} >> $bashFile

#initalize ardino cli (may hang here, if so the system will require a reboot then try again)
arduino-cli config init

exportCommand='export PATH=$PATH:/home/pi/Arduino/bin'

#update the sketchfolder to the current working directory
sed -i "s:$sketchFolderPath:$PWD:"  $yamlFile


#add the longan board url to the boards manager
declare -a StringArray=("board_manager:" "      additional_urls:"
"       - https://raw.githubusercontent.com/Longan-Labs/Longan-RP2040/main/pa")

for val in "${StringArray[@]}"; do
 echo $val >> $yamlFile
done


#update the core index
arduino-cli core update-index

#download the longan board
arduino-cli core install longan-rp2040

#search for longan board (confirm its there)
ardunio-cli core seach longan-rp2040

#confirm that the longan board is here
arduino-cli board listall
