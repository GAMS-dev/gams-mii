#!/bin/bash -x

set +e

## get MII version information for artifact and tag name
MII_MAJOR_VERSION=$(grep ^MI_MAJOR_VERSION version | cut -f2 -d"=")
MII_MINOR_VERSION=$(grep ^MI_MINOR_VERSION version | cut -f2 -d"=")
MII_PATCH_LEVEL=$(grep ^MI_PATCH_LEVEL version | cut -f2 -d"=")

## change to artifact location and name
cd artifacts
mv $(find *arm_64.dmg) "GAMS_MII-$MII_MAJOR_VERSION.$MII_MINOR_VERSION.$MII_PATCH_LEVEL-arm_64.dmg"
mv $(find *x86_64.dmg) "GAMS_MII-$MII_MAJOR_VERSION.$MII_MINOR_VERSION.$MII_PATCH_LEVEL-x86_64.dmg"
mv $(find *AppImage) "GAMS_MII-$MII_MAJOR_VERSION.$MII_MINOR_VERSION.$MII_PATCH_LEVEL-x86_64.AppImage"
mv $(find *zip) "GAMS_MII-$MII_MAJOR_VERSION.$MII_MINOR_VERSION.$MII_PATCH_LEVEL-x86_64.zip"

## folder name which matches the current tag, like v0.10.2 or v0.11.0-rc
FOLDER_NAME="${TAG_NAME}"

## create GAMS MII version directory
mkdir -p $FOLDER_NAME

## move artifacts to correct location
mv *.dmg *.AppImage *.zip $FOLDER_NAME

## URL to S3 MII
URL=${S3_URL}/mii/

## S3 upload to gams.com
s3cmd sync --acl-public ./ $URL --access_key=${S3_ACCESS_KEY} --secret_key=${S3_SECRET_KEY}

## S3 content listing
s3cmd ls -r $URL --access_key=${S3_ACCESS_KEY} --secret_key=${S3_SECRET_KEY}
