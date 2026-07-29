function iSqrt(n) {
	let x = n;
	let y = (x+1) / 2;

	while (y < x) {
		x = y;
		y = ( ((n/x) + x) / 2);
	}
	return Math.trunc(x);
}


function isPrime(n) {
	if (n%2 === 0) return n === 2;

	const r = iSqrt(n);
	for (let i = 3; i<=r; i+=2) {
		if (n%i === 0) return false;
	}
	return true;
}


const prompt = require("prompt-sync")({ sigint: true });
const n = Number(prompt("Count: "));
const start = Date.now();


for (let p = 2; p <= n; p++) {
	if (isPrime(p)) console.log(p);
}


console.log(`\nDone in ${(Date.now()-start) / 1000.0}s.`);
