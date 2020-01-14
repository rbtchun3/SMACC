#!/bin/bash

GITHUB_USER=reelrobotics

#---- TEST GHPAGES LOCALLY VARIABLES------
# uncomment this for local testing and comment the TRAVIS BLOCK
GITHUB_USER=pabloinigoblasco
TRAVIS_BRANCH=master
TRAVIS_REPO_SLUG=smacc
GITHUB_TOKEN=8f612550ed359620ea014df5bbb0b180b15fc8d6
CATKIN_WORKSPACE_ROOT=`pwd`/../..
# ----------- TRAVIS --------------------------
# industrial_ci catkin workspace
#CATKIN_WORKSPACE_ROOT=/root/target_ws 
#---------------------------------------
#REPO_URL=github.com/reelrbtx/smacc
DOC_REPO_URL=github.com/reelrbtx/SMACC_Documentation.git

DIRECTORY=$(cd `dirname $0` && pwd)
echo $DIRECTORY

echo "GH-PAGES"
if [ -n "$GITHUB_TOKEN" ]; then
    echo "GH - PAGES script working directory: $TRAVIS_BUILD_DIR"
    cd "$TRAVIS_BUILD_DIR"

    #find / | grep SMACC
    
    #apt-get install -y git

    source $CATKIN_WORKSPACE_ROOT/install/setup.bash
    #source /root/catkin_ws/install/setup.bash

    #remove cloned folder just in case it already existed
    rm -R /tmp/doc

    # This generates a `web` directory containing the website.
    echo "cloning gh-pages"
    git clone https://pabloinigoblasco:$GITHUB_TOKEN@$REPO_URL --branch gh-pages /tmp/doc

    echo "removing specific branch folder from repo clone.."
    cd /tmp/doc
    rm -r $TRAVIS_BRANCH >/dev/null
    mkdir -p $TRAVIS_BRANCH

    echo "cd $CATKIN_WORKSPACE_ROOT"
    ls $CATKIN_WORKSPACE_ROOT/src
    cd $CATKIN_WORKSPACE_ROOT/src/SMACC
    ls
    
    echo "executing doxygen command"
    doxygen Doxyfile

    echo "moving result files to branch directory..."
    ls /tmp
    mv /tmp/html /tmp/doc/$TRAVIS_BRANCH
    mv /tmp/latex /tmp/doc/$TRAVIS_BRANCH

    #git init
    #git checkout -b gh-pages
    echo "cd /tmp/doc/$TRAVIS_BRANCH -> directories:"
    ls /tmp/doc
    cd /tmp/doc/$TRAVIS_BRANCH

    git add .
    #git add -f /tmp/doc/index.html
    #echo "------LIST OF FILES ------"
    #find . 
    echo "commiting new documentation"
    
    echo `which git`
    echo $(git -c user.name='travis' -c user.email='travis' commit -q -m "gh-pages travis")
    git -c user.name='travis' -c user.email='travis' commit -q -m "gh-pages travis"

    #git diff origin/master..HEAD
    echo "pushing new documentation"
    echo "GITHUB USER: $GITHUB_USER"
    
    # Make sure to make the output quiet, or else the API token will leak!
    # This works because the API key can replace your password.
    git push -f -v https://$GITHUB_USER:$GITHUB_TOKEN@$REPO_URL gh-pages > pushout.txt
    cat pushout.txt

    # going back to travis build dir
    cd "$TRAVIS_BUILD_DIR"
fi
