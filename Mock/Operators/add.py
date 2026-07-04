import ScriptErrors
import ScopeState
import ExpressionParser

MAP = {
	'INT': ['INT', 'FLOAT'],
	'FLOAT': ['FLOAT', 'INT'],
	'STR': ['STR'],
}


def exec(state:dict, first:dict, second:dict, _symbol:str):
	if first['t'] == None or second['t'] == None:
		ScriptErrors.error(f'Cannot add value of no type.')
		return

	if first['t'] not in MAP:
		ScriptErrors.error(f'Invalid type for "add" operation "{first['t']}".')
		return

	expected_second_type = MAP.get(first['t'])
	if second['t'] not in expected_second_type:
		ScriptErrors.error(ScriptErrors.err_operand_type_mismatch('add', first['t'], second['t']))
		return

	type = first['t']
	if (first['t'] == 'INT' and second['t'] == 'FLOAT') or (first['t'] == 'FLOAT' and second['t'] == 'INT'):
		type = 'FLOAT'

	return {
		't': type,
		'd': first['d'] + second['d'],
	}

