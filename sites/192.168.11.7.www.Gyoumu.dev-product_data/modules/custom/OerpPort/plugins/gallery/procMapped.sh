#! /bin/sh

BASEDIR=$(dirname $(readlink -f $0))/$1
PRCDIR=$BASEDIR/.processing
QUEDIR=$BASEDIR/queue
TARDIR=$BASEDIR/publish

TS=`date +%s%N`
TMPPIC=/tmp/tmpToParse_$TS
TMPACC=/tmp/tmpAccts_$TS

#seperate group dist
while ls $PRCDIR/*.jpg 2> /dev/null | grep '\(.*,.*\).jpg' > $TMPPIC
do
  cat $TMPPIC | sed 's:.*/::' > $TMPPIC
  sed 's:(\(.*\),\(.*\))\(.*\):cp "'$PRCDIR'/&" "'$PRCDIR'/(\2)\3"; mv "'$PRCDIR'/&" "'$PRCDIR'/(\1)\3":' < $TMPPIC |
    sh
done

# parse (All)
ls $BASEDIR/watermark/*.png | grep '[^-]*-0-.*\.png$' | sed 's:.*/\([^-]*\)-0-.*\.png$:\1:' > $TMPACC

for img in $(ls $PRCDIR/*.jpg 2> /dev/null | grep '(\(_all\|_all-.*\)).*\.jpg$')
do
  for acct in $(cat $TMPACC)
  do
  	echo $img | sed "s:\(.*\)/(_all\(-.*\)\?)\(.*\)$:cp \"&\" \"\1/($acct\2)\3\":" |
      sh
  done
  rm $img
done

for img in $(ls $PRCDIR/*.jpg 2> /dev/null | grep -v '(\(_all\|_all-.*\)).*\.jpg$')
do
  mv -t $QUEDIR $img
done
