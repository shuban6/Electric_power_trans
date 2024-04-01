#include "IspImp.hpp"

#include "Led.hpp"
#include "Buzzer.hpp"
#include "Dr16.hpp"
#include "CanManager.hpp"
#include "CanMsgDispatcher.hpp"
#include "JudgeSystem.hpp"

#include "BoardPacket.hpp"
#include "HostPacket.hpp"

#include "Time.hpp"

#include "Testbot.hpp"

extern "C" void SysTick_Handler(void);

const uint32_t MS_PRE_TICK = 1;

Testbot testbot;

void SysTick_Handler(void)
{
	// Disable all interrupt
	__set_PRIMASK(1);

	Time::Tick();

	Dr16::Instance()->Update();	
	CanMsgDispatcher::Instance()->Update();
	// JudgeSystem::Instance()->Update();
	// BoardPacketManager::Instance()->Update();
	HostPacketManager::Instance()->Update();

	testbot.Tick();

	DjiCanMotorCommander::Instance()->Update();

	BoardPacketManager::Instance()->FlushSendBuffer();
	HostPacketManager::Instance()->FlushSendBuffer();

	CanManager::Instance()->Update();

	// Enable all interrupt
	__set_PRIMASK(0);
}

// Debug handlers
Dr16* pDr16 = Dr16::Instance();
CanManager* pCanManager = CanManager::Instance();
BoardPacketManager* pBoardPacketManager = BoardPacketManager::Instance();
HostPacketManager* pHostPacketManager = HostPacketManager::Instance();

int main(void)
{
	// NVIC setup. Remember, no Preemption.
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	// Hardware init
	Buzzer::Instance()->Init();
	Led::Init();
	Dr16::Instance()->Init();
	CanManager::Instance()->Init();

	BoardPacketManager::Instance()->Init();
	
	HostPacketManager::Instance()->Init();

	// RobotEngine init
	testbot.Init();

	// System time init, all the logic are running in SysTick_Handler
	Time::Init(MS_PRE_TICK);
	
	while (1)
	{
	}
}
