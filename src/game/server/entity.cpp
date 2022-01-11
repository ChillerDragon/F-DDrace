/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include "entity.h"
#include "gamecontext.h"
#include "player.h"

CEntity::CEntity(CGameWorld *pGameWorld, int ObjType, vec2 Pos, int ProximityRadius)
{
	m_pGameWorld = pGameWorld;

	m_pPrevTypeEntity = 0;
	m_pNextTypeEntity = 0;

	m_ID = Server()->SnapNewID();
	m_ObjType = ObjType;

	m_ProximityRadius = ProximityRadius;

	m_MarkedForDestroy = false;
	m_Pos = Pos;

	// F-DDrace
	m_PlotID = -1;
	m_BrushCID = -1;
}

CEntity::~CEntity()
{
	GameWorld()->RemoveEntity(this);
	Server()->SnapFreeID(m_ID);
}

int CEntity::NetworkClipped(int SnappingClient, bool CheckShowAll, bool DefaultRange)
{
	return NetworkClipped(SnappingClient, m_Pos, CheckShowAll, DefaultRange);
}

int CEntity::NetworkClipped(int SnappingClient, vec2 CheckPos, bool CheckShowAll, bool DefaultRange)
{
	if (SnappingClient == -1 || (CheckShowAll && GameServer()->m_apPlayers[SnappingClient]->m_ShowAll))
		return 0;

	// Border to also receive objects a bit off the screen so they dont pop up, 6 blocks should be okay
	float Border = 32.f * 6.f;
	vec2 ShowDistance = GameServer()->m_apPlayers[SnappingClient]->m_ShowDistance;
	if (m_PlotID >= PLOT_START || DefaultRange)
		ShowDistance = GameServer()->m_apPlayers[SnappingClient]->m_StandardShowDistance;

	float dx = GameServer()->m_apPlayers[SnappingClient]->m_ViewPos.x-CheckPos.x;
	if(absolute(dx) > ShowDistance.x/2.f + Border)
		return 1;

	float dy = GameServer()->m_apPlayers[SnappingClient]->m_ViewPos.y-CheckPos.y;
	if(absolute(dy) > ShowDistance.y/2.f + Border)
		return 1;

	return 0;
}

bool CEntity::GameLayerClipped(vec2 CheckPos)
{
	int rx = round_to_int(CheckPos.x) / 32;
	int ry = round_to_int(CheckPos.y) / 32;
	return (rx < -200 || rx >= GameServer()->Collision()->GetWidth()+200)
			|| (ry < -200 || ry >= GameServer()->Collision()->GetHeight()+200);
}

bool CEntity::GetNearestAirPos(vec2 Pos, vec2 PrevPos, vec2* pOutPos)
{
	for(int k = 0; k < 16 && GameServer()->Collision()->CheckPoint(Pos); k++)
	{
		Pos -= normalize(PrevPos - Pos);
	}

	vec2 PosInBlock = vec2(round_to_int(Pos.x) % 32, round_to_int(Pos.y) % 32);
	vec2 BlockCenter = vec2(round_to_int(Pos.x), round_to_int(Pos.y)) - PosInBlock + vec2(16.0f, 16.0f);

	*pOutPos = vec2(BlockCenter.x + (PosInBlock.x < 16 ? -2.0f : 1.0f), Pos.y);
	if (!GameServer()->Collision()->TestBox(*pOutPos, vec2(28.0f, 28.0f)))
		return true;

	*pOutPos = vec2(Pos.x, BlockCenter.y + (PosInBlock.y < 16 ? -2.0f : 1.0f));
	if (!GameServer()->Collision()->TestBox(*pOutPos, vec2(28.0f, 28.0f)))
		return true;

	*pOutPos = vec2(BlockCenter.x + (PosInBlock.x < 16 ? -2.0f : 1.0f),
		BlockCenter.y + (PosInBlock.y < 16 ? -2.0f : 1.0f));
	if (!GameServer()->Collision()->TestBox(*pOutPos, vec2(28.0f, 28.0f)))
		return true;

	return false;
}

bool CEntity::GetNearestAirPosPlayer(vec2 PlayerPos, vec2* OutPos)
{
	for (int dist = 5; dist >= -1; dist--)
	{
		*OutPos = vec2(PlayerPos.x, PlayerPos.y - dist);
		if (!GameServer()->Collision()->TestBox(*OutPos, vec2(28.0f, 28.0f)))
		{
			return true;
		}
	}
	return false;
}

bool CEntity::IsPlotDoor()
{
	return m_ObjType == CGameWorld::ENTTYPE_DOOR && GameServer()->Collision()->IsPlotDoor(m_Number);
}
