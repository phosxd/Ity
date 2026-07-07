RESET=$'\033[0m'
BOLD=$'\033[1m'
BLACK=$'\x1B[30m'
RED=$'\x1B[31m'
GREEN=$'\x1B[32m'
ORANGE=$'\x1B[33m'

BIN_SIZE_LIMIT=101256
BUILD_ARGS="-flto=4 -O3 -fno-exceptions -fno-rtti -Wl,--gc-sections Main.cpp -o Ity.bin"

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


clear


if [[ $DEBUG == 1 ]]; then
	echo "${BOLD}Building debug binary...${RESET}"
fi
if [[ $DEBUG == 0 ]]; then
	echo "${BOLD}Building prodution binary...${RESET}"
fi

start=$(date +%s)
cd src

if [[ $DEBUG == 0 ]]; then
	g++ $BUILD_ARGS
	strip Ity.bin
fi
if [[ $DEBUG == 1 ]]; then
	g++ -g $BUILD_ARGS
fi

cd ../
mv src/Ity.bin Ity.bin

end=$(date +%s)
bin_size=$(wc -c < Ity.bin)
echo "Done in" $((end-start))"s."
echo "Final size: ${ORANGE}${bin_size}${RESET} bytes."
if [[ $bin_size > $BIN_SIZE_LIMIT ]]; then
	echo "${RED}Binary size is over the goal of \"${BIN_SIZE_LIMIT}\"."
fi




if [[ $DO_TESTS == 1 ]]; then
	echo
	echo "${BOLD}Running tests...${RESET}"
	./test.sh
fi
