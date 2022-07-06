#git pull for all subdirectories
#https://stackoverflow.com/questions/3497123/run-git-pull-over-all-subdirectories/40601481
find . -type d -maxdepth 1 -print -exec git --git-dir={}/.git --work-tree=$PWD/{} pull origin master \;


