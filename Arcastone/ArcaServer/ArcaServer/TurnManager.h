#pragma once

class TurnManager
{
public:
	TurnManager();
	~TurnManager();

	void						TurnSwap();

	int							GetBreakDownTurn(){ return m_BreakDownTurn; }

	void						TurnFlow(){ m_PlayTurn++; m_IsFirstTurn = false; }
	int							GetFlowTurn(){ return m_PlayTurn; }

	PlayerNumber				GetWhosTurn(){ return m_Turn; }

	bool						GetIsFirstTurn(){ return m_IsFirstTurn; }

private:
	PlayerNumber				m_Turn;				// ���� �������� ��Ÿ��
	int							m_BreakDownTurn;	// ���� ��������!
	int							m_PlayTurn;			// ����� ��
	bool						m_IsFirstTurn;
};

