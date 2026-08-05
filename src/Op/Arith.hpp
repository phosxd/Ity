#pragma once


void OP_Arith_exec(ScopeState& state, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& _result_ptr) {
	// All logic for actually getting the results of an arithmetic operation are in `Common.hpp`.
	Variant* o1 = resovlve_potential_ref(state, first);
	Variant* o2 = resovlve_potential_ref(state, second);
	switch (symbol) {
		case OpSymbol_add: {result = *o1 + *o2;  break;}
		case OpSymbol_sub: {result = *o1 - *o2;  break;}
		case OpSymbol_mul: {result = *o1 * *o2;  break;}
		case OpSymbol_div: {result = *o1 / *o2;  break;}
		case OpSymbol_mod: {result = *o1 % *o2;  break;}
		default: break;
	}
}




const auto OP_Arith = new Operation{
	OP_Arith_exec,
};
