#!/bin/sh

set -e

if [ -f .git/hooks/pre-commit.sample ] && [ ! -f .git/hooks/pre-commit ]; then
    # This part is allowed to fail
    cp -p .git/hooks/pre-commit.sample .git/hooks/pre-commit && \
        chmod +x .git/hooks/pre-commit && \
        echo "Activated pre-commit hook." || :
fi

# README and INSTALL are required by automake, but may be deleted by
# clean up rules. to get automake to work, simply touch these here,
# they will be regenerated from their corresponding *.in files by
# ./configure anyway.
touch README INSTALL

# Make sure m4 directory exist
mkdir -p m4

autoreconf --force --install --verbose || exit $?
