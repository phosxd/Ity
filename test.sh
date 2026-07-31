RESET=$'\033[0m'
BOLD=$'\033[1m'
BLACK=$'\x1B[30m'
RED=$'\x1B[31m'
GREEN=$'\x1B[32m'
ORANGE=$'\x1B[33m'
CLEAR_LINE=$'\x1B[2K'
RESET_POS=$'\x1B[1G'


tests=(
# 1

"
# comment
###
# com;ment;

throw 'all good'; # inline comment!
;;
"

"Error: 0"

# 2

"
# Assignment
# ----------

var * none_ = 'a';
none_; none;
var * true_ = 'a';
true_; true;

var NONE a;
a = none;
a = none);
var BOOL b = true;
b = false;
b = false);

# Var syntax comprehension.
var c=1;
var d = 1;
var INT e=1;
var INT f =1;
"

""

# 3

"
var INT _c = 0;
_c = 00;
_c = 0_0;
_c = 0 0;
_c = 01;
_c = 00000000000000000001;
_c = -0;
_c = -1;
_c = -10 0_0;

var inferred = 0;
inferred = 100;
"

""

# 4

"
var FLOAT _d = 0.0;
_d = 0.00000000000001;
_d = 1.0;
_d = 100 000 000 . 500;
_d = (-0.0);
_d = -1.0;
_d = -0.001;
_d = -100 000 000 . 500;

var inferred = 0.0;
inferred = 100.125;
"

""

# 5

"
var STR _e = 'a';
_e = 'a';
_e = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa';
_e = '
	multi
	line
	string
';
_e = '\;';
_e = '\'escape\'';

var inferred = 'a';
inferred = 'b';
"

""

# 6

"
var ANY _j = none;
_j = true;
_j = false;
_j = 0;
_j = 1;
_j = 1.123;
_j = 'a';
_j = 'abc';
"

""

# 7

"
# Complex Data Assignment
# -----------------------

var ARR _a = [1,2,3];
_a *= 2;

var MAP _b = {'a',1, 'b',2, 'c',3};

merge IO;
print:[_a, '\n', _b];
"

"[1, 2, 3, 1, 2, 3]
{\"c\": 3, \"b\": 2, \"a\": 1}"

# 8

"
# Accessor Operator
# -----------------

merge IO;
print:[ ([10,20,30]:2) ];
print:[( {'a',[10,20,30]} : 'a' : 0 )];
"

"30
10"

# 9

"
# Mathematical Expression
# -----------------------

merge IO;

print:[ (0 + 0) ];
print:[ (1 + 1) ];
print:[ (1+1) ];
print:[ (1    +   1) ];
print:[ (1.0 + 1.0) ];

print:[ (0 - 0) ];
print:[ (1 - 1) ];

print:[ (2 * 4) ];
print:[ (2 * 2.5) ];

print:[ (8 / 2) ];
print:[ (8 / 1.5) ];

print:[ (10 % 4) ];
"

"0
2
2
2
2
0
0
8
5
4
5.333333333333333
2"

# 10

"
# String Operations
# -----------------

merge IO;

print:[ ('Hello' + ' World')];
print:[ ('Hello' + ' ' + 'World' + '!') ];
print:[ ('abc ' * 3) ];
"

"Hello World
Hello World!
abc abc abc "

# 11

"
# Comparison expression
# ---------------------

merge IO;

print:[ (true == true) ];
print:[ (true == false) ];
print:[ (true != true) ];
print:[];
print:[ (1 == 1) ];
print:[ (0 == 1) ];
print:[ (1.3 == 1.4) ];
print:[];
print:[ (2 > 1) ];
print:[ (2 < 1) ];
print:[ (2 >= 2) ];
print:[];
print:[ (2 >= 1) ];
print:[ (2 <= 2) ];
print:[ (2 <= 1) ];
print:[];
print:[ ('Hello' == 'World') ];
print:[ ('Hello' != 'World') ];
"

"true
false
false

true
false
false

true
false
true

true
true
false

false
true"

# 12

"
# Expression syntax
# -----------------

'early') + 'exit';

1 + (2);
1 + (2 ;
20 - (5+5);
20 + (-10);
'Hello' + ' '+ ( 'W'+'o' + ('r'+'d') );
"

""

# 13

"
merge IO;

# Conditional code block
# ----------------------

if true == true;
	print:['true is true'];
/; else; throw; /;
if true == false;
	throw 'true is false';
/;
"

"true is true"

# 14

"
merge IO;

# Nested conditionals

if true == true;
	if false == false;
		if 1 == 1;
			if 2 == 2;
				if 1 == 0; throw; /;
				elif 'A' != 'B';
					print:['passed'];
				/; else; throw; /;
			/; else; throw; /;
		/; else; throw; /;
	/; else; throw; /;
/;
"

"passed"

# 15

"
# Chaining conditionals

if true;
	'Yes';
/;
elif true;
	throw 'No';
/;
else;
	throw 'No';
/;

if false;
	throw 'No';
/;
elif true;
	'Yes';
/;
else;
	throw 'No';
/;

if false;
	throw 'No';
/;
elif false;
	throw 'No';
/;
else;
	'Yes';
/;
"

""

# 16

"
merge IO;

# Scoped conditional blocks
# -------------------------

if true;
	var INT x = 1;
	print:[x];
/;

x; # Should throw error
"

"1
Error: 27"

# 17

"
merge IO;

# While loop
# ----------

var INT i = 0;
while i < 10;
	i += 1;
/;

print:[i];


# Should not run
while false;
	throw 'That\'s not right.';
/;
"

"10"

# 18

"
merge IO;

# Nested while loop
# -----------------

var INT i = 0;
while i < 10;
	var INT j = 0;
	while j < 10;
		j += 1;
	/;
	i += 1;
/;

print:[i];
"

"10"

# 19

"
# Variable in while loop should be destroyed after each iteration
var INT i = 0;
while i < 4;
	var INT test = 512;
	i += 1;
/;

test; # Should throw an error
"

"Error: 27"

# 20

"
func NONE test;
	var INT j = 0; while j < 5;
		if j == 2; return; /;
		j += 1;
	/;
/;


var INT i = 0; while i < 5;
	test:[];
	i += 1;
/;
"

""

# 21

"
merge IO;

# Function definition
# -------------------

func NONE say_hi;
	print:['Hello there!'];
/;

say_hi:[];


# With arguments

func INT add; arg INT a; arg INT b;
	return a+b;
/;

print:[ (add:[4,5]) ];
"

"Hello there!
9"

# 22

"
merge IO;

# Function scope correctness.
# ---------------------------

# Should throw a shadowed name warning, because 'a' is defined in the same scope as the function declaration.
func NONE test; arg ANY a;
	print:[a];
/;


var INT a = 1;
test:[a];



# Does not throw a warning, because outer var 'b' is not in the same scope as the function declaration
func NONE test_2; arg ANY b;
	print:[b];
/;


if true;
	var INT b = 5;
	test_2:[b];
/;
"

"Warning: 25
1
5"

# 23

"
# Early exit conditional check
# ----------------------------

func BOOL test;
	throw 'This should not run';
/;

func BOOL test2;
	return true;
/;


merge IO;

print:[( (1 == 0) && (test:[]) )];
print:[( (1 == 1) && (test2:[]) )];
print:[( (1 == 0) || (test2:[]) )];
print:[( (1 == 1) || (test:[]) )];
"

"false
true
true
true"

# 24

"
merge IO;

# Type casting
# ------------

print:[( 'true' -> BOOL )];
print:[( 'false' -> BOOL )];
print:[( 'invalid representation' -> BOOL )];
print:[( '12.76' -> INT )];
print:[( '12.78' -> FLOAT )];
print:[( 'invalid representation' -> FLOAT )];
print:[( (100 -> STR) + '.' )];
print:[( (10.8760 -> STR) + '.' )];
"

"true
false
false
12
12.78
0
100.
10.876."

# 25

"
merge IO;

# Early exit
# ----------

var INT i = 0; while i < 10;
	if i == 4;
		exit;
	/;
	out:[i];
	i += 1;
/;
"

"0123"

# 26

"
merge IO;

# Array/Map property assignment
# -----------------------------


# Array...
var ARR a = [1,2,3,4];
print:[a];

a:1 *= 10;
print:[a];


print:[];


# Map...
var MAP b = {'a',1, 'b',2, 'c',3};
print:[b];

b.a = 'hello';
print:[b];
"

"[1, 2, 3, 4]
[1, 20, 3, 4]

{\"c\": 3, \"b\": 2, \"a\": 1}
{\"c\": 3, \"b\": 2, \"a\": \"hello\"}"

# 27

"
merge IO;

# Nested Array/Map property assignment
# ------------------------------------


# Array...
var ARR a = [1,2,3,[4,5,6]];
print:[a];

a:3:1 = 500;
print:[a];


print:[];


# Map...
var MAP b = {'a',1, 'b',2, 'c',{'key','value'}};
print:[b];

b.c.key = [1,2,3];
print:[b];
"

"[1, 2, 3, [4, 5, 6]]
[1, 2, 3, [4, 500, 6]]

{\"c\": {\"key\": \"value\"}, \"b\": 2, \"a\": 1}
{\"c\": {\"key\": [1, 2, 3]}, \"b\": 2, \"a\": 1}"

# 28

"
# Loop continue / break
# ---------------------

merge IO;


var INT i = 0; while true; i+=1;
	print:[i];
	if i == 5; break; /;

	continue;
	throw 'This should be skipped';
/;

print:['Done'];
"

"1
2
3
4
5
Done"

# 29

"
# Nested loop continue
# --------------------

merge IO;

var INT total = 0;
var INT i = 0; while i < 5; i+=1;
	var INT j = 0; while j < 10; j+=1;
		if j%2 == 0; continue; /; # Skip every other number, meaning we skip half of all iterations.
		total += 1;
	/;
	print:[i];
/;

print:[total];
"

"1
2
3
4
5
25"

# 30

"
# System command
# --------------

var INT code = system:['echo hello'];
if code != 0; throw 'Wrong code'; /;
"

"hello"

# 31

"
# Complex data addition
# ---------------------

merge IO;

var ARR arr = [1,2,3];
arr += [4,5,6];
print:[arr];

var MAP map = {'a',1};
map += {'b',2};
print:[map];
"

"[1, 2, 3, 4, 5, 6]
{\"b\": 2, \"a\": 1}"

# 32

"
# Array/Map item erase
# --------------------

merge IO;

var ARR arr = [1,2,3];
arr.erase:[0];
print:[arr];

var MAP map = {'a',1, 'b',2};
map.erase:['a'];
print:[map];
"

"[2, 3]
{\"b\": 2}"

# 33

"
# Iterate on map keys
# -------------------

merge IO;

var MAP map = {'a',1, 'b',2, 'c',3};
const ARR keys = map.keys:[];

var INT i = 0; while i < (length:[keys]);
	print:[(map:(keys:i))];
	i+=1;
/;
"

"3
2
1"

# 34

"
# Add raw character to string
# ---------------------------

merge IO;

var STR string = 'ab';
string += 99; # ASCII character for 'c'.

print:[string];
print:[( 'a'.raw:[] )]; # Print the integer representation of 'a'.
"

"abc
97"

# 35

"
# Loop over iterable
# ------------------

merge IO;

for i in [1,2,3];
	print:[i];
/;


for i in 'abc';
	print:[i];
/;


# Shadowed var name.
var INT i = 0;
for i in 3;
	print:[(i+4)];
/;
"

"1
2
3
a
b
c
Warning: 25
4
5
6"

# 36

"
# Complex nested loops
# --------------------

merge IO;

for i in 2;
	for i in 2;
		var INT i2 = 0; while i2 < 2;
			for i3 in 2;
				print:[(i+i2+i3)];
			/;
			i2 += 1;
		/;
	/;
/;
"

"Warning: 25
0
1
1
2
1
2
2
3
Warning: 25
0
1
1
2
1
2
2
3"

# 37

"
{'a': 1, 'b': 2}; # Invalid map declaration syntax.
"

"Error: 21"

# 38

"
{'a', 1+1, 'b', 2}; # Invalid map item at 'a'.
"

"Error: 21"

# 39

"
[1+1, 2, 3]; # Invalid array item at index 0.
"

"Error: 21"

# 40

"
# Reference comprehension in arrays
# ---------------------------------

merge IO;

var a = [1,[2,3]];
print:[a];
a:1:0 = 99;
print:[a];

var ARR b = [];
b <<= a; # Move a into b.
print:[a, ' ', b];

var ARR c = [];
c <<= (b:1); # Move nested array into c.
print:[b, ' ', c];

"

"[1, [2, 3]]
[1, [99, 3]]
[] [1, [99, 3]]
[1, []] [99, 3]"

# 41

"
# Reference comprehension in maps
# -------------------------------

merge IO;

var a = {'a',1 ,'b',[1,2]};
print:[a];
a.b:0 = 99;
print:[a];

var MAP b = {};
b <<= a; # Move a into b.
print:[a, ' ', b];

var ARR c = [];
c <<= (b.b); # Move nested array into c.
print:[b, ' ', c];

"

"{\"b\": [1, 2], \"a\": 1}
{\"b\": [99, 2], \"a\": 1}
{} {\"b\": [99, 2], \"a\": 1}
{\"b\": [], \"a\": 1} [99, 2]"

# 42

"
# Complex temporary integrity
# ---------------------------

merge IO;

func INT f;
	const val = 10;
	while true;
		return val; # Return variable value from an early exit.
	/;
/;


func NONE f2;
	var res = f:[];
	print:[res];
/;


f2:[];
"

"10"

# 43

"
func INT f2;
	for i in 2;
		return rand:[0,100];
	/;
/;

func NONE f;
	var n = f2:[];
	for i in range:[n];
		return;
	/;
	1+1;
/;

for i in 3;
	f:[];
/;
"

""

# 44

"
# Passing / modifying pointer values
# -----------------------------------

merge IO;

func NONE do_modification; arg PTR obj;
	if type_name:[~obj] != ARR;
		throw 'Invalid pointer value, not an array.';
	/;
	obj += [3]; # Add an item to the referenced array.
/;


var ARR my_arr = [1,2];
do_modification:[@my_arr];
print:[my_arr];
"

"[1, 2, 3]"

# 45

"
merge IO;

# Declare variables we can point to.
var a = 0;
var b = 0;

# Assign pointers.
const a_ptr = @a;
const b_ptr = @b;

# Set value.
a = 10;
b = 5;

# Use pointers just like it were the real value.
print:[( a_ptr + b_ptr )];

# Setting the value on a pointer sets the referenced variant, doesn't overwrite the pointer.
a_ptr = 99;
print:[a];

var copy_of_a = ~a_ptr;
copy_of_a = 0; # 'a' & 'a_ptr' unchanged.
print:[copy_of_a];
print:[a];

"

"15
99
0
99"

# 46

"
# Reassign pointer
# ----------------

merge IO;

const a = 10;
const b = 20;

var ptr = @a;
print:[ptr];
ptr.reassign:[@b];
print:[ptr];
"

"10
20"

# 47

"
# Fail reassign const pointer
# ---------------------------

var a = 1;
var b = 2;
const ptr = @a;
ptr.reassign:[@b]; # Should throw const value error.
"

"Error: 29"
)


starting_idx=0
if [[ "$1" != "" ]]; then
	starting_idx=$1
fi

# Iterate on each test.
for (( idx=0; idx<${#tests[@]}; idx+=2 )); do
	i="${tests[$idx]}" # Get the test code.
	case_num=$(( (idx/2)+1 ))
	if (( $case_num < $starting_idx )); then
		continue
	fi

	# Print test case number.
	printf "${CLEAR_LINE}${RESET_POS}[Case ${case_num}] "

	echo "$i" > .test.ity # Put test code in a file.
	./ity.bin -codes .test.ity > .test_result.txt # Run test code, then output the result to a file.
	code=$?

	expected=${tests[(($idx+1))]} # Get expected test output.
	result=$(cat .test_result.txt) # Get test results.
	# If results do not match up, test failed...
	if [[ "${result}" != "${expected}" || ("$code" != "0" && "$code" != "1") ]]; then
		echo
		echo "${BOLD}Test case source code:${RESET}"
		echo "${i}"
		echo "${BOLD}Test case debug results:${RESET}"
		echo
		echo "$(./ity.bin -d-full .test.ity)"
		echo
		echo "- - - - - - - - - - - - - - - - - - - -"
		echo
		echo "${RED}Case ${case_num} failed!${RESET}"
		echo
		echo "${ORANGE}EXPECTED:${RESET}"
		echo "${expected}" | sed "s/^/  /"
		echo "."
		echo "${ORANGE}RECEIVED:${RESET}"
		echo "${result}" | sed "s/^/  /"
		echo "."
		echo
		exit
	fi
done


# Remove testing files.
rm .test.ity
rm .test_result.txt


echo "${CLEAR_LINE}${RESET_POS}${BOLD}${GREEN}All tests passed!${RESET}"
