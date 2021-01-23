cd src
ok_test=0
fail_test=0
for FILE in $(ls *.cpp) ; do
	echo "Evaluando $FILE"
	cp $FILE main.cpp
	cd ..
	make
	cd src
	../main > $FILE.out
	colordiff -b -B -i $FILE.out $FILE.sol
	if [ "$?" == "0" ]; then
		echo "OK"
		ok_test=$((ok_test+1))
	else
		echo "ERROR"
		fail_test=$((fail_test+1))
	fi		
done
echo "OK: $ok_test FAIL: $fail_test" 

cd ..
make clean