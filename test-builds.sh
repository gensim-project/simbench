#!/bin/sh

ARCHES=`ls -d -- arch/*`

OK=1

for THEARCH in $ARCHES; do
	ARCHNAME=`echo $THEARCH | sed 's/^[^/]*\///g'`
	echo $ARCHNAME
	
	PLATFORMS=`ls -d -- platform/$ARCHNAME/*`
	for THEPLATFORM in $PLATFORMS; do
		PLATNAME=`echo $THEPLATFORM | sed 's/^[^/]*\///g'`
		PLATNAME=`echo $PLATNAME | sed 's/^[^/]*\///g'`
		echo -n "  $PLATNAME"
		
		ARCH="$ARCHNAME" PLATFORM="$PLATNAME" make clean 2>/dev/null > /dev/null
		ARCH="$ARCHNAME" PLATFORM="$PLATNAME" make 2>/dev/null > /dev/null
		CODE="$?"
		
		if [ $CODE -eq "0" ]; then
			echo " - OK"
		else
			echo " - failed"
			OK=0
		fi
		
	done
	
done

if [ $OK -eq "0" ]; then
	echo "Some builds failed"
else
	echo "All builds succeeded"
fi

exit $OK
