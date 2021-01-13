archive=~/CSE130-Lab1.tar
echo "Creating $archive.gz"
rm $archive* 2>/dev/null
cd ../../..
BASE=`pwd`

tar cvf $archive \
	`diff -u -r pintos .pintos | lsdiff | sed 's/$BASE//' | sed '/\/\.vscode/d'` 2>/dev/null

tar rvf $archive \
	`diff -r .pintos pintos | grep Only | sed 's/: /\//' | sed '/\/\.pintos/d' | sed 's/Only in //' | sed 's/$BASE//' | sed '/^>/d' | sed '/^</d' | sed '/\/\.vscode/d'`

gzip $archive
