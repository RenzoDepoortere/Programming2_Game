#include "pch.h"
#include "Game.h"
#include "SVGParser.h"
#include <vector>

#include"Stage.h"

#include <iostream>

Game::Game( const Window& window ) 
	:m_Window{ window }
{
	Initialize( );
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	m_pStage = new Stage{ Rectf{0,0,m_Window.width,m_Window.height} };
}

void Game::Cleanup( )
{
	delete m_pStage;
	m_pStage = nullptr;
}

void Game::Update( float elapsedSec )
{
	m_pStage->Update(elapsedSec);
}

void Game::Draw( ) const
{
	ClearBackground( );

	m_pStage->Draw();
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	switch ( e.keysym.sym )
	{

	case SDLK_t:
		m_pStage->DevTeleport();

		break;
	
	case SDLK_i:
		PrintControls();

		break;
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::PrintControls()
{
	std::cout << "Game controls:" << '\n';
	std::cout << "A / D: Move" << '\n';
	std::cout << "W: Look up" << '\n';
	std::cout << "S: Crouch" << '\n';
	std::cout << "SPACE: Jump" << '\n';
	std::cout << "ENTER: Shoot / Start / Select" << '\n';
	std::cout << "ESC: Pause-Menu" << '\n';
}