#!/bin/bash

if [ "$#" -ne 2 ]
then
  echo "Wrong number of arguments. Should be 2, was $#";
  exit 1;
fi

git checkout $1
git merge --ff-only $2