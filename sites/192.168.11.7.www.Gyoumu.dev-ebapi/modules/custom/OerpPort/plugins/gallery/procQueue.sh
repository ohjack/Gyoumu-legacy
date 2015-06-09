#!/bin/bash

BASEDIR=$(dirname $(readlink -f $0))/$1
QUEDIR=$BASEDIR/queue
TARDIR=$BASEDIR/publish

LIMIT=1000
umask 000

# assign default wm
for img in $(ls $QUEDIR/*.jpg 2> /dev/null | grep -v '(.*-.*).*\.jpg')
do
  echo $img | sed 's:\(.*\)/(\(.*\))\(.*\)$:mv "&" "\1/(\2-1)\3":' |
    sh
done

# manipulate
C=1

for img in $(ls $QUEDIR/*.jpg 2> /dev/null)
do
  target=$(echo $img | sed 's:.*/(.*)\(.*\)$:\1:')
  tag=$(echo $img | sed 's:.*/(\(.*\)).*\.jpg$:\1:')
  wm=$(ls $BASEDIR/watermark/$tag*.png 2> /dev/null)

  if [ $? -eq 0 ];	then
  	wm=$(echo $wm | head -n 1)
  	tok='\([^-]*\)-\?'
  	pat=$tok$tok$tok$tok

  	acc=$(echo $wm | sed 's:.*/'$pat'.png$:\1:')

  	mrg=$(echo $wm | sed 's:.*/'$pat'.png$:\3:')
  	mrg=$(echo ${mrg:-over})
  	
  	dim=$(echo $wm | sed 's:.*/'$pat'.png$:\4:')
  	dim=$(echo ${dim:-800x600})

  	ls -d $TARDIR/$acc 1> /dev/null 2>&1
  
  	if [ $? != 0 ]; then
	    mkdir -m 777 $TARDIR/$acc
  	fi

  	convert $img -resize $dim^ -gravity center -extent $dim -compose $mrg $wm -composite $img
   	mv $img $TARDIR/$acc/$target
  	echo $TARDIR/$acc/$target

  else
    echo $img | grep '.*/(.*-_del_)'
    if [ $? -eq 0 ]; then
      acc=$(echo $img | sed 's:.*/(\(.*\)-_del_).*:\1:')
    	ls -d $TARDIR/$acc 1> /dev/null 2>&1
  
    	if [ $? != 0 ]; then
  	    mkdir -m 777 $TARDIR/$acc
    	fi

      cp $BASEDIR/blank.jpg $TARDIR/$acc/$target
      rm $img
    fi
  fi

  if [[ $acc == rakuten_* ]]; then
    echo $target >> $BASEDIR'/to_'$acc'_new'
  fi

  if [[ $acc == Amazon_* ]]; then
    echo $target >> $BASEDIR'/to_'$acc'_new'
  fi

  if [[ $acc == YahooShop_* ]]; then
    echo $target >> $BASEDIR'/to_'$acc'_new'    
  fi

  C=`expr $C + 1`
  if [ $C -gt $LIMIT ]; then
    break
  fi
done

