SRC=src

for file in $(ls $SRC/*.cpp)
do    
    make clean
    echo -e "##### TEST: $file #####"
    cp $file $SRC/main.cpp
    
    make    

    ./main > $file.sol
done

make clean
rm -rf $SRC/main.cpp