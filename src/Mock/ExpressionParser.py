import ScriptErrors
import ScopeState
import Operators.add as OP_add
import Operators.subtract as OP_sub

ALPHA = list('AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz')
NUM = list('0123456789')
OPERATORS = {
	'+': OP_add,
	'-': OP_sub,
	'--': OP_sub,
	'*': None,
	'/': None,
	'%': None,
	':': None,

	'==': None,
	'!=': None,
	'>': None,
	'>=': None,
	'<': None,
	'<=': None,
}

expr_cache = {}


def get_type(v):
	type_ = type(v)
	if type_ is bool: return 'BOOL'
	elif type_ is int: return 'INT'
	elif type_ is float: return 'FLOAT'
	elif type_ is str: return 'STR'


def is_valid_name(name:str):
	for char in name:
		if (char in ALPHA) == False and char != '_': return False
	return True


def is_special_symbol(char:str):
	return (
		char not in ALPHA
		and char not in NUM
		and char not in ['_','.','\'','"']
	)


def _check_ahead(text:str, start_idx:int, substr:str):
	substr_length = len(substr)
	if len(text) > start_idx+len(substr): return False
	i = 0
	while i < substr_length:
		if text[start_idx+i] != substr[i]: return False
		i += 1
	return True


def _new_item(t=None, d=None):
	return {
		't': t,
		'd': d,
	}


def _get_literal(type:str, str_val:str):
	if type in ['REF','STR']:
		return str_val
	elif type == 'BOOL':
		return str_val == 'true'
	elif type == 'INT':
		return int(str_val)
	elif type == 'FLOAT':
		return float(str_val)


def tokenize(expr:str):
	if expr in expr_cache:
		return expr_cache[expr]
	expr_length = len(expr)

	sequence = []
	item = _new_item()
	buffer = ''
	is_start = True
	is_operator = False
	is_string = False
	string_type = ''

	i = -1
	for char in expr:
		i += 1
		if is_string:
			if char == string_type:
				is_string = False
				continue

		else:
			if char == ' ' or char == '\n' or char == '\t': continue

			if is_start:
				if char in NUM:
					item['t'] = 'INT'
				elif char in ['\'','"']:
					item['t'] = 'STR'
					is_string = True
					string_type = char
					continue
				elif _check_ahead(expr, i, 'true') or _check_ahead(expr, i, 'false'):
					item['t'] = 'BOOL'
				elif is_valid_name(char):
					item['t'] = 'REF'
				is_start = False

			if is_operator == False:
				# If "." found in INT, convert to FLOAT.
				if item['t'] == 'INT':
					if char == '.':
						item['t'] = 'FLOAT'
					elif char not in NUM and char != '_':
						ScriptErrors.error(f'Invalid number construct, invalid character "{char}".')
						return

				# Start operator.
				if is_special_symbol(char):
					item['d'] = _get_literal(item['t'], buffer)
					sequence.append(item)
					item =_new_item()
					buffer = ''
					is_operator = True

			# End operator.
			if is_operator == True and expr_length > i+1 and not is_special_symbol(expr[i+1]):
				sequence.append(_new_item(
					t='OP',
					d=buffer+char
				))
				item = _new_item()
				buffer = ''
				is_operator = False
				is_start = True
				continue

		buffer += char

	if item['d'] == None and len(buffer) > 0:
		item['d'] = _get_literal(item['t'], buffer)
		buffer = ''
		sequence.append(item)
	expr_cache[expr] = sequence
	return sequence


def _resolve(state:dict, item:dict):
	if item['t'] == 'REF':
		if ScopeState.is_name_globally_free(state, item['d']) == True:
			ScriptErrors.error(ScriptErrors.err_name_does_not_exist(item['d']))
			return
		return ScopeState.get_data_globally(state, item['d'])

	elif item['t'] in ['OP','BOOL','INT','FLOAT','STR']:
		return item


def exec(state:dict, expr:str):
	sequence = tokenize(expr)
	result = None
	op = None
	op_symbol = ''
	i = -1
	for item in sequence:
		i += 1
		if op != None:
			result = op.exec(state, result, _resolve(state,item), op_symbol)
			op = None
			op_symbol = ''
			continue

		elif item['t'] == 'OP':
			if item['d'] not in OPERATORS or OPERATORS[item['d']] == None:
				ScriptErrors.error(f'Invalid operator "{item['d']}".')
				return
			op = OPERATORS[item['d']]
			op_symbol = item['d']
			continue

		else:
			result = _resolve(state, item)
			continue

	return result
