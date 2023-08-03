for i in $(ls -d "tests"/*/); do
    cd $i
    cat scene.txt > ../../scene.txt
    cat config.txt > ../../config.txt
    cd ../../
    ./script.sh
    diff -w stage1.txt $i/stage1.txt
    printf "\nOK for $i/stage1.txt\n"
    diff -w stage2.txt $i/stage2.txt
    printf "\nOK for $i/stage2.txt\n"
    diff -w stage3.txt $i/stage3.txt
    printf "\nOK for $i/stage3.txt\n"
    diff -w z_buffer.txt $i/z_buffer.txt
    printf "\nOK for $i/z_buffer.txt\n"
    img=$(echo "$i" | cut -d "/" -f 2)
    mv out.bmp "results_bmp/out_$img.bmp"
    printf "\nOK for out_$img.bmp\n"
    rm stage1.txt stage2.txt stage3.txt z_buffer.txt
done