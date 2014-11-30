#pragma once

#include "Header.h"

class Target
{
public:
	Target(){}
	~Target(){}

	HexaDirection			GetTargetDirection(){ return m_Direction; }
	int						GetTargetRange(){ return m_Range; }
	vector<HexaPoint>*		GetTargetCourseStack(){ return &m_CourseStack; }
	
	ScreenPoint				GetStartPoint(){ return m_StartPoint; }
	ScreenPoint				GetCursoredPoint(){ return m_CursoredPoint; }
	ScreenPoint				GetSelectedUnitPoint(){ return m_SelectedUnitPoint; }
	int						GetSelectedUnit(){ return m_SelectedUnit; }



	void					SetTargetDirection(HexaDirection args){ m_Direction = args; }
	void					SetTargetRange(int args){ m_Range = args; }
	void					SetTargetCourseStack(vector<HexaPoint>* args){ m_CourseStack = *args; }

	void					SetStartPoint(ScreenPoint args){ m_StartPoint = args; }
	void					SetCursoredPoint(ScreenPoint args){ m_CursoredPoint = args; }
	void					SetSelectedUnitPoint(ScreenPoint args){ m_SelectedUnitPoint = args; }
	void					SetSelectedUnit(int args){ m_SelectedUnit = args; }


private:
	/*  이동, 스킬 범위 기록 변수  */
	HexaDirection			m_Direction;
	int						m_Range;
	vector<HexaPoint>		m_CourseStack;

	ScreenPoint				m_StartPoint;
	ScreenPoint				m_CursoredPoint;
	ScreenPoint				m_SelectedUnitPoint;
	int						m_SelectedUnit;
};

