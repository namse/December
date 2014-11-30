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
	PlayerNumber				m_Turn;				// 누구 차례인지 나타냄
	int							m_BreakDownTurn;	// 땅이 무너진다!
	int							m_PlayTurn;			// 경과한 턴
	bool						m_IsFirstTurn;
};

