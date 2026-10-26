#pragma once
// Fragment Size: 992


static void OP_Set_arith_set(const OpSymbol& symbol, Variant*& o1, Variant*& o2) {
	switch (symbol) {
		case OpSymbol_add_set: {*o1 = *o1 + *o2; break;}
		case OpSymbol_sub_set: {*o1 = *o1 - *o2; break;}
		case OpSymbol_mul_set: {
			if (o1->t == VT_STR && o2->t == VT_INT) {
				multiply_string(AnyCast(STR_t,o1->d), (uint32_t)AnyCast(INT_t,o2->d));
			}
			else if (o1->t == VT_ARR && o2->t == VT_INT) {
				multiply_vector(AnyCast(ARR_t,o1->d), (uint32_t)AnyCast(INT_t,o2->d));
			}
			else *o1 = *o1 * *o2;
			break;
		}
		case OpSymbol_div_set: {*o1 = *o1 / *o2; break;}
		case OpSymbol_mod_set: {*o1 = *o1 % *o2; break;}
		default: break;
	}
}


void OP_Set_exec(ItyState& state, ExprState& _expr_state, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) {
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
			VariantType t = o1->t;
			VariantMode m = o1->m;
			OP_Set_arith_set(symbol, o1, o2);
			if (not o1->type_mode_matches(o1->t, o1->m, t, m)) return;
			result_ptr = o1;
			break;
		};
	}
}




const auto OP_Set = new Operation{
	OP_Set_exec,
};
