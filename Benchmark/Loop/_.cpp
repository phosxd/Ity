#include <iostream>
#include <string>
#include <chrono>

using namespace std;


int main() {
	std::cout << "Count: ";
	std::string input;
	std::getline(std::cin, input);
	int n = std::stoi(input);

	auto start = std::chrono::high_resolution_clock::now();

	for(int i = 0; i <= n; i++) {
		std::cout << i << "\n";
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);

	std::cout << "Done in " << static_cast<float>(duration.count())/1000.0 << "s\n";

	return 0;
}
