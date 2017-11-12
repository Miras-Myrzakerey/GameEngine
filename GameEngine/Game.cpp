#include "Game.h"

Game::Game(const GameData& data) 
:System(SystemType::Sys_Game){
	m_SystemType = SystemType::Sys_Game;
}

Game::~Game() {
}
