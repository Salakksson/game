#!/bin/bash

BUILD_VERSION=${1:-dev}

BUILD_DIR="build/$BUILD_VERSION"

BIN_DIR="$BUILD_DIR/bin"
SRC_DIR="src"

CXX=g++
CC=gcc
LD=g++

TARGET=game

PRECOMPILED_DIR="$BUILD_DIR/precompiled"
BUILD_CACHE="$BUILD_DIR/cache"
DEPFILE="$BUILD_CACHE/.deps"

CXXFLAGS=" "
CCFLAGS=" "
LDFLAGS="-lraylib"


PRECOMPILED="$SRC_DIR/pch.h "
# PRECOMPILED+="$(echo $SRC_DIR/std/*.h) "

case $BUILD_VERSION in
    dev) 
		CXXFLAGS+="-O0 -g0 -D DEBUG "
		CCFLAGS+="-O0 -g0 -D DEBUG "
		;;
    debug)
        CXXFLAGS+="-Og -g3 -D DEBUG "
        CCFLAGS+="-Og -g3 -D DEBUG "
		;;
    release)
        CXXFLAGS+="-O3 -g0 -flto "
        CCFLAGS+="-O3 -g0 -flto "
        LDFLAGS+="-flto "
        ;;
esac


if [ "$2" = clean ]; then
	echo rm -r "$BIN_DIR"/*
	rm -r "$BIN_DIR"/*
	exit
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN} - Building target: ${TARGET} ${NC}"
echo -e "${CYAN} - Version: ${BUILD_VERSION} ${NC}"

mkdir -p $BIN_DIR
mkdir -p $SRC_DIR
mkdir -p $BUILD_CACHE
mkdir -p $PRECOMPILED_DIR

BIN_TARGET=$BIN_DIR/$TARGET
SRC_CPP=$(find $SRC_DIR -name '*.cpp')
SRC_C=$(find $SRC_DIR -name '*.c')

OBJECTS=""
NEED_LINK=1
DEPS_GENERATED=0
DEPS_GENERATED_STRING=""

Get_deps=""
function get_deps() # file
{
	Get_deps=$(g++ -MM "$1" | sed 's/\\//g' | cut -d ':' -f 2)
	((DEPS_GENERATED++))
	DEPS_GENERATED_STRING+="$1 "
}
function store_deps() # file, deps
{
	file=$1
	deps=${@:2}
	depline="$file::$deps"
	while read -r line; 
	do
		qfile=$(echo $line | sed 's|\(.*\)::.*|\1|')
		if [ $qfile = $file ];
		then
			found=0
			break
		fi
	done < $DEPFILE
	if [ $found -eq 0 ];
	then
		sed -i "s|$line|$depline|" $DEPFILE
	else
		echo $depline >> $DEPFILE
	fi
}

Find_deps=""
function find_deps() # file, object
{
	file=$1
	object=$2
	found=1
	if [ ! -f $DEPFILE ];
	then
		touch $DEPFILE
		get_deps $file
		deps=$Get_deps
		store_deps $file $deps
		CURRENT_DEPS=$deps
		return
	elif [ $file -nt $object ];
	then
		get_deps $file
		deps=$Get_deps
		store_deps $file $deps
		CURRENT_DEPS=$deps
		return
	fi
	# Find location of file in depfile
	while read -r line; 
	do
		qfile=$(echo $line | sed 's|\(.*\)::.*|\1|')
		if [ $qfile = $file ];
		then
			found=0
			break
		fi
	done < $DEPFILE
	if [ $found -eq 0 ];
	then
		CURRENT_DEPS=$(echo $line | sed 's|.*::\(.*\)|\1|')
		return
	else
		get_deps $file
		deps=$Get_deps
		store_deps $file $deps
		CURRENT_DEPS=$deps
		return
	fi
}

function need_compile() # file, object
{
	file=$1
	object=$2
	compile=1
	# deps=$(find_deps $file $object)
	find_deps $file $object
	deps=$CURRENT_DEPS
	for dep in $deps;
	do 
		if [ $dep -nt $object ];
		then
			compile=0
			NEED_LINK=0
			break
		fi
	done
	return $compile
}

function need_link()
{
	return $NEED_LINK
}

function print_and_run()
{
	echo -e "${MAGENTA} - Running: $@ ${NC}"
	if $@;
	then
		echo -e "${GREEN} ✓ Success${NC}"
	else	
		echo -e "${RED} × Command failed: $@ ${NC}"
		exit
	fi
}

function skip()
{
	echo -e "${GREEN} × Skipped $@ ${NC}"
}

for file in $PRECOMPILED;
do
	# object=${file/$SRC_DIR/$PRECOMPILED_DIR}.gch
	object="$file".gch
	if need_compile $file $object;
	then
		print_and_run $CXX $CXXFLAGS -x c++-header $file -o $object
	else
		skip $file
	fi
done

for file in $SRC_CPP;
do
	object=${file/$SRC_DIR/$BIN_DIR}.o
	if [ $file -nt $object ];
	then
		print_and_run $CXX $CXXFLAGS -c $file -o $object -MMD -MF /tmp/build.deps
		deps=$(cat /tmp/build.deps | sed 's/\\//g' | cut -d ':' -f 2)
		store_deps $file $deps
		((DEPS_GENERATED++))
		DEPS_GENERATED_STRING+="$file "
		continue
	fi
	if need_compile $file $object;
	then
		print_and_run $CXX $CXXFLAGS -c $file -o $object
	else
		skip $file
	fi
	OBJECTS+=" $object"
done

for file in $SRC_C;
do
	object=${file/$SRC_DIR/$BIN_DIR}.o
	if need_compile $file $object;
	then
		print_and_run $CC $CCFLAGS -c $file -o $object
	else
		skip $file
	fi
	OBJECTS+=" $object"
done

if [ $DEPS_GENERATED -gt 0 ];
then
	echo -e "${CYAN} - Generated $DEPS_GENERATED dependancy list(s) in $DEPFILE for files: $DEPS_GENERATED_STRING ${NC}"
fi

for object in $OBJECTS;
do
	if [ $object -nt $BIN_TARGET ];
	then
		NEED_LINK=0
		break
	fi
done

if need_link;
then
	print_and_run $LD $LDFLAGS $OBJECTS -o $BIN_TARGET
	print_and_run cp $BIN_TARGET $TARGET
else
	skip linking
fi

if [ $BUILD_VERSION = debug ];
then
	print_and_run gdb $TARGET
elif [ $BUILD_VERSION = dev ];
then
	echo print_and_run $TARGET
fi
