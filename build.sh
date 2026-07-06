do_tests=$1




echo "Building..."

start=$(date +%s)

cd src
g++ -flto=4 -O3 -fno-exceptions -fno-rtti Main.cpp -o Ity.bin
strip Ity.bin
cd ../
mv src/Ity.bin Ity.bin

end=$(date +%s)
echo "Done in" $((end-start))"s."
echo "Final size: $(wc -c < Ity.bin) bytes."




if [[ "$do_tests" != "no" ]]; then
	./test.sh
fi
