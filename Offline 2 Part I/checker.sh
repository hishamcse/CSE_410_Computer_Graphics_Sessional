for i in $(ls -d "tests"/*/); do
    cd $i
    cat scene.txt > ../../scene.txt
    cd ../../
    ./script.sh
    diff -w stage1.txt $i/stage1.txt
    printf "\nOK for $i/stage1.txt\n"
    diff -w stage2.txt $i/stage2.txt
    printf "\nOK for $i/stage2.txt\n"
    diff -w stage3.txt $i/stage3.txt
    printf "\nOK for $i/stage3.txt\n"
done