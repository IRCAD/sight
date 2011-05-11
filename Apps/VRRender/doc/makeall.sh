#!/bin/bash
# -*- coding: UTF8 -*-
PROJ=VRRender

test  $(basename $(dirname $PWD)) = "$PROJ" && (


make html htmlhelp latex && (

(
cd build
#patch -p1 < ../patch.cedil
)

(
cd build/latex
make all-pdf
)

find build/ -type d -name .svn|xargs rm -rf

./htmlhelpTowxHtmlHelp build/htmlhelp

(
cd build/htmlhelp/_images
mogrify -resize "1024>" *.jpg *.png
)


(
cd build/html/_images
mogrify -resize "850>" *.jpg
)

(
rm ../rc/doc.zip
cd build/htmlhelp/
zip -r ../../../rc/doc.zip ./
)

) || echo Error

) || echo "Erreur: A executer dans $PROJ/doc"
