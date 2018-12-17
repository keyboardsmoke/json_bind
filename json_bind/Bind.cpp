#include "pch.h"
#include "Bind.h"

BoundElement::BoundElement(BoundElement* parent, std::string name) : m_parent(parent), m_name(name), m_value(Json::Value::null)
{
	BoundObject* objectParent = dynamic_cast<BoundObject *>(parent);

	if (objectParent)
	{
		objectParent->AddElement(this);
	}
}

std::string BoundString::GetValue()
{
	if (!IsDefined())
		throw BoundElementDoesNotExist(m_parent->GetName(), m_name);

	return m_value.asString();
}

bool BoundBoolean::GetValue()
{
	if (!IsDefined())
		throw BoundElementDoesNotExist(m_parent->GetName(), m_name);

	return m_value.asBool();
}

int BoundInteger::GetValue()
{
	if (!IsDefined())
		throw BoundElementDoesNotExist(m_parent->GetName(), m_name);

	return m_value.asInt();
}

unsigned int BoundUnsignedInteger::GetValue()
{
	if (!IsDefined())
		throw BoundElementDoesNotExist(m_parent->GetName(), m_name);

	return m_value.asUInt();
}

double BoundDouble::GetValue()
{
	if (!IsDefined())
		throw BoundElementDoesNotExist(m_parent->GetName(), m_name);

	return m_value.asDouble();
}

template<typename T>
static void CheckAndSetArrayValue(BoundObject* parent, Json::Value& root, Json::Value& setValue, BoundArray<T>* ba, bool requireAllFields)
{
	Json::Value value = root.get(ba->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (requireAllFields)
			throw BoundElementDoesNotExist(parent->GetName(), ba->GetName());
	}
	else if (value.type() != Json::arrayValue || value.isArray() == false)
	{
		throw BoundElementInvalidType(parent->GetName(), ba->GetName(), ValueTypeToString(Json::arrayValue), ValueTypeToString(value.type()));
	}
	else
	{
		setValue = value;

		// Now that we have the value we have to initialize the array as well...
	}
}

void BoundString::InitializeForObject(BoundElement* parent)
{
	Json::Value value = parent->GetJsonObject().get(this->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (BoundObject* objectParent = dynamic_cast<BoundObject*>(parent))
		{
			if (objectParent->RequiresAllFields())
				throw BoundElementDoesNotExist(m_name, this->GetName());
			else
				SetJsonObject(value);
		}
		else
		{
			SetJsonObject(value);
		}
	}
	else if (value.type() != Json::stringValue)
	{
		throw BoundElementInvalidType(m_name, this->GetName(), ValueTypeToString(Json::stringValue), ValueTypeToString(value.type()));
	}
	else
	{
		SetJsonObject(value);
	}
}

void BoundBoolean::InitializeForObject(BoundElement* parent)
{
	Json::Value value = parent->GetJsonObject().get(this->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (BoundObject* objectParent = dynamic_cast<BoundObject*>(parent))
		{
			if (objectParent->RequiresAllFields())
				throw BoundElementDoesNotExist(m_name, this->GetName());
			else
				SetJsonObject(value);
		}
		else
		{
			SetJsonObject(value);
		}
	}
	else if (value.type() != Json::booleanValue)
	{
		throw BoundElementInvalidType(m_name, this->GetName(), ValueTypeToString(Json::booleanValue), ValueTypeToString(value.type()));
	}
	else
	{
		SetJsonObject(value);
	}
}

void BoundInteger::InitializeForObject(BoundElement* parent)
{
	Json::Value value = parent->GetJsonObject().get(this->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (BoundObject* objectParent = dynamic_cast<BoundObject*>(parent))
		{
			if (objectParent->RequiresAllFields())
				throw BoundElementDoesNotExist(m_name, this->GetName());
			else
				SetJsonObject(value);
		}
		else
		{
			SetJsonObject(value);
		}
	}
	else if (value.type() != Json::intValue)
	{
		throw BoundElementInvalidType(m_name, this->GetName(), ValueTypeToString(Json::intValue), ValueTypeToString(value.type()));
	}
	else
	{
		SetJsonObject(value);
	}
}

void BoundUnsignedInteger::InitializeForObject(BoundElement* parent)
{
	Json::Value value = parent->GetJsonObject().get(this->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (BoundObject* objectParent = dynamic_cast<BoundObject*>(parent))
		{
			if (objectParent->RequiresAllFields())
				throw BoundElementDoesNotExist(m_name, this->GetName());
			else
				SetJsonObject(value);
		}
		else
		{
			SetJsonObject(value);
		}
	}
	else if (value.type() != Json::uintValue)
	{
		throw BoundElementInvalidType(m_name, this->GetName(), ValueTypeToString(Json::uintValue), ValueTypeToString(value.type()));
	}
	else
	{
		SetJsonObject(value);
	}
}

void BoundDouble::InitializeForObject(BoundElement* parent)
{
	Json::Value value = parent->GetJsonObject().get(this->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (BoundObject* objectParent = dynamic_cast<BoundObject*>(parent))
		{
			if (objectParent->RequiresAllFields())
				throw BoundElementDoesNotExist(m_name, this->GetName());
			else
				SetJsonObject(value);
		}
		else
		{
			SetJsonObject(value);
		}
	}
	else if (value.type() != Json::realValue)
	{
		throw BoundElementInvalidType(m_name, this->GetName(), ValueTypeToString(Json::realValue), ValueTypeToString(value.type()));
	}
	else
	{
		SetJsonObject(value);
	}
}

void BoundObject::InitializeForObject(BoundElement* parent)
{
	Json::Value value = parent->GetJsonObject().get(this->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (BoundObject* objectParent = dynamic_cast<BoundObject*>(parent))
		{
			if (objectParent->RequiresAllFields())
				throw BoundElementDoesNotExist(m_name, this->GetName());
			else
				SetJsonObject(value);
		}
		else
		{
			SetJsonObject(value);
		}
	}
	else if (value.type() != Json::objectValue)
	{
		throw BoundElementInvalidType(m_name, this->GetName(), ValueTypeToString(Json::objectValue), ValueTypeToString(value.type()));
	}
	else
	{
		SetJsonObject(value);
		Initialize();
	}
}

void BoundObject::Initialize()
{
	for (auto& element : m_elements)
	{
		element.second->InitializeForObject(this);

		/*
		if (BoundString* bs = dynamic_cast<BoundString*>(element.second))
		{
			bs->InitializeForObject(this);
		}
		else if (BoundBoolean* bb = dynamic_cast<BoundBoolean*>(element.second))
		{
			bb->InitializeForObject(this);
		}
		else if (BoundInteger* bi = dynamic_cast<BoundInteger*>(element.second))
		{
			Json::Value value = m_root.get(bi->GetName(), Json::Value::null);
			if (value == Json::Value::null)
			{
				if (m_requireAllFields)
					throw BoundElementDoesNotExist(m_name, element.first);
			}
			else if (value.type() != Json::intValue)
			{
				throw BoundElementInvalidType(m_name, element.first, ValueTypeToString(Json::intValue), ValueTypeToString(value.type()));
			}
			else
			{
				bi->m_value = value;
			}
		}
		else if (BoundUnsignedInteger* bu = dynamic_cast<BoundUnsignedInteger*>(element.second))
		{
			Json::Value value = m_root.get(bu->GetName(), Json::Value::null);
			if (value == Json::Value::null)
			{
				if (m_requireAllFields)
					throw BoundElementDoesNotExist(m_name, element.first);
			}
			else if (value.type() != Json::uintValue)
			{
				throw BoundElementInvalidType(m_name, element.first, ValueTypeToString(Json::uintValue), ValueTypeToString(value.type()));
			}
			else
			{
				bu->m_value = value;
			}
		}
		else if (BoundDouble* bd = dynamic_cast<BoundDouble*>(element.second))
		{
			Json::Value value = m_root.get(bd->GetName(), Json::Value::null);
			if (value == Json::Value::null)
			{
				if (m_requireAllFields)
					throw BoundElementDoesNotExist(m_name, element.first);
			}
			else if (value.type() != Json::realValue)
			{
				throw BoundElementInvalidType(m_name, element.first, ValueTypeToString(Json::realValue), ValueTypeToString(value.type()));
			}
			else
			{
				bd->m_value = value;
			}
		}
		else if (BoundObject* bc = dynamic_cast<BoundObject*>(element.second))
		{
			Json::Value value = m_root.get(bc->GetName(), Json::Value::null);
			if (value == Json::Value::null)
			{
				if (m_requireAllFields)
					throw BoundElementDoesNotExist(m_name, element.first);
			}
			else if (value.type() != Json::objectValue)
			{
				throw BoundElementInvalidType(m_name, element.first, ValueTypeToString(Json::objectValue), ValueTypeToString(value.type()));
			}
			else
			{
				bc->m_root = value;
				bc->m_value = value;
				bc->Initialize();
			}
		}
		else if (BoundArray<BoundString>* ba = dynamic_cast<BoundArray<BoundString>*>(element.second))
		{
			CheckAndSetArrayValue<BoundString>(this, m_root, ba->m_value, ba, m_requireAllFields);
		}
		else if (BoundArray<BoundBoolean>* ba = dynamic_cast<BoundArray<BoundBoolean>*>(element.second))
		{
			CheckAndSetArrayValue<BoundBoolean>(this, m_root, ba->m_value, ba, m_requireAllFields);
		}
		else if (BoundArray<BoundInteger>* ba = dynamic_cast<BoundArray<BoundInteger>*>(element.second))
		{
			CheckAndSetArrayValue<BoundInteger>(this, m_root, ba->m_value, ba, m_requireAllFields);
		}
		else if (BoundArray<BoundUnsignedInteger>* ba = dynamic_cast<BoundArray<BoundUnsignedInteger>*>(element.second))
		{
			CheckAndSetArrayValue<BoundUnsignedInteger>(this, m_root, ba->m_value, ba, m_requireAllFields);
		}
		else if (BoundArray<BoundDouble>* ba = dynamic_cast<BoundArray<BoundDouble>*>(element.second))
		{
			CheckAndSetArrayValue<BoundDouble>(this, m_root, ba->m_value, ba, m_requireAllFields);
		}
		else if (BoundArray<BoundObject>* ba = dynamic_cast<BoundArray<BoundObject>*>(element.second))
		{
			std::cout << "BoundArray<BoundObject> Handling [" << ba->GetName() << "]" << std::endl;

			CheckAndSetArrayValue<BoundObject>(this, m_root, ba->m_value, ba, m_requireAllFields);

			// Things are defined...
			if (ba->IsDefined())
			{
				for (auto& arrayValue : ba->m_value)
				{
					BoundObject* newObject = new BoundObject();

					newObject->m_root = arrayValue;
					newObject->m_value = arrayValue;

					newObject->Initialize();

					ba->m_arrayCache.push_back(newObject);
				}
			}
		}*/
	}
}