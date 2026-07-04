echo "Building..."

start=$(date +%s)

g++ -flto=4 -O3 Main.cpp -o Ity.bin

end=$(date +%s)
echo "Done in" $((end-start)) "s."
