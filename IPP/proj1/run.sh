#!/bin/sh
INTERPRET="php -d open_dir=\"\""
SRCNAME="main.php"
ABSOL_PATH="/home/xvecer18/Documents/Sem4_projects/IPP/proj1/"

## testovani volani help
# $INTERPRET $SRCNAME --help 
# echo "return $?" 
# $INTERPRET $SRCNAME --help --input=./file01.in
# echo "return $?" 
############################################################################

##testovani zpracovani vstupnich parametru
# $INTERPRET $SRCNAME
# echo "<empty>: return = $?"
# $INTERPRET $SRCNAME -g
# echo "-g: return = $?"
# $INTERPRET $SRCNAME -a
# echo "-a: return = $?"
# $INTERPRET $SRCNAME --help
# echo "--help: return = $?"
# $INTERPRET $SRCNAME --etc=2 -g
# echo "--etc=2 -g: return = $?"
# $INTERPRET $SRCNAME --etc=2 
# echo "--etc=2: return = $?"
############################################################################

## testovani nacitani souboru pri ruznych typech cest
# $INTERPRET $SRCNAME --input=./file01.txt 
# echo "--input=(relative): return = $?"
# $INTERPRET $SRCNAME --input=${ABSOL_PATH}file01.txt 
# echo ${ABSOL_PATH}file01.txt
# echo "--input=(absolute): return = $?"
# $INTERPRET $SRCNAME --output=./file02.txt 
# echo "--output=(relative): return = $?"
# $INTERPRET $SRCNAME --output=${ABSOL_PATH}file02.txt 
# echo "--output=(absolute): return = $?"
# $INTERPRET $SRCNAME --header="Toto je hlavicka a bude vlozena na zacatek vystupu"
# echo "--header=(string): return = $?"
############################################################################

## nacteni a vypis souboru 
# $INTERPRET $SRCNAME --input=./file01.in --output=./file01_in.out 
# echo "--input & --output: return = $?"
# rm ./file01_in.out
# $INTERPRET $SRCNAME --output=./file01_in.out < ./file01.in
# echo "STDIN & --output: return = $?"
# rm ./file02_in.out
# $INTERPRET $SRCNAME --input=./file01.in > ./file02_in.out
# echo "--input & STDOUT: return = $?"
# rm ./file03_in.out
# $INTERPRET $SRCNAME < file01.in > ./file03_in.out
# echo "STDIN & STDOUT: return = $?"
############################################################################

