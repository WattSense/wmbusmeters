#!/bin/sh

if [ -z $1 ]; then
    OUTPUT="/dev/stdout"
else
    OUTPUT=$1
fi

echo 'R"MANUAL(' > ${OUTPUT}
sed -n '/wmbusmeters version/,/```/p' README.md \
    | grep -v 'wmbusmeters version' \
    | grep -v '```' >> ${OUTPUT}
echo ')MANUAL";' >> ${OUTPUT}
