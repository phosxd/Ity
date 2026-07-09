RESET=$'\033[0m'
BOLD=$'\033[1m'
BLACK=$'\x1B[30m'
RED=$'\x1B[31m'
GREEN=$'\x1B[32m'
ORANGE=$'\x1B[33m'

BIN_SIZE_LIMIT=100000
BUILD_ARGS="-flto=4 -O3 -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--gc-sections Main.cpp -o Ity.bin"

DO_TEST=0
DEBUG=0


for i in "$@"; do
	case $i in
		-t|--test)
			DO_TESTS=1
		;;
		-d|--debug)
			DEBUG=1
		;;
		*)
			echo "${RED}Unknown option \"$i\"."
			exit 1
		;;
	esac
done


# Run build process...

if [[ $DEBUG == 1 ]]; then
	echo "${BOLD}Building debug binary...${RESET}"
fi
if [[ $DEBUG == 0 ]]; then
	echo "${BOLD}Building prodution binary...${RESET}"
fi

start=$(date +%s)
cd src

result=0
if [[ $DEBUG == 0 ]]; then
	g++ $BUILD_ARGS
	result=$?
	strip Ity.bin # Better results than "-s" flag for gcc.
fi
if [[ $DEBUG == 1 ]]; then
	g++ -g $BUILD_ARGS
	result=$?
fi

if (( result != 0 )); then
	echo "${RED}Build failed... Resolve errors then try again.${RESET}"
	exit
fi

cd ../
mv src/Ity.bin Ity.bin




# Print results...

end=$(date +%s)
echo "Done in" $((end-start))"s."

# Get size difference between this & the last build.
bin_size=$(wc -c < Ity.bin)
prev_bin_size=0
if [[ -e ".last_build_size" ]]; then
	prev_bin_size=$(cat .last_build_size)
fi
diff=$((bin_size-prev_bin_size))
diff_text="${RED}+${diff}${RESET}"
if (( $diff < 0 )); then
	diff_text="${GREEN}${diff}${RESET}"
fi
if (( $diff == 0 )); then
	diff_text="${GREEN}+${diff}${RESET}"
fi
# Save size of current build for next diff.
echo "$bin_size" > .last_build_size

# Print results...
echo "Final size: ${ORANGE}${bin_size}${RESET} bytes. (${diff_text})"
if (( $bin_size > $BIN_SIZE_LIMIT )); then
	echo "${RED}Binary size is over the goal of \"${BIN_SIZE_LIMIT}\"."
fi




# Run tests...

if [[ $DO_TESTS == 1 ]]; then
	echo
	echo "${BOLD}Running tests...${RESET}"
	./test.sh
fi
