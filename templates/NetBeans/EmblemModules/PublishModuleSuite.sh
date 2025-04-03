#! /bin/bash

PUBLISH_LOCATION=$1
PUBLISH_LOCATION=${PUBLISH_LOCATION:="/mnt/public_on_phabricator/Net/nb/"}
if [ -e $PUBLISH_LOCATION ] ; then
    echo "Publishing to $PUBLISH_LOCATION"
    mkdir -p $PUBLISH_LOCATION
    cp -R build/updates/* $PUBLISH_LOCATION
else
    echo "$PUBLISH_LOCATION not found. Please specify the destination of the nb repository you want to publish to. (local directory/mount)"
fi

