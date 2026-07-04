import sys
import ExpressionParser
import ScopeState
import ScriptErrors
import Instructions.var as INST_var
import Instructions.set as INST_set

ALPHANUM = 'AaBbCcDdEeFfGgHhIiJjKkLlMmOoPpQqRrSsTtUuVvWwXxYyZz0123456789'.split()
INSTRUCTIONS = {
	'merge':None,
	'import':None,

	'const': INST_var,
	'var': INST_var,
	'set': INST_set,

	'if':None,
	'elif':None,
	'else':None,

	'for':None,
	'while':None,
	'continue':None,
	'break':None,

	'func':None,
	'return':None,
}



def _new_item(ln:int=1, col:int=1):
	return {
		'ln': ln,
		'col': col,
		'args': [],
	}


def tokenize(script:str):
	sequence = []
	buffer = ''
	current_line = 1
	current_column = 0
	item = None

	is_start = True
	is_comment = False

	is_string = False
	string_type = ''
	for char in script:
		current_column += 1
		if char == '\n':
			current_line += 1
			current_column = 0

		# Skip over spaces & tabs at the start of the item.
		if is_start:
			if char == ' ' or char == '\n' or char == '\t': continue
			item = _new_item(ln=current_line, col=current_column)
			is_start = False

		# End comment.
		if is_comment:
			if char == ';': is_comment = False
			continue

		# End string.
		if is_string:
			if char == string_type:
				is_string = False
				buffer += char
				continue

		else:
			# Start comment.
			if char == '#' and len(item['args']) == 0:
				is_comment = True
				continue

			# Start string.
			if (char == '\'' or char == '"'):
				is_string = True
				string_type = char
				buffer += char
				continue

			# Add argument to args.
			if char == ' ' or char == '\n':
				if len(buffer) > 0:
					item['args'].append(buffer)
					buffer = ''
				continue

			# If is instruction end, append item to sequence & reset state.
			if char == ';':
				# Append remaining argument to args.
				if len(buffer) > 0:
					item['args'].append(buffer)
					buffer = ''
				sequence.append(item)
				is_start = True
				continue

		# Add char to buffer.
		buffer += char

	return {
		'sequence': sequence,
	}



def exec(sequence):
	state = ScopeState.create_new()
	for item in sequence:
		ScriptErrors.current_line = item['ln']
		ScriptErrors.current_column = item['col']
		args = item['args']
		arg_count = len(args)
		if arg_count == 0: continue

		# Execute instruction.
		inst = INSTRUCTIONS.get(args[0])
		if inst != None:
			if arg_count < inst.REQUIRED:
				ScriptErrors.error(f'Invalid number of arguments for instruction "{args[0]}". Expected at least {inst.REQUIRED} separated by a space.')
				return
			inst.exec(state, args)

		# If not matched any instruction, run as expression.
		else:
			ExpressionParser.exec(state, ' '.join(args))

	return state



f = open(sys.argv[1], 'r')
file_text = f.read()
f.close()

result = tokenize(file_text)
state = exec(result['sequence'])
print(state)

