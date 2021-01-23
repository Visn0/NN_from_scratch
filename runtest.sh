cd src
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
	else
		echo "ERROR"
	fi	
done
cd ..
make clean