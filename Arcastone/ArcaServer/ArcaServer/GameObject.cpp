#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void GameObject::InitObject(ObjectNumber objectNum)
{
	m_ObjectNum = objectNum;
}
