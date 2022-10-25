#!/bin/bash

#
# Parameters
#		$1 - Project type: "lib", "dll", "console", "winapp", "driver", "klib"
#		$2 - Project name: For example, "myproject"
#


lowercase(){
    echo "$1" | sed "y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/"
}

uppercase(){
    echo "$1" | sed "y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/"
}

if [ $1. == . ]; then	
	echo " "
	echo "genproj.sh <ProjectType> <ProjectName>"
	echo " "
	echo "    ProjectType - Should be one of 'winapp', 'console', 'dll'"
	echo "                  'mfcdll', 'lib', 'driver' and 'klib'"
	echo "    ProjectName - The name of project, no space"
	exit 1
fi

ProjectType=`lowercase $1`
ProjectName=`lowercase $2`

if [ $ProjectType == help ]; then	
	echo " "
	echo "genproj.sh <ProjectType> <ProjectName>"
	echo " "
	echo "    ProjectType - Should be one of 'winapp', 'console', 'dll'"
	echo "                  'mfcdll', 'lib', 'driver' and 'klib'"
	echo "    ProjectName - The name of project, no space"
	exit 0
fi

if [ $2. == . ]; then
	echo "ERROR: ProjectName is not defined"
	exit 1
fi

if [ -d $ProjectName ]; then
	echo "ERROR: Project directory already exist"
	exit 1
fi

echo "Create project directory: $ProjectName"
mkdir $ProjectName
cd $ProjectName

ProjectGuid=`uuidgen`
ProjectGuid=`uppercase $ProjectGuid`
if [ $ProjectGuid. == . ]; then	
	echo "ERROR: Fail to generate new project GUID"
	exit 1
fi
echo Generate new project uuid $ProjectGuid

if [ "{$ProjectType}" == "{console}" ]; then
	cp $WORKSPACE/build/templates/cmdproject/cmdproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/cmdproject/cmdproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/cmdproject/cmdproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/cmdproject/cmdproject.rc ./$ProjectName.rc
	cp $WORKSPACE/build/templates/cmdproject/base.manifest ./base.manifest
	cp $WORKSPACE/build/templates/cmdproject/cmdproject.cpp ./$ProjectName.cpp
	cp $WORKSPACE/build/templates/cmdproject/stdafx.cpp ./stdafx.cpp
	cp $WORKSPACE/build/templates/cmdproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/cmdproject/targetver.h ./targetver.h
	cp $WORKSPACE/build/templates/cmdproject/resource.h ./resource.h
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj.filters
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.rc
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
elif [ "{$ProjectType}" == "{winapp}" ]; then
	cp $WORKSPACE/build/templates/winproject/winproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/winproject/winproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/winproject/winproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/winproject/winproject.rc ./$ProjectName.rc
	cp $WORKSPACE/build/templates/winproject/base.manifest ./base.manifest
	cp $WORKSPACE/build/templates/winproject/winmain.cpp ./winmain.cpp
	cp $WORKSPACE/build/templates/winproject/winmain.h ./winmain.h
	cp $WORKSPACE/build/templates/winproject/stdafx.cpp ./stdafx.cpp
	cp $WORKSPACE/build/templates/winproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/winproject/targetver.h ./targetver.h
	cp $WORKSPACE/build/templates/winproject/resource.h ./resource.h
	mkdir ./res
	cp $WORKSPACE/build/templates/winproject/res/small.ico ./res/small.ico
	cp $WORKSPACE/build/templates/winproject/res/large.ico ./res/large.ico
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/dllproject/dllproject.vcxproj.filters $ProjectName.vcxproj.filters
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.rc
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
elif [ "{$ProjectType}" == "{dll}" ]; then
	cp $WORKSPACE/build/templates/dllproject/dllproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/dllproject/dllproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/dllproject/dllproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/dllproject/dllproject.rc ./$ProjectName.rc
	cp $WORKSPACE/build/templates/dllproject/dllproject.def ./$ProjectName.def
	cp $WORKSPACE/build/templates/dllproject/dllproject.cpp ./$ProjectName.cpp
	cp $WORKSPACE/build/templates/dllproject/stdafx.cpp ./stdafx.cpp
	cp $WORKSPACE/build/templates/dllproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/dllproject/targetver.h ./targetver.h
	cp $WORKSPACE/build/templates/dllproject/resource.h ./resource.h
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj.filters
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.def
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.rc
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
elif [ "{$ProjectType}" == "{mfcdll}" ]; then
	cp $WORKSPACE/build/templates/mfcdllproject/mfcdllproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/mfcdllproject/mfcdllproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/mfcdllproject/mfcdllproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/mfcdllproject/mfcdllproject.rc ./$ProjectName.rc
	cp $WORKSPACE/build/templates/mfcdllproject/mfcdllproject.def ./$ProjectName.def
	cp $WORKSPACE/build/templates/mfcdllproject/base.manifest ./base.manifest
	cp $WORKSPACE/build/templates/mfcdllproject/mfcdllapp.cpp ./mfcdllapp.cpp
	cp $WORKSPACE/build/templates/mfcdllproject/mfcdllapp.h ./mfcdllapp.h
	cp $WORKSPACE/build/templates/mfcdllproject/stdafx.cpp ./stdafx.cpp
	cp $WORKSPACE/build/templates/mfcdllproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/mfcdllproject/targetver.h ./targetver.h
	cp $WORKSPACE/build/templates/mfcdllproject/resource.h ./resource.h
	mkdir ./res
	cp $WORKSPACE/build/templates/mfcdllproject/res/external.rc2 ./res/external.rc2
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj.filters
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.def
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.rc
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
elif [ "{$ProjectType}" == "{comdll}" ]; then
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.rc ./$ProjectName.rc
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.def ./$ProjectName.def
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.idl ./$ProjectName.idl
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.rgs ./base.rgs
	cp $WORKSPACE/build/templates/comdllproject/comdllproject.cpp ./$ProjectName.cpp
	cp $WORKSPACE/build/templates/comdllproject/stdafx.cpp ./stdafx.cpp
	cp $WORKSPACE/build/templates/comdllproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/comdllproject/dllmain.cpp ./dllmain.cpp
	cp $WORKSPACE/build/templates/comdllproject/dllmain.h ./dllmain.h
	cp $WORKSPACE/build/templates/comdllproject/targetver.h ./targetver.h
	cp $WORKSPACE/build/templates/comdllproject/resource.h ./resource.h
	ComDllGuid=`uuidgen`
	ComDllGuid=`uppercase $ProjectGuid`
	if [ $ComDllGuid. == . ]; then	
		echo "ERROR: Fail to generate new com GUID"
		exit 1
	fi
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj.filters
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.cpp
	sed -i "s/PROJECTNAME/$ProjectName/g" dllmain.cpp
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.rc
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.idl
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
	sed -i "s/COMDLLGUID/$ComDllGuid/g" $ProjectName.idl
elif [ "{$ProjectType}" == "{lib}" ]; then
	cp $WORKSPACE/build/templates/libproject/libproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/libproject/libproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/libproject/libproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/libproject/libproject.cpp ./$ProjectName.cpp
	cp $WORKSPACE/build/templates/libproject/stdafx.cpp ./stdafx.cpp
	cp $WORKSPACE/build/templates/libproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/libproject/targetver.h ./targetver.h
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj.filters
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
elif [ "{$ProjectType}" == "{klib}" ]; then
	cp $WORKSPACE/build/templates/libproject/libproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/libproject/libproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/libproject/libproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/libproject/stdafx.cpp ./stdafx.cpp
	cp $WORKSPACE/build/templates/libproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/libproject/targetver.h ./targetver.h
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
elif [ "{$ProjectType}" == "{driver}" ]; then
	cp $WORKSPACE/build/templates/drvproject/drvproject.sln $ProjectName.sln
	cp $WORKSPACE/build/templates/drvproject/drvproject.vcxproj $ProjectName.vcxproj
	cp $WORKSPACE/build/templates/drvproject/drvproject.vcxproj.filters $ProjectName.vcxproj.filters
	cp $WORKSPACE/build/templates/drvproject/drvmain.c ./$ProjectName.c
	cp $WORKSPACE/build/templates/drvproject/drvmain.h ./$ProjectName.h
	cp $WORKSPACE/build/templates/libproject/stdafx.h ./stdafx.h
	cp $WORKSPACE/build/templates/libproject/targetver.h ./targetver.h
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.sln
	sed -i "s/PROJECTNAME/$ProjectName/g" $ProjectName.vcxproj
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.sln
	sed -i "s/PROJECTGUID/$ProjectGuid/g" $ProjectName.vcxproj
else
	echo "ERROR: Unknown ProjectType - $ProjectType"
	exit 1
fi

echo "Project has been created successfully"
echo "    Name: $ProjectName"
echo "    Type: $ProjectType"
echo "    Guid: $ProjectGuid"
echo "    "