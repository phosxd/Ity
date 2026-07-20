install_path="/usr/local/bin/ity"

bash build.sh
code=$?
if [[ "${code}" != "0" ]]; then
	exit
fi

echo
echo $'\033[1mInstalling production build to "'$install_path$'"...\033[0m'
sudo rm $install_path
sudo cp Ity.bin $install_path
echo "Done. You should be able to run it anywhere on your system! Try running the command \"ity\"!"
