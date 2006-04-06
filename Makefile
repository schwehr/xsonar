all:
	cd usgsmap && make
	cd xsonar && make byteOrder
	cd xsonar && make xsonar
	cd showimage && make

optimize:
	cd usgsmap && make clean OPTIMIZE=1
	cd usgsmap && make
	cd xsonar && make clean
	cd xsonar && make byteOrder
	cd xsonar && make xsonar OPTIMIZE=1
	cd showimage && make OPTIMIZE=1

clean:
	-rm -f */*.o */*.a */{showimage,xsonar}
# 	-cd usgsmap && make clean
# 	-cd xsonar && make clean
# 	-cd showimage && make clean

VERSION:=0.7
TMPDIR:=xsonar-${VERSION}

.PHONY: release
release: clean
	rm -rf ${TMPDIR} ${TMPDIR}.tar.bz2
	mkdir ${TMPDIR}
	cp -rp Makefile README.txt showimage usgsmap xsonar ${TMPDIR}
	tar --exclude=.svn -cf ${TMPDIR}.tar ${TMPDIR}
	bzip2 -9 ${TMPDIR}.tar

xsonar.info: xsonar.info.in ${TMPDIR}.tar.bz2
	perl -p -e "s/\@VERSION\@/${VERSION}/g" $< > $@
	MD5=`md5 ${TMPDIR}.tar.bz2| awk '{print $$4}'` && perl -pi -e "s/\@MD5\@/$$MD5/g" $@

release-web:
	scp ${TMPDIR}.tar.bz2 xsonar.info schwehr.org:www/software/xsonar/

release-local:
	cp xsonar.info /sw/fink/10.4-transitional/local/main/finkinfo/sci/