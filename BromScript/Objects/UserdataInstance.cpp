#include "../Objects/UserdataInstance.h"
#include "../Objects/Variable.h"
#include "../Managers/Instance.h"

namespace BromScript{
	UserdataInstance::UserdataInstance(void) :CallDTor(false) {
	}

	UserdataInstance::~UserdataInstance(void) {
		if (!this->CallDTor || this->TypeData == null || this->TypeData->Dtor == null || this->Ptr == null)
			return;

		this->TypeData->Dtor(this->TypeData->BromScript, this->Ptr);
		this->Ptr = null;
	}

	void UserdataInstance::SetIndex(Variable* selfobj, Variable* keyvar, Variable* value) {
		Variable* member = this->GetIndex(selfobj, keyvar);
		if (this->TypeData->BromScript->GetCurrentFunction()->ForceReturn) return;

		if (member->Type != VariableType::Null) {
			if (Converter::SetMember(this->TypeData->BromScript, member, value, keyvar->ToString())) {
				return;
			}
		}

		BSFunction setop = this->GetOperator(Operators::ArithmeticSetIndex);
		if (setop != nullptr) {
			ArgumentData args;
			args.SetThisObject(selfobj);
			args.AddVariable(keyvar);
			args.AddVariable(value);

			Variable* ret = setop(this->TypeData->BromScript, &args);
			if (ret != null) {
				this->TypeData->BromScript->GC.RegisterVariable(ret);
			}

			return;
		}

		BS_THROW_ERROR(this->TypeData->BromScript, Scratch::CString::Format("No index called '%s' to set in %s type", keyvar->ToString(this->TypeData->BromScript).str_szBuffer, Converter::TypeToString(this->TypeData->BromScript, (VariableType::Enum)this->TypeData->TypeID).str_szBuffer));
	}

	Variable* UserdataInstance::GetMethod(const Scratch::CString& key) {
		Userdata* curp = this->TypeData;
		while (curp != null) {
			for (int i = 0; i < curp->Functions.Count(); i++) {
				if (curp->Functions.GetKeyByIndex(i) == key) {
					Function* func = new Function(curp->BromScript);
					func->CppFunc = curp->Functions.GetValueByIndex(i);
					func->IsCpp = true;
					func->Name = key;
					func->Filename = "C++";
					func->SetReferences(curp->BromScript->GetCurrentFunction(), 0);

					Variable* ret = curp->BromScript->GC.GetPooledVariable();
					ret->Type = VariableType::Function;
					ret->Value = func;

					return ret;
				}
			}

			curp = curp->InheritFrom;
		}

		return nullptr;
	}

	Variable* UserdataInstance::GetIndex(Variable* selfobj, Variable* keyvar) {
		Variable* ret = nullptr;
		Scratch::CString key = keyvar->ToString(this->TypeData->BromScript);

		Variable* member = this->GetMember(key);
		if (member != nullptr) return member;

		Variable* method = this->GetMethod(key);
		if (method != nullptr) return method;

		BSFunction getop = this->GetOperator(Operators::ArithmeticGetIndex);
		if (getop != nullptr) {
			ArgumentData args;
			args.SetThisObject(selfobj);
			args.AddVariable(keyvar);

			ret = getop(this->TypeData->BromScript, &args);
			if (ret == nullptr) ret = this->TypeData->BromScript->GetDefaultVarNull();
			else this->TypeData->BromScript->GC.RegisterVariable(ret);

			return ret;
		}

		// aaand we give up
		BS_THROW_ERROR(this->TypeData->BromScript, Scratch::CString::Format("No index called '%s' to get in %s type", key.str_szBuffer, Converter::TypeToString(this->TypeData->BromScript, (VariableType::Enum)this->TypeData->TypeID).str_szBuffer));
		return this->TypeData->BromScript->GetDefaultVarNull();
	}

	BSFunction UserdataInstance::GetOperator(const Operators& op) {
		Userdata* curp = this->TypeData;
		while (curp != nullptr) {
			int index = BS_ARITHMATICOP_TOFUNCINDEX(op);
			if (curp->OperatorsOverrides[index] != nullptr) {
				return curp->OperatorsOverrides[index];
			}

			curp = curp->InheritFrom;
		}
		
		return nullptr;
	}

	Variable* UserdataInstance::GetMember(const Scratch::CString& key) {
		Userdata* curp = this->TypeData;
		while (curp != nullptr) {
			for (int i = 0; i < curp->Members.Count; i++) {
				Userdata* ud = curp->Members[i];
				if (ud->Name == key) {
					Variable* ret;

					if (ud->Getter != nullptr) {
						ret = ud->Getter(curp->BromScript, (byte*)this->Ptr + ud->Offset);

						if (ret == nullptr) ret = curp->BromScript->GetDefaultVarNull();
						else curp->BromScript->GC.RegisterVariable(ret);
					} else {
						UserdataInstance* udi2 = new UserdataInstance();
						udi2->TypeData = ud;
						udi2->Ptr = (byte*)this->Ptr + udi2->TypeData->Offset;

						ret = curp->BromScript->GC.GetPooledVariable();
						ret->Value = udi2;
						ret->Type = (VariableType::Enum)udi2->TypeData->TypeID;
						ret->IsCpp = true;
					}

					return ret;
				}
			}

			curp = curp->InheritFrom;
		}

		return nullptr;
	}
}