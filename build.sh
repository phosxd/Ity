RESET=$'\033[0m'
BOLD=$'\033[1m'
BLACK=$'\x1B[30m'
RED=$'\x1B[31m'
GREEN=$'\x1B[32m'
ORANGE=$'\x1B[33m'

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
	g++ -flto=4 -O3 -fno-exceptions -fno-rtti Main.cpp -o Ity.bin
	strip Ity.bin
fi
if [[ $DEBUG == 1 ]]; then
	g++ -g -flto=4 -O3 -fno-exceptions -fno-rtti Main.cpp -o Ity.bin
fi

cd ../
mv src/Ity.bin Ity.bin

end=$(date +%s)
echo "Done in" $((end-start))"s."
echo "Final size: ${ORANGE}$(wc -c < Ity.bin)${RESET} bytes."




if [[ $DO_TESTS == 1 ]]; then
	echo
	echo "${BOLD}Running tests...${RESET}"
	./test.sh
fi
