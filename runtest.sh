SRC=src
EXPECTED=$SRC/main.cpp.out
RESULT=$SRC/main.cpp.res

for file in $(ls $SRC/*.cpp)
do    
    make clean
    echo -e "##### TEST: $file #####"
    cp $file $SRC/main.cpp
    cp $file.out $EXPECTED
    
    make    

    ./main > $RESULT
    diffresult=$(colordiff -Z -s $EXPECTED $RESULT)

    if [ -z "$diffresult" ]; 
    then 
        echo -e "\tOK\n"; 
    else 
        echo -e "\tERROR: $file \n $diffresult \n"; 
    fi
done

make clean
rm -rf $SRC/main.cpp