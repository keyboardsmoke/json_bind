#pragma once

static std::string ValueTypeToString(Json::ValueType type)
{
	switch (type)
	{
	case Json::ValueType::arrayValue: return "Array";
	case Json::ValueType::booleanValue: return "Array";
	case Json::ValueType::intValue: return "Int";
	case Json::ValueType::nullValue: return "Null";
	case Json::ValueType::objectValue: return "Object";
	case Json::ValueType::realValue: return "Real";
	case Json::ValueType::stringValue: return "String";
	case Json::ValueType::uintValue: return "UInt";
	default: __assume(0);
	}
}

class BoundObject;

class BoundException : public std::exception
{
public:
	BoundException(const std::string& what) : m_what(what) {}

	virtual char const* what() const
	{
		return m_what.c_str();
	}

protected:
	std::string m_what;
};

class BoundElementDoesNotExist : public BoundException
{
public:
	BoundElementDoesNotExist(const std::string& rootName, const std::string& elementName) : BoundException(std::string())
	{
		std::stringstream ss;
		ss << "Element in \"" << rootName << "\" called \"" << elementName << "\" does not exist.";
		m_what = ss.str();
	}
};

class BoundElementInvalidType : public BoundException
{
public:
	BoundElementInvalidType(
		const std::string& rootName,
		const std::string& elementName, 
		const std::string& expectedType, 
		const std::string& actualType) : BoundException(std::string())
	{
		std::stringstream ss;
		ss << "Element in \"" << rootName << "\" called \"" << elementName << "\" has an invalid type (expected " << 
			expectedType << ", got " << actualType << ".";

		m_what = ss.str();
	}
};

class BoundElement
{
public:
	friend class BoundObject;

	BoundElement(BoundElement* parent, std::string name);
	virtual ~BoundElement() = default;

	virtual void InitializeForObject(BoundElement* parent) = 0;

	std::string& GetName()
	{
		return m_name;
	}

	bool IsDefined()
	{
		return m_value.type() != Json::ValueType::nullValue;
	}

	Json::Value& GetJsonObject()
	{
		return m_value;
	}

	void SetJsonObject(Json::Value& object)
	{
		m_value = object;
	}

protected:
	BoundElement* m_parent;
	std::string m_name;
	Json::Value m_value;
};

class BoundString : public BoundElement
{
public:
	BoundString(BoundElement* parent, std::string name) : BoundElement(parent, name) {}

	virtual void InitializeForObject(BoundElement* parent) override;

	std::string GetValue();
};

class BoundBoolean : public BoundElement
{
public:
	BoundBoolean(BoundElement* parent, std::string name) : BoundElement(parent, name) {}

	virtual void InitializeForObject(BoundElement* parent) override;

	bool GetValue();
};

class BoundInteger : public BoundElement
{
public:
	BoundInteger(BoundElement* parent, std::string name) : BoundElement(parent, name) {}

	virtual void InitializeForObject(BoundElement* parent) override;

	int GetValue();
};

class BoundUnsignedInteger : public BoundElement
{
public:
	BoundUnsignedInteger(BoundElement* parent, std::string name) : BoundElement(parent, name) {}

	virtual void InitializeForObject(BoundElement* parent) override;

	unsigned int GetValue();
};

class BoundDouble : public BoundElement
{
public:
	BoundDouble(BoundElement* parent, std::string name) : BoundElement(parent, name) {}

	virtual void InitializeForObject(BoundElement* parent) override;

	double GetValue();
};

class BoundObject : public BoundElement
{
public:
	// Special case initialization where custom code has to be written
	BoundObject() : BoundElement(nullptr, std::string()) {}

	// Root initialization
	BoundObject(Json::Value root, bool requireAllFields = false) : 
		BoundElement(nullptr, ""), m_requireAllFields(requireAllFields) 
	{
		SetJsonObject(root);
	}

	// Sub-object initialization
	BoundObject(BoundElement* parent, std::string name, bool requireAllFields = false) :
		BoundElement(parent, name), m_requireAllFields(requireAllFields)
	{
		if (BoundObject *objectParent = dynamic_cast<BoundObject*>(parent))
		{
			Json::Value nextObject = parent->m_value.get(name, Json::Value::null);
			if (nextObject == Json::Value::null)
				throw BoundElementDoesNotExist(parent->GetName(), name);

			SetJsonObject(nextObject);
		}
	}

	virtual void InitializeForObject(BoundElement* parent) override;

	void AddElement(BoundElement* element)
	{
		m_elements[element->GetName()] = element;
	}

	void Initialize();

	bool RequiresAllFields()
	{
		return m_requireAllFields;
	}

protected:
	bool m_requireAllFields;
	std::unordered_map<std::string, BoundElement*> m_elements;
};

template<typename T>
class BoundArray : public BoundElement
{
public:
	friend class BoundObject;

	BoundArray(BoundElement* parent, std::string name) : BoundElement(parent, name) {}

	virtual void InitializeForObject(BoundElement* parent) override
	{
		Json::Value value = parent->GetJsonObject().get(GetName(), Json::Value::null);
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
		else if (value.type() != Json::arrayValue)
		{
			throw BoundElementInvalidType(parent->GetName(), GetName(), ValueTypeToString(Json::arrayValue), ValueTypeToString(value.type()));
		}
		else
		{
			SetJsonObject(value);

			// Now we should go through the list and try to initialize objects...
			for (auto& arrayValue : m_value)
			{
				T* newArrayValue = new T(this, std::string());

				newArrayValue->SetJsonObject(arrayValue);

				// If it's some sort of object, we need to initialize...
				if (BoundObject* arrayValueObject = dynamic_cast<BoundObject *>(newArrayValue))
				{
					arrayValueObject->Initialize();
				}

				m_arrayCache.push_back(newArrayValue);
			}
		}
	}

	std::vector<T *> GetValue()
	{
		if (!IsDefined())
			throw BoundElementDoesNotExist(m_parent->GetName(), m_name);

		return m_arrayCache;
	}

private:
	std::vector<T *> m_arrayCache;
};