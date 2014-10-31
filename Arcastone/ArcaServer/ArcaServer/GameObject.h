#pragma once
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void InitObject(ObjectNumber objectNum);

protected:
	ObjectNumber	m_ObjectNum;
	ObjectType		m_ObjectType;
	ObjectMoveType	m_UnitMoveType;
	std::string		m_Name;
	int				m_HP;
	int				m_Attack;
	int				m_Weight;
	int				m_MoveRange;
};

