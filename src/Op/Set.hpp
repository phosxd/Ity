#pragma once


static Variant OP_Set_arith_set(const OpSymbol& symbol, Variant*& o1, Variant*& o2) {
	switch (symbol) {
		case OpSymbol_add_set: {return *o1 + *o2;}
		case OpSymbol_sub_set: {return *o1 - *o2;}
		case OpSymbol_mul_set: {return *o1 * *o2;}
		case OpSymbol_div_set: {return *o1 / *o2;}
		case OpSymbol_mod_set: {return *o1 % *o2;}
		default: return VPS.empty;
	}
}


void OP_Set_exec(ItyState& state, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);
	Variant* o2 = resovlve_potential_ref(state, second);

	// Throw error if target variant is constant.
	if (o1->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return;
	}

	switch (symbol) {
		case OpSymbol_set: {
			if (not (*o2).matches(*o1)) return;
			o1->t = o2->t;
			o1->d = o2->d;
			result_ptr = o1;
			break;
		}

		// Move second into first, unsetting the second variant.
		case OpSymbol_mov_set: {
			if (not o2->matches(*o1)) return;
			// Throw error if source variant is constant.
			if (second->m == VariantMode_constant) {
				emit_error(ERR_cannot_change_constant);
				return;
			}
			// Move second into first.
			*o1 = std::move(*o2);
			result_ptr = first;
			break;
		}

		default: {
			Variant var = OP_Set_arith_set(symbol, o1, o2);
			if (not var.matches(*o1)) return;
			var.m = o1->m;
			*o1 = std::move(var);
			result_ptr = o1;
			break;
		};
	}
}




const auto OP_Set = new Operation{
	OP_Set_exec,
};
