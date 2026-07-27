#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

using namespace std;


std::string operator*(const std::string& a, const int& b) {
	std::string sum; sum.reserve(a.size()*b);
	for (int i = 0; i < b; i++) sum += a;
	return sum;
}


int main() {
	auto start = std::chrono::high_resolution_clock::now();

	std::ifstream f ("Benchmark/Allocation/Blob.txt", std::ios::in | std::ios::binary);
	const std::string& blob = (std::ostringstream() << f.rdbuf()).str() * 100000;
	f.close();
	std::cout << blob.size() << '\n';

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
	std::cout << "\nDone in " << static_cast<float>(duration.count())/1000.0 << "s\n";

	return 0;
}
