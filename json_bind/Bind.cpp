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
	}
}