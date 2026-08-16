install_path="/usr/local/bin/ity"
share_install_path='/usr/local/share/ity'

echo $'\033[1mInstalling production build to "'$install_path$'"...\033[0m'
sudo rm $install_path
sudo cp ity.bin $install_path

echo $'\033[1mInstalling modules to "'$share_install_path$'"...\033[0m'
if [ -d "${share_install_path}" ]; then
	sudo rm -rf "${share_install_path}/Modules"
else
	sudo mkdir "${share_install_path}"
fi
sudo cp -r "Scripts/Modules" "${share_install_path}/Modules"

echo "Done. You should be able to run it anywhere on your system! Try running the command \"ity\"!"
