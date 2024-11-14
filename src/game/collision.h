/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <base/vmath.h>
#include <engine/shared/protocol.h>

#include <list>
#include <vector>
#include "mapitems.h"

enum
{
	PLOT_SMALL,
	PLOT_BIG,
	NUM_PLOT_SIZES,

	PLOT_SMALL_MAX_DOORS = 2,
	PLOT_BIG_MAX_DOORS = 5,

	PLOT_SMALL_MAX_TELE = 2,
	PLOT_BIG_MAX_TELE = 5,
};

enum
{
	CANTMOVE_LEFT=1<<0,
	CANTMOVE_RIGHT=1<<1,
	CANTMOVE_UP=1<<2,
	CANTMOVE_DOWN=1<<3,

	CANTMOVE_DIRECTIONS=0xf,

	// These get set when you can't move in the direction, caused by a
	// two-way or all-way stopper.
	CANTMOVE_LEFT_TWOWAY=1<<4,
	CANTMOVE_RIGHT_TWOWAY=1<<5,
	CANTMOVE_UP_TWOWAY=1<<6,
	CANTMOVE_DOWN_TWOWAY=1<<7,

	CANTMOVE_TWOWAY_DIRECTIONS=0xf0,

	// These get set when you can't move in the direction, caused by a
	// one-way stopper on the tile of the character.
	CANTMOVE_LEFT_HERE=1<<8,
	CANTMOVE_RIGHT_HERE=1<<9,
	CANTMOVE_UP_HERE=1<<10,
	CANTMOVE_DOWN_HERE=1<<11,

	CANTMOVE_HERE_DIRECTIONS=0xf00,

	// F-DDrace
	CANTMOVE_ROOM=1<<12,
	CANTMOVE_VIP_PLUS_ONLY=1<<13,
	CANTMOVE_PLOT_DOOR=1<<14,
	CANTMOVE_DOWN_LASERDOOR=1<<15, // used by prediction
};

vec2 ClampVel(int MoveRestriction, vec2 Vel);
typedef bool (*CALLBACK_SWITCHACTIVE)(int Number, void *pUser);

struct CAntibotMapData;

class CCollision
{
	class CTile* m_pTiles;
	int m_Width;
	int m_Height;
	class CLayers* m_pLayers;

	class CTeleTile* m_pTele;
	class CSpeedupTile* m_pSpeedup;
	class CTile* m_pFront;
	class CSwitchTile* m_pSwitch;
	class CTuneTile* m_pTune;
	class CDoorTile* m_pDoor;

public:
	CCollision();
	~CCollision();
	void Init(class CLayers* pLayers, class CConfig *pConfig);
	void FillAntibot(CAntibotMapData *pMapData);
	bool CheckPoint(float x, float y) { return IsSolid(round_to_int(x), round_to_int(y)); }
	bool CheckPoint(vec2 Pos) { return CheckPoint(Pos.x, Pos.y); }
	int GetCollisionAt(float x, float y) { return GetTile(round_to_int(x), round_to_int(y)); }
	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };
	int IntersectLine(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision);
	int IntersectLineTeleWeapon(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision, int* pTeleNr);
	int IntersectLineTeleHook(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision, int* pTeleNr);
	void MovePoint(vec2* pInoutPos, vec2* pInoutVel, float Elasticity, int* pBounces);

	struct MoveRestrictionExtra
	{
		bool m_RoomKey;
		bool m_VipPlus;

		MoveRestrictionExtra()
		{
			m_RoomKey = false;
			m_VipPlus = false;
		}
	};

	void MoveBox(CALLBACK_SWITCHACTIVE pfnSwitchActive, void *pUser, vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity, bool CheckStopper, MoveRestrictionExtra Extra = MoveRestrictionExtra());
	void MoveBox(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity, bool CheckStopper, MoveRestrictionExtra Extra = MoveRestrictionExtra())
	{
		MoveBox(0, 0, pInoutPos, pInoutVel, Size, Elasticity, CheckStopper, Extra);
	}
	bool TestBox(vec2 Pos, vec2 Size);

	// DDRace

	void Dest();
	void SetCollisionAt(float x, float y, int id);
	int GetFCollisionAt(float x, float y) { return GetFTile(round_to_int(x), round_to_int(y)); }
	int IntersectNoLaser(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision, int Number = -1);
	int IntersectNoLaserNW(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision);
	int IntersectAir(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision);
	int GetIndex(int x, int y);
	int GetIndex(vec2 PrevPos, vec2 Pos);
	int GetFIndex(int x, int y);

	int GetMoveRestrictions(CALLBACK_SWITCHACTIVE pfnSwitchActive, void *pUser, vec2 Pos, float Distance = 18.0f, int OverrideCenterTileIndex = -1, MoveRestrictionExtra Extra = MoveRestrictionExtra());
	int GetMoveRestrictions(vec2 Pos, float Distance = 18.0f, MoveRestrictionExtra Extra = MoveRestrictionExtra())
	{
		return GetMoveRestrictions(0, 0, Pos, Distance, -1, Extra);
	}

	int GetTile(int x, int y);
	int GetFTile(int x, int y);
	int Entity(int x, int y, int Layer);
	int GetPureMapIndex(float x, float y);
	int GetPureMapIndex(vec2 Pos) { return GetPureMapIndex(Pos.x, Pos.y); }
	std::list<int> GetMapIndices(vec2 PrevPos, vec2 Pos, unsigned MaxIndices = 0);
	int GetMapIndex(vec2 Pos);
	bool TileExists(int Index);
	bool TileExistsNext(int Index);
	vec2 GetPos(int Index);
	int GetTileIndex(int Index);
	int GetFTileIndex(int Index);
	int GetTileFlags(int Index);
	int GetFTileFlags(int Index);
	int IsTeleport(int Index);
	int IsEvilTeleport(int Index);
	int IsCheckTeleport(int Index);
	int IsCheckEvilTeleport(int Index);
	int IsTeleportWeapon(int Index);
	int IsTeleportHook(int Index);
	int IsTCheckpoint(int Index);
	int IsSpeedup(int Index);
	int IsTune(int Index);
	int IsTuneLock(int Index) const;
	void GetSpeedup(int Index, vec2* Dir, int* Force, int* MaxSpeed);
	int IsSwitch(int Index);
	int GetSwitchNumber(int Index);
	int GetSwitchDelay(int Index);

	int IsSolid(int x, int y);
	bool IsThrough(int x, int y, int xoff, int yoff, vec2 pos0, vec2 pos1);
	bool IsHookBlocker(int x, int y, vec2 pos0, vec2 pos1);
	int IsWallJump(int Index);
	int IsNoLaser(int x, int y);
	int IsFNoLaser(int x, int y);

	int IsCheckpoint(int Index);
	int IsFCheckpoint(int Index);

	int IsMover(int x, int y, int* pFlags);

	vec2 CpSpeed(int index, int Flags = 0);

	class CTeleTile* TeleLayer() { return m_pTele; }
	class CSwitchTile* SwitchLayer() { return m_pSwitch; }
	class CTuneTile* TuneLayer() { return m_pTune; }
	class CLayers* Layers() { return m_pLayers; }
	int m_HighestSwitchNumber;

	struct SSwitchers
	{
		bool m_Status[VANILLA_MAX_CLIENTS];
		bool m_Initial;
		int m_EndTick[VANILLA_MAX_CLIENTS];
		int m_Type[VANILLA_MAX_CLIENTS];
		int m_LastUpdateTick[VANILLA_MAX_CLIENTS];
		// F-DDrace
		int m_ClientID[VANILLA_MAX_CLIENTS];
		int m_StartTick[VANILLA_MAX_CLIENTS];
	};

	// F-DDrace
	class CConfig *m_pConfig;
	int IntersectLineFlagPickup(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision);
	int IntersectLinePortalRifleStop(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision);
	int IntersectLineNoBonus(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision, bool Enter);

	// redirect tiles
	struct SRedirectTile
	{
		int m_Number;
		vec2 m_Pos;
	};
	std::vector<SRedirectTile> m_vRedirectTiles;
	vec2 GetRandomRedirectTile(int Number);

	// tiles
	bool TileUsed(int Index) { return m_aTileUsed[Index]; }
	bool m_aTileUsed[NUM_INDICES];
	// When using GetRandomTile with tiles other than already used ones, add the index to CGameContext::FDDraceInit to aRequiredRandomTilePositions.
	vec2 GetRandomTile(int Index);
	std::vector< std::vector<vec2> > m_vTiles;

	int GetTileRaw(int x, int y);
	int GetTileRaw(vec2 Pos) { return GetTileRaw(Pos.x, Pos.y); }
	int GetFTileRaw(int x, int y);
	int GetFTileRaw(vec2 Pos) { return GetFTileRaw(Pos.x, Pos.y); }

	// IntersectLineDoor: returns -1 when intersected with a plot built laserwall, otherwise returns the number of the intersected laser wall
	int IntersectLineDoor(vec2 Pos0, vec2 Pos1, vec2* pOutCollision, vec2* pOutBeforeCollision, int Team, bool PlotDoorOnly, bool ClosedOnly = true);
	bool TestBoxDoor(vec2 Pos, vec2 Size, int Team, bool PlotDoorOnly, bool ClosedOnly = true);
	int CheckPointDoor(vec2 Pos, int Team, bool PlotDoorOnly, bool ClosedOnly);
	void UnsetDCollisionAt(float x, float y, int Number);

	// plots
	bool IsPlotTile(int Index);
	int GetPlotID(int Index);
	int GetSwitchByPlot(int PlotID);
	int GetPlotBySwitch(int SwitchID);
	int m_NumPlots;

	int m_aNumPlots[NUM_PLOT_SIZES];
	int *m_apPlotSize; // size: m_NumPlots

	int GetSwitchByPlotLaserDoor(int Plot, int Door);
	int GetNumPlotLaserDoors() { return m_aNumPlots[PLOT_SMALL] * PLOT_SMALL_MAX_DOORS + m_aNumPlots[PLOT_BIG] * PLOT_BIG_MAX_DOORS; }
	int GetNumMaxDoors(int PlotID);

	int GetNumAllSwitchers() { return !m_pSwitch ? 0 : m_HighestSwitchNumber + m_NumPlots + GetNumPlotLaserDoors() + GetNumFreeDrawDoors(); }
	int GetNumFreeDrawDoors() { return 255 - m_HighestSwitchNumber - m_NumPlots - GetNumPlotLaserDoors(); }

	bool IsPlotDoor(int Number) { return Number > m_HighestSwitchNumber && !IsPlotDrawDoor(Number); }
	bool IsPlotDrawDoor(int Number) { return Number > m_HighestSwitchNumber + m_NumPlots; }

	// doors
	bool AddDoorTile(int Index, int Type, int Number, int Flags = 0);
	bool RemoveDoorTile(int Index, int Type, int Number);
	int GetDoorIndex(int MapIndex, int Type, int Number);

	std::vector<int> GetButtonNumbers(int Index);
	bool IsFightBorder(vec2 Pos, int Fight);

	// Boxbig
	const float ms_MinStaticPhysSize = 30; // actually the smallest object right now is a map tile (32 x 32)
	bool TestBoxBig(vec2 Pos, vec2 Size);
	void MoveBoxBig(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity);
	bool IsBoxGrounded(vec2 Pos, vec2 Size);

	// speedups
	void SetSpeedup(vec2 Pos, int Angle, int Force, int MaxSpeed);

	// teleporters
	int m_NumTeleporters;
	//bool IsPlotDrawTele(int Number) { return Number > m_NumTeleporters; } // unused rn
	void SetTeleporter(vec2 Pos, int Type, int Number);

	int GetSwitchByPlotTeleporter(int Plot, int Tele);
	int GetNumPlotTeleporters() { return m_aNumPlots[PLOT_SMALL] * PLOT_SMALL_MAX_TELE + m_aNumPlots[PLOT_BIG] * PLOT_BIG_MAX_TELE; }
	int GetNumMaxTeleporters(int PlotID);

	//int GetNumAllTeleporters() { return m_NumTeleporters + GetNumPlotTeleporters() + GetNumFreeDrawTeleporters(); } // unused rn
	int GetNumFreeDrawTeleporters() { return 255 - m_NumTeleporters - GetNumPlotTeleporters(); }

	int IsTeleportTile(int Index);
	bool IsTeleportInOut(int Index);

	// access to plots: PlotID + m_NumSwitchers && PlotID < m_NumPlots + 1
	SSwitchers *m_pSwitchers;
};

void ThroughOffset(vec2 Pos0, vec2 Pos1, int* Ox, int* Oy);
#endif
