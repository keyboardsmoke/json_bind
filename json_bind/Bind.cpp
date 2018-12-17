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
static void InitializeForObjectGeneric(T* object, BoundElement* parent, Json::ValueType type)
{
	Json::Value value = parent->GetJsonObject().get(object->GetName(), Json::Value::null);
	if (value == Json::Value::null)
	{
		if (BoundObject* objectParent = dynamic_cast<BoundObject*>(parent))
		{
			if (objectParent->RequiresAllFields())
				throw BoundElementDoesNotExist(parent->GetName(), object->GetName());
			else
				object->SetJsonObject(value);
		}
		else
		{
			object->SetJsonObject(value);
		}
	}
	else if (value.type() != type)
	{
		throw BoundElementInvalidType(parent->GetName(), object->GetName(), ValueTypeToString(type), ValueTypeToString(value.type()));
	}
	else
	{
		object->SetJsonObject(value);
	}
}

void BoundString::InitializeForObject(BoundElement* parent)
{
	InitializeForObjectGeneric<BoundString>(this, parent, Json::stringValue);
}

void BoundBoolean::InitializeForObject(BoundElement* parent)
{
	InitializeForObjectGeneric<BoundBoolean>(this, parent, Json::booleanValue);
}

void BoundInteger::InitializeForObject(BoundElement* parent)
{
	InitializeForObjectGeneric<BoundInteger>(this, parent, Json::intValue);
}

void BoundUnsignedInteger::InitializeForObject(BoundElement* parent)
{
	InitializeForObjectGeneric<BoundUnsignedInteger>(this, parent, Json::uintValue);
}

void BoundDouble::InitializeForObject(BoundElement* parent)
{
	InitializeForObjectGeneric<BoundDouble>(this, parent, Json::realValue);
}

void BoundObject::InitializeForObject(BoundElement* parent)
{
	InitializeForObjectGeneric<BoundObject>(this, parent, Json::objectValue);
	Initialize();
}

void BoundObject::Initialize()
{
	for (auto& element : m_elements)
	{
		element.second->InitializeForObject(this);
	}
}