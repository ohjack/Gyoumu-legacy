#! /bin/sh

BASEDIR=$(dirname $(readlink -f $0))/$1
INDIR=$BASEDIR/input
PRCDIR=$BASEDIR/.procInput
DESTDIR=$BASEDIR/tofix/fixed

TS=`date +%s%N`

#move non-empty dirs to process
find $INDIR -mindepth 1 -maxdepth 1 -not -empty -type d -print0 |
  xargs -0 -I {} mv -t $PRCDIR {}

pattern="$PRCDIR/\(.\{8\}\).*"

#rename & convert to JPG
OIFS=$IFS
IFS='
'

for img in $(find $PRCDIR -maxdepth 2 \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.bmp' -o -iname '*.png' -o -iname '*.gif' \) )
do
    timestamp=$(expr substr $(date +%s%N) 1 13)
    echo $img |
      sed 's:'$pattern':convert -resize 1024x1024\\> "&" "'$DESTDIR'/\1_'$timestamp'.jpg":' |
        sh

    rm -f $img
done

#remove processed
rm -Rf $PRCDIR/*

#create new toPhoto
for dir in $(cat $BASEDIR/toPhoto)
do
  ls -d "$INDIR/$dir" 1> /dev/null 2>&1
    if [ $? != 0 ]
      then
      mkdir -m 777 "$INDIR/$dir"
    fi
done

echo '' > $BASEDIR/toPhoto
IFS=$OIFS


