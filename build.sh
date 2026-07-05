do_tests=$1




echo "Building..."

start=$(date +%s)

cd src
g++ -flto=4 -O3 Main.cpp -o Ity.bin
cd ../
mv src/Ity.bin Ity.bin

end=$(date +%s)
echo "Done in" $((end-start))"s."




if [[ "$do_tests" != "no" ]]; then
	./test.sh
fi
