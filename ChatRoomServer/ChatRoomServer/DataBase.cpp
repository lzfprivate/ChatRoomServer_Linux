#include "DataBase.h"

_Field_::_Field_(const _Field_& field)
{
	m_strName = field.m_strName;
	m_strType = field.m_strType;
	m_strSize = field.m_strSize;
	m_uAttr = field.m_uAttr;
	m_strDefault = field.m_strDefault;
	m_strCheck = field.m_strCheck;
	m_uCondition = field.m_uCondition;
	//列属性
	UnValueType.Integer = field.UnValueType.Integer;
	UnValueType.Double = field.UnValueType.Double;
	if (field.UnValueType.String->size() > 0) {
		UnValueType.String = new CBuffer;
		*UnValueType.String = *field.UnValueType.String;
	}
}

_Field_& _Field_::operator=(const _Field_& field)
{
	if (&field != this)
	{
		m_strName = field.m_strName;
		m_strType = field.m_strType;
		m_strSize = field.m_strSize;
		m_uAttr = field.m_uAttr;
		m_strDefault = field.m_strDefault;
		m_strCheck = field.m_strCheck;
		m_uCondition = field.m_uCondition;
		//列属性
		UnValueType.Integer = field.UnValueType.Integer;
		UnValueType.Double = field.UnValueType.Double;
		if (field.UnValueType.String->size() > 0) {
			UnValueType.String = new CBuffer;
			*UnValueType.String = *field.UnValueType.String;
		}
	}
	return *this;
}
