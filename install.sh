#!/bin/bash
#This script install S-GUI benchmark suite

HELP="This script install the S-GUI benchmark suite\n
Usage: install.sh [OPTIONS=*]\n
OPTIONS:
-h, --help : display help
-j, --proc-num: number of processors to use with make -j
-q, --qmake: custom qmake command
#--hidpi: enable hidpi support"
DIRECTORY_CHECK_DEP=benchmark-suite/utilities/check_dependencies.sh
PROC=1
RETINA=NO
QMAKE=qmake

# Setup variables
for i in "$@"
do
case $i in
    -j=*|--proc-num=*)
    PROC="${i#*=}"
    shift 
    ;;
    -q=*|--qmake=*)
    QMAKE="${i#*=}"
    shift
    ;;
    --hidpi)
    RETINA=YES
    shift 
    ;;
	-h|--help)
	echo -e "$HELP"
	exit 0
	;;
    *)
    ;;
esac
done

# Check dependency script existence
if [ ! -e "$DIRECTORY_CHECK_DEP" ]; 
then
	echo "$DIRECTORY_CHECK_DEP not found. Aborting..."
	exit 1
fi

echo "Checking S dependencies..."
./$DIRECTORY_CHECK_DEP awk time fio iostat dd xterm

if [ $? == "1" ]
then 
	exit 1
fi 

echo "Checking QT 5..." 


OS=$(lsb_release -si)
echo "System distribution: $OS"

if [ "$QMAKE" == "" ];
then
	if [[ "$OS" == "Debian" || "$OS" == "Ubuntu" ]];
	then
		QMAKE=qmake
	else
		QMAKE=qmake-qt5
	fi
fi

# Check qmake existence
type $QMAKE >/dev/null 2>&1 || 
			{ echo -e >&2 "qmake command it's not installed.\nTry to install qt5 package.\nAborting..."; exit 1; }

$QMAKE >/dev/null 2>&1 || 
			{ echo -e >&2 "qmake command error.\nTry to install qt5 package.Aborting..."; exit 1; }

#Check Qt version 5
$QMAKE -v | grep -q 'Using Qt version 5'

if [ $? -eq 1 ]
then
	echo "Qt version error: must be grater then 5.0."
	VERSION=$($QMAKE -v | grep "Using" | awk '{print $4}')
	echo "Current Qt version: $VERSION"
	echo "Correct version of Qt...no"
	echo "Try to install qt5 package.Aborting..."
	exit 1
fi

echo "Correct version of Qt...yes"

if [ ! -e "S-GUI.pro" ]; 
then
	echo "S-GUI.pro not found. Aborting..."
	exit 1
fi

echo "Running qmake..."
$QMAKE S-GUI.pro

echo "Running make..."
make -j$PROC

if [ $? -eq 0 ];
then
	echo "Creating desktop shortcut..."
	touch S-GUI.desktop
	echo -e "[Desktop Entry]\n
Version=1.0
Type=Application
Terminal=false
Exec=gksu -k $PWD/S-GUI
Name=S-GUI
Comment=Benchmarking tools
Icon=$PWD/algodevlogo.png" > S-GUI.desktop
	chmod +x S-GUI.desktop
	echo "Installation finished. Run with sudo -E ./S-GUI"
fi


