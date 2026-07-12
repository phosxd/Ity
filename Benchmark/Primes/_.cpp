#include <iostream>
#include <string>
#include <chrono>
#include <cmath>

using namespace std;


bool is_prime(int n) {
	if(n < 2) {return false;}
	if(n%2 == 0) {return n == 2;}
	int r = std::sqrt(n);
	for(int i = 3; i <= r; i += 2) {
		if(n%i == 0) {return false;}
	}
	return true;
}


int main() {
	std::cout << "Count: ";
	std::string input;
	std::getline(std::cin, input);
	int n = std::stoi(input);

	auto start = std::chrono::high_resolution_clock::now();

	for(int p = 2; p <= n; p++) {
		if(is_prime(p)) {
			std::cout << p << "\n";
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);

	std::cout << "\nDone in " << static_cast<float>(duration.count())/1000.0 << "s\n";

	return 0;
}
