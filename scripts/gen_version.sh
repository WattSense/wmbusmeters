#!/bin/sh

if [ -z $1 ]; then
    OUTPUT="/dev/stdout"
else
    OUTPUT=$1
fi

COMMIT_HASH=$(git log --pretty=format:'%H' -n 1)
TAG=$(git describe --tags)
BRANCH=$(git rev-parse --abbrev-ref HEAD)
TAG_COMMIT_HASH=$(git show-ref --tags | grep ${TAG} | cut -f 1 -d ' ')

if [ "${BRANCH}" = "master" ]; then
  BRANCH=
else
  BRANCH="${BRANCH}_"
fi

VERSION="${BRANCH}${TAG}"
DEBVERSION="${BRANCH}${TAG}"

git status -s | grep -v -q '?? '
if [ $? -eq 0 ]; then
  # There are changes, signify that with a +changes
  VERSION="${VERSION} with local changes"
  COMMIT_HASH="${COMMIT_HASH} with local changes"
  DEBVERSION="${DEBVERSION}l"
fi

echo "#define VERSION \"${VERSION}\"" > ${OUTPUT}
echo "#define COMMIT \"${COMMIT_HASH}\"" >> ${OUTPUT}
