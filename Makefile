IPL.tmp : ./src/IPL/IPL.s
	nasm ./src/IPL/IPL.s -o IPL.tmp

os.img : IPL.tmp
	cp IPL.tmp os.img
	dd if=/dev/zero of=os.img bs=512 seek=1 count=2879

goo.img : os.img black.tmp
	rm ./tmp -rf
	mkdir ./tmp
	sudo mount -o loop os.img ./tmp
	sudo cp black.tmp ./tmp/black.sys
	sudo umount ./tmp
	mv os.img goo.img

black.tmp : ./src/app/black.s
	nasm ./src/app/black.s -o black.tmp

clean : 
	find . -name "*.tmp"  | xargs rm -f
	find . -name "*.img"  | xargs rm -f
	rm ./tmp -rf
