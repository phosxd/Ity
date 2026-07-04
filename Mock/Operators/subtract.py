import ScriptErrors
import ScopeState
import ExpressionParser

MAP = {
	'INT': ['INT', 'FLOAT'],
	'FLOAT': ['FLOAT', 'INT'],
}


def exec(state:dict, first:dict, second:dict, symbol:str):
	if first == None and second['t'] in MAP.keys():
		return {
			't': second['t'],
			'd': -second['d'],
		}

	if first['t'] == None or second['t'] == None:
		ScriptErrors.error(f'Cannot subtract value of no type.')
		return

	if first['t'] not in MAP:
		ScriptErrors.error(f'Invalid type for "subtract" operation "{first['t']}".')
		return

	expected_second_type = MAP.get(first['t'])
	if second['t'] not in expected_second_type:
		ScriptErrors.error(ScriptErrors.err_operand_type_mismatch('subtract', first['t'], second['t']))
		return

	type = first['t']
	if (first['t'] == 'INT' and second['t'] == 'FLOAT') or (first['t'] == 'FLOAT' and second['t'] == 'INT'):
		type = 'FLOAT'

	if symbol == '--': second['d'] = -second['d']
	return {
		't': type,
		'd': first['d'] - second['d'],
	}

