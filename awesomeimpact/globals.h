#pragma once
#include "Utils/il2cpp-api-types.h"
#include "Offsets.h"
#include <corecrt_math.h>
#include <d3d11.h>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>
#include "GUI/imgui.h"
#include "Utils/stb_image.h"
#include "Utils/ComPtr.h"
#include "Features/FeatureBase.h"

#define MoleMole_EvtCrash 64976
#define MoleMole_EvtBeingHit 37118

inline auto add = [](ImVec2 a, ImVec2 b) { return ImVec2(a.x + b.x, a.y + b.y); };
inline auto sub = [](ImVec2 a, ImVec2 b) { return ImVec2(a.x - b.x, a.y - b.y); };


template <typename T>
static T* FindFeature()
{
    for (auto& f : GetFeatureManager().All())
        if (auto* p = dynamic_cast<T*>(f.get()))
            return p;
    return nullptr;
}
struct Color {
    float r;
    float g;
    float b;
    float a;
};
struct Color32 {
    uint8_t r, g, b, a;
};
struct Quaternion {
    float x; 
    float y;  
    float z;  
    float w;
};
extern ID3D11ShaderResourceView* g_radiusSrv;
extern ComPtr<ID3D11Device> g_device;
extern int g_radiusTexW;
extern int g_radiusTexH;

inline uintptr_t g_GameBase = 0;
inline bool g_loginDoorOverlayActive = false;
inline constexpr int MoleMole_ActorManager = 49291;

struct Vector3 {
    float x = 0, y = 0, z = 0;
    Vector3() {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    static Vector3 Null() { return { 0, 0, 0 }; }
    float distance(Vector3 b) const { return sqrtf(powf(x - b.x, 2) + powf(y - b.y, 2) + powf(z - b.z, 2)); }
    float magnitude() const { return sqrtf(x * x + y * y + z * z); }
    bool  zero()      const { return x == 0 && y == 0 && z == 0; }
    Vector3 operator+(Vector3 b)  const { return { x + b.x, y + b.y, z + b.z }; }
    Vector3 operator-(Vector3 b)  const { return { x - b.x, y - b.y, z - b.z }; }
    Vector3 operator*(float k)    const { return { x * k,   y * k,   z * k }; }
    Vector3 operator*(Vector3 b)  const { return { x * b.x, y * b.y, z * b.z }; }
};


template <typename T>
struct Nullable {
    bool hasValue = false;
    T value{};
};

template <typename T>
inline Nullable<T> MakeNullable(const T& v)
{
    return Nullable<T>{ true, v };
}

template <typename T>
inline Nullable<T> NullNullable()
{
    return Nullable<T>{ false, T{} };
}

enum class OJJEPCGMLDD : int32_t
{
    Official = 0,
    Custom = 1,
    SubBlueprint = 2
};
struct Bounds
{
    Vector3 m_Center;
    Vector3 m_Extents;
};

struct NBGOLDDNIBO
{
    uint32_t BAOEGKOAIHK = 0;
    OJJEPCGMLDD AEKKEJJPAHA = OJJEPCGMLDD::Official;
};

struct OLOHHCPGJLF
{
    Nullable<NBGOLDDNIBO> OIEBIDPAAEN{};
};

struct ABKFGEMPBNN
{
    uint32_t handle = 0;
    void* proxy = nullptr;
};
struct GenericList { void* klass; void* monitor; void* items; int32_t size; int32_t version; };
inline Vector3 currentPlayerPos = { 0,0,0 };


struct Vector2 { float x, y; };
struct Vector4 {
    float x = 0, y = 0, z = 0, w = 0;

    Vector4() {}
    Vector4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w) {
    }

    static Vector4 Zero() { return { 0, 0, 0, 0 }; }

    Vector4 operator+(const Vector4& b) const { return { x + b.x, y + b.y, z + b.z, w + b.w }; }
    Vector4 operator-(const Vector4& b) const { return { x - b.x, y - b.y, z - b.z, w - b.w }; }
    Vector4 operator*(float k) const { return { x * k, y * k, z * k, w * k }; }


    float magnitude() const { return sqrtf(x * x + y * y + z * z + w * w); }

    Vector4 normalized() const {
        float mag = magnitude();
        return mag > 0 ? Vector4(x / mag, y / mag, z / mag, w / mag) : Vector4();
    }

    bool isZero() const { return x == 0 && y == 0 && z == 0 && w == 0; }
};
struct Rect {
    float x, y, width, height;
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}
};

enum class EnviroConfigColorType : int32_t
{
    Color = 0,
    Gradient = 1,
    None = 2
};

enum class EnviroConfigValueType : int32_t
{
    Value = 0,
    Curve = 1,
    None = 2
};

struct EnviroConfigColor
{
    EnviroConfigColorType colorType; 
    Color inputColor;                
    uint32_t _pad0;                  
    void* gradient;                  
};

struct EnviroConfigFloat
{
    EnviroConfigValueType valueType; 
    float inputValue;                
    void* curve;                     
    float minValue;                  
    float maxValue;                  
    float timeStart;                 
    float timeEnd;                   
    Color curveColor;                
};

struct EnviroSkySettingsConfig
{
    char _pad0[0x138];
};

struct EnviroSkySettings
{
    void* klass;                      
    void* monitor;                    
    int32_t profileNameHash;          
    int32_t _pad0;                    
    void* newStarSetting;             
    EnviroSkySettingsConfig config;   

    EnviroConfigColor topFrontColor;        
    EnviroConfigColor topBackColor;         
    EnviroConfigColor bottomFrontColor;     
    EnviroConfigColor bottomBackColor;      
    EnviroConfigFloat frontAndBackBlendFactor; 
    EnviroConfigFloat bottomColorHeight;    
    EnviroConfigFloat seaLevelHeight;       
    EnviroConfigColor horizonHaloColor;     
    EnviroConfigFloat horizonHaloIntensity; 
    EnviroConfigFloat horizonHaloHeight;    
    EnviroConfigFloat sunAlpha;             
    EnviroConfigFloat sunAngle;             
    EnviroConfigFloat sunInclination;       
    EnviroConfigFloat sunInclinationOffset; 
    EnviroConfigFloat celestialBodyMoveSpeed; 
    EnviroConfigColor sunDiskColor;         
    EnviroConfigFloat sunDiskBrightness;    
    EnviroConfigFloat sunDiskSize;          
    EnviroConfigFloat sunDiskSharpness;     
    EnviroConfigColor sunHaloColor;         
    EnviroConfigFloat sunHaloBrightness;    
    EnviroConfigFloat sunHaloSize;          
    EnviroConfigFloat moonAlpha;            
    EnviroConfigFloat moonAngle;            
    EnviroConfigFloat moonInclination;      
    EnviroConfigFloat moonInclinationOffset; 
    EnviroConfigFloat moonRotation;         
    EnviroConfigFloat moonOrbitOffset;      
    EnviroConfigColor moonColor;            
    EnviroConfigFloat moonBrightness;       
    EnviroConfigFloat moonSize;             
    EnviroConfigFloat moonLunarPhase;       
    EnviroConfigFloat moonGlowBrightness;   
    EnviroConfigFloat starsScintillation;   
    EnviroConfigFloat starsBrightness;      
    EnviroConfigFloat starsDensity;         
    EnviroConfigFloat newStarsBrightnessPower; 
    EnviroConfigFloat newStarsScintillation;   
    EnviroConfigFloat newStarsBrightness;      
    EnviroConfigFloat newStarsDensityValue;    
    bool enableSkySmooth;                  
    bool enableNewStar;                    
    char _pad1[0x6];                       
    EnviroConfigFloat auroraBrightness;    
    EnviroConfigFloat auroraSpeed;         
    bool skyLightEnable;                   
    char _pad2[0x7];                       
    EnviroConfigColor skyLightColor;       
    EnviroConfigColor skyLightGroundColor; 
    EnviroConfigFloat skyLightRatio;       
    EnviroConfigFloat skyLightInstensity;  
    EnviroConfigFloat ambientProbeGlobalMultiplier; 
    bool useLensFlare;                     
    char _pad3[0x7];                       
    void* lensFlareName;                   
};

struct EnviroLightSettings
{
    void* klass;                        
    void* monitor;                      
    int32_t profileNameHash;            
    int32_t _pad0;                      
    void* lightIntensityTransSetting;   
    void* mainLightClipPlaneSetting;    

    char _pad1[0x2E0];                  
    float skyIslandAlpha;               
    EnviroConfigColor mainLightColor;   
    EnviroConfigFloat mainLightIntensity;

    char _pad2[0x38];                  
    EnviroConfigFloat mainLightSpecularWrap;
    EnviroConfigFloat farLightIntensity;    

    char _pad3[0x1C8];   
    EnviroConfigFloat shadowStrength; 

    char _pad4[0x20];                   
    int32_t shadowDistanceRunTime;     

    char _pad5[0xB4];               
    EnviroConfigColor ambientSkyColor;     
    EnviroConfigColor ambientEquatorColor;  
    EnviroConfigColor ambientGroundColor;  
    EnviroConfigFloat ambientRotate;        

    char _pad6[0x98];                 
    EnviroConfigFloat hbaoStrength;     
    EnviroConfigFloat hbaoMaximum;     

    
    float lightClampThreshold;             
    float baseSunDawnThreshold;           
    float dayNightLerpSpeed;               
    float transparentEnvironmentIntensity;  
    Color mobileLODReflectionColor;         
    EnviroConfigColor mobileCubemapTintColor; 

    EnviroConfigFloat postThreshold;            
    EnviroConfigFloat postThresholdCharacter;   
    EnviroConfigFloat postProcessExposure;     
    EnviroConfigFloat postProcessContrast;     
    EnviroConfigFloat postScaler;              

    float autoExposureScale;               
    char _pad9[0x4];                       
    EnviroConfigFloat autoExposureMinEV;   
    EnviroConfigFloat autoExposureMaxEV;   
    EnviroConfigFloat autoExposureCompensation; 

    char _pad10[0x3A0];                    
    EnviroConfigFloat vegetationTransitionLuminance;
    EnviroConfigColor waterShallowColor;           
    EnviroConfigFloat waterShallowIntensity;       
    EnviroConfigColor waterDeepColor;              
    EnviroConfigFloat waterDeepIntensity;          
    EnviroConfigFloat waterReflectionIntensity;    
    EnviroConfigFloat grassSpecularScale;          
    EnviroConfigFloat grassTransmitScale;         

    char _pad11[0x80];                    
    EnviroConfigFloat lyraGIEnable;        
};

struct EnviroComponents
{
    void* klass;               
    void* monitor;             
    void* WeatherSimulation;   
    void* AvatarLight;         
    void* Atmosphere;          
    void* Sun;                 
    void* Moon;                
    void* Clouds;              
    void* CloudsEmitter;       
    void* CloudsEmitter1;      
    void* CloudsEmitter2;      
    void* CloadShadow;         
    void* DirectLight;         
    void* HugePointLight;      
    void* EwindZone;           
    void* Galaxy;              
    void* Aurora;              
    void* ShakeObj;            
    void* MonoPrefabRoot;      
    void* FlareParticle;       
};

struct EnviroSky
{
    void* klass;                       
    void* monitor;                      
    char _pad0[0x58];                  
    EnviroComponents* Components;       
    char _pad1[0x80];                   
    EnviroLightSettings* lightSettings; 
    EnviroSkySettings* skySettings;     
    void* cloudsSettings;              
    void* fogSettings;                  
    void* transitionSettings;           
    void* customParamsSettings;         
};

struct EnviroCloudSettingsConfig
{
    char _pad0[0xF4];
};

struct EnviroCloudSettings
{
    void* klass;                          
    void* monitor;                        
    int32_t profileNameHash;              
    EnviroCloudSettingsConfig config;     
    EnviroConfigFloat cloudTiling;        
    EnviroConfigFloat cloudHeight;        
    EnviroConfigFloat cloudOpacity;       
    EnviroConfigColor cloudLightFrontColor; 
    EnviroConfigColor cloudLightBackColor;  
    EnviroConfigColor cloudDarkFrontColor;  
    EnviroConfigColor cloudDarkBackColor;   
    EnviroConfigColor effCloudLightFrontColorConfig; 
    EnviroConfigColor effCloudLightBackColorConfig;  
    EnviroConfigColor effCloudDarkFrontColorConfig;  
    EnviroConfigColor effCloudDarkBackColorConfig;   
    EnviroConfigFloat frontAndBackBlendFactor; 
    EnviroConfigFloat cloudSunBrightenIntensity; 
    EnviroConfigFloat cloudLightingIntensity; 
    EnviroConfigFloat cloudRimLightRadius;   
    EnviroConfigFloat cloudWispsCoverage;    
    EnviroConfigFloat cloudWispsOpacity;     
    EnviroConfigColor cloudLitToneColorConfig;  
    EnviroConfigColor cloudRimToneColorConfig;  
    EnviroConfigColor cloudDarkToneColorConfig; 
    EnviroConfigColor cloudBackLitColorConfig;  
    EnviroConfigFloat cloudDensityConfig;    
    EnviroConfigFloat cloudFadeConfig;       
    EnviroConfigFloat cloudFogEffect;        
    bool cloudsWispsToggle;                  
    bool cloudsEmitterToggle;                
    bool cloudsEmitter1Toggle;               
    bool cloudsEmitter2Toggle;               
};

struct MoveSyncTask {
    struct Vector3 position;
    struct Vector3 velocity;
    struct Vector3 forward;
    struct Vector3 up;
    struct Quaternion rotation;
    int32_t motion;
    int32_t paramNumber;
    struct Vector3 param0;
    struct Vector3 param1;
    struct Vector3 param2;
    struct Vector3 param3;
    uint32_t time;
    uint32_t reliableSeq;
    uint32_t clientSequenceId;
    uint32_t subSequenceId;
    bool reliable;
    bool isFake;
    struct Vector3 refPosition;
    uint32_t refEntityId;
    uint32_t refEntityTimestamp;
    bool hasValue;
    struct Vector3 positionRS;
    struct Vector3 positionRSLocal;
    uint32_t randomDelay;
    int32_t frameCountWhenAddTask;
};

struct VCHumanoidMoveData {
    char _pad0[0x370];
    uint8_t tryDoJump;             
    char _pad1[0xD];               
    uint8_t tryFlyUp;               
    char _pad2[0x1];                
    uint8_t tryFlyDown;            
    char _pad3[0x23];               
    uint8_t forceAirStateFly;       
    char _pad4[0x3];                
    float jumpXZVelocityScale;      
    char _pad5[0x14C];              
    void* vcHumanoidMove;          
};

enum EntityType
{
    None = 0,
    Avatar = 1,
    Monster = 2,
    Bullet = 3,
    AttackPhyisicalUnit = 4,
    AOE = 5,
    Camera = 6,
    EnviroArea = 7,
    Equip = 8,
    MonsterEquip = 9,
    Grass = 10,
    Level = 11,
    NPC = 12,
    TransPointFirst = 13,
    TransPointFirstGadget = 14,
    TransPointSecond = 15,
    TransPointSecondGadget = 16,
    DropItem = 17,
    Field = 18,
    Gadget = 19,
    Water = 20,
    GatherPoint = 21,
    GatherObject = 22,
    AirflowField = 23,
    SpeedupField = 24,
    Gear = 25,
    Chest = 26,
    EnergyBall = 27,
    ElemCrystal = 28,
    Timeline = 29,
    Worktop = 30,
    Team = 31,
    Platform = 32,
    AmberWind = 33,
    EnvAnimal = 34,
    SealGadget = 35,
    Tree = 36,
    Bush = 37,
    QuestGadget = 38,
    Lightning = 39,
    RewardPoint = 40,
    RewardStatue = 41,
    MPLevel = 42,
    WindSeed = 43,
    MpPlayRewardPoint = 44,
    ViewPoint = 45,
    RemoteAvatar = 46,
    GeneralRewardPoint = 47,
    PlayTeam = 48,
    OfferingGadget = 49,
    EyePoint = 50,
    MiracleRing = 51,
    Foundation = 52,
    WidgetGadget = 53,
    Vehicle = 54,
    DangerZone = 55,
    EchoShell = 56,
    HomeGatherObject = 57,
    Projector = 58,
    Screen = 59,
    CustomTile = 60,
    FishPool = 61,
    FishRod = 62,
    CustomGadget = 63,
    RoguelikeOperatorGadget = 64,
    ActivityInteractGadget = 65,
    BlackMud = 66,
    SubEquip = 67,
    UIInteractGadget = 68,
    NightCrowGadget = 69,
    Partner = 70,
    DeshretObeliskGadget = 71,
    CoinCollectLevelGadget = 72,
    UgcSpecialGadget = 73,
    UgcTowerLevelUpGadget = 74,
    JourneyGearOperatorGadget = 75,
    CurveMoveGadget = 76,
    MagnetPlant = 77,
    PlaceHolder = 99
};

enum PropType : uint32_t {
    PROP_NONE = 0,
    PROP_EXP = 1001,
    PROP_BREAK_LEVEL = 1002,
    PROP_SATIATION_VAL = 1003,
    PROP_SATIATION_PENALTY_TIME = 1004,
    PROP_GEAR_START_VAL = 2001,
    PROP_GEAR_STOP_VAL = 2002,
    PROP_LEVEL = 4001,
    PROP_LAST_CHANGE_AVATAR_TIME = 10001,
    PROP_MAX_SPRING_VOLUME = 10002,
    PROP_CUR_SPRING_VOLUME = 10003,
    PROP_IS_SPRING_AUTO_USE = 10004,
    PROP_SPRING_AUTO_USE_PERCENT = 10005,
    PROP_IS_FLYABLE = 10006,
    PROP_IS_WEATHER_LOCKED = 10007,
    PROP_IS_GAME_TIME_LOCKED = 10008,
    PROP_IS_TRANSFERABLE = 10009,
    PROP_MAX_STAMINA = 10010,
    PROP_CUR_PERSIST_STAMINA = 10011,
    PROP_CUR_TEMPORARY_STAMINA = 10012,
    PROP_PLAYER_LEVEL = 10013,
    PROP_PLAYER_EXP = 10014,
    PROP_PLAYER_HCOIN = 10015,
    PROP_PLAYER_SCOIN = 10016,
    PROP_PLAYER_MP_SETTING_TYPE = 10017,
    PROP_IS_MP_MODE_AVAILABLE = 10018,
    PROP_PLAYER_WORLD_LEVEL = 10019,
    PROP_PLAYER_RESIN = 10020,
    PROP_PLAYER_WAIT_SUB_HCOIN = 10022,
    PROP_PLAYER_WAIT_SUB_SCOIN = 10023,
    PROP_IS_ONLY_MP_WITH_PS_PLAYER = 10024,
    PROP_PLAYER_MCOIN = 10025,
    PROP_PLAYER_WAIT_SUB_MCOIN = 10026,
    PROP_PLAYER_LEGENDARY_KEY = 10027,
    PROP_IS_HAS_FIRST_SHARE = 10028,
    PROP_PLAYER_FORGE_POINT = 10029,
    PROP_CUR_CLIMATE_METER = 10035,
    PROP_CUR_CLIMATE_TYPE = 10036,
    PROP_CUR_CLIMATE_AREA_ID = 10037,
    PROP_CUR_CLIMATE_AREA_CLIMATE_TYPE = 10038,
    PROP_PLAYER_WORLD_LEVEL_LIMIT = 10039,
    PROP_PLAYER_WORLD_LEVEL_ADJUST_CD = 10040,
    PROP_PLAYER_LEGENDARY_DAILY_TASK_NUM = 10041,
    PROP_PLAYER_HOME_COIN = 10042,
    PROP_PLAYER_WAIT_SUB_HOME_COIN = 10043,
    PROP_IS_AUTO_UNLOCK_SPECIFIC_EQUIP = 10044
};

enum class FSMStateID : int32_t {
    Move = 0,
    TurnDirection = 1,
    FallOnGround = 2,
    GoUpstairs = 3,
    JumpUpWallReady = 4,
    Climb = 5,
    JumpUpWallForStandby = 6,
    StandbyToClimb = 7,
    Jump = 8,
    Drop = 9,
    Fly = 10,
    CombatMove = 11,
    CombatFallOnGround = 12,
    CombatAir = 13,
    Swim = 14,
    SwimJump = 15,
    Ladder = 16,
    FlyGateLoading = 17,
    Crouch = 18,
    Perform = 19,
    FlyFollowRoute = 20,
    Null = 21,
    Vehicle = 22,
    Skiff = 23
};
enum class MoveConfigFlag : int32_t {
    Climb = 0,
    FlyGroundDistCheck = 1,
    EnableLand = 2,
    Frozen = 3,
    EnableFly = 4,
    EnableJump = 5,
    EnableSprint = 6,
    EnableWalkAndRun = 7
};

enum MotionState {
    MotionNone = 0,
    MotionReset = 1,
    MotionStandby = 2,
    MotionStandbyMove = 3,
    MotionWalk = 4,
    MotionRun = 5,
    MotionDash = 6,
    MotionClimb = 7,
    MotionClimbJump = 8,
    MotionStandbyToClimb = 9,
    MotionFight = 10,
    MotionJump = 11,
    MotionDrop = 12,
    MotionFly = 13,
    MotionSwimMove = 14,
    MotionSwimIdle = 15,
    MotionSwimDash = 16,
    MotionSwimJump = 17,
    MotionSlip = 18,
    MotionGoUpstairs = 19,
    MotionFallOnGround = 20,
    MotionJumpUpWallForStandby = 21,
    MotionJumpOffWall = 22,
    MotionPoweredFly = 23,
    MotionLadderIdle = 24,
    MotionLadderMove = 25,
    MotionLadderSlip = 26,
    MotionStandbyToLadder = 27,
    MotionLadderToStandby = 28,
    MotionDangerStandby = 29,
    MotionDangerStandbyMove = 30,
    MotionDangerWalk = 31,
    MotionDangerRun = 32,
    MotionDangerDash = 33,
    MotionCrouchIdle = 34,
    MotionCrouchMove = 35,
    MotionCrouchRoll = 36,
    MotionNotify = 37,
    MotionLandSpeed = 38,
    MotionMoveFailAck = 39,
    MotionWaterfall = 40,
    MotionDashBeforeShake = 41,
    MotionSitIdle = 42,
    MotionForceSetPos = 43,
    MotionQuestForceDrag = 44,
    MotionFollowRoute = 45,
    MotionSkiffBoarding = 46,
    MotionSkiffNormal = 47,
    MotionSkiffDash = 48,
    MotionSkiffPoweredDash = 49,
    MotionDestroyVehicle = 50,
    MotionFlyIdle = 51,
    MotionFlySlow = 52,
    MotionFlyFast = 53,
    MotionAimMove = 54,
    MotionAirCompensation = 55,
    MotionSorushNormal = 56,
    MotionRollerCoaster = 57,
    MotionDiveIdle = 58,
    MotionDiveMove = 59,
    MotionDiveDash = 60,
    MotionDiveDolphine = 61,
    MotionDebug = 62,
    MotionOceanCurrent = 63,
    MotionDiveSwimMove = 64,
    MotionDiveSwimIdle = 65,
    MotionDiveSwimDash = 66,
    MotionArcLight = 67,
    MotionArcLightSafe = 68,
    MotionVehicleStandby = 69,
    MotionVehicleRun = 70,
    MotionVehicleDash = 71,
    MotionVehicleClimb = 72,
    MotionVehicleClimbJump = 73,
    MotionVehicleStandbyToClimb = 74,
    MotionVehicleFight = 75,
    MotionVehicleJump = 76,
    MotionVehicleDrop = 77,
    MotionVehicleFly = 78,
    MotionVehicleSwimMove = 79,
    MotionVehicleSwimIdle = 80,
    MotionVehicleSwimDash = 81,
    MotionVehicleSlip = 82,
    MotionVehicleGoUpstairs = 83,
    MotionVehicleFallOnGround = 84,
    MotionVehicleJumpOffWall = 85,
    MotionVehiclePoweredFly = 86,
    MotionVehicleDangerStandby = 87,
    MotionVehicleDangerRun = 88,
    MotionVehicleDangerDash = 89,
    MotionVehicleNotify = 90,
    MotionVehicleLandSpeed = 91,
    MotionVehicleDashBeforeShake = 92,
    MotionVehicleQuestForceDrag = 93,
    MotionVehicleFollowRoute = 94,
    MotionVehicleFlyIdle = 95,
    MotionVehicleFlySlow = 96,
    MotionVehicleFlyFast = 97,
    MotionVehicleAirCompensation = 98,
    MotionVehicleArcLight = 99,
    MotionVehicleArcLightSafe = 100,
    MotionVehicleDangerSwimMove = 101,
    MotionVehicleDangerSwimIdle = 102,
    MotionVehicleDangerSwimDash = 103,
    MotionFollowCurveRoute = 104,
    MotionVehicleFollowCurveRoute = 105,
    MotionNatsaurusNormal = 106,
    MotionNatsaurusEntering = 107,
    MotionMaglev = 108,
    MotionMaglevSafe = 109,
    MotionNum = 110
};

enum GadgetState
{
    Default = 0,
    GatherDrop = 1,
    ChestLocked = 101,
    ChestOpened = 102,
    ChestTrap = 103,
    ChestBramble = 104,
    ChestFrozen = 105,
    ChestRock = 106,
    GearStart = 201,
    GearStop = 202,
    GearAction1 = 203,
    GearAction2 = 204,
    CrystalResonate1 = 301,
    CrystalResonate2 = 302,
    CrystalExplode = 303,
    CrystalDrain = 304,
    StatueActive = 401,
    Action01 = 901,
    Action02 = 902,
    Action03 = 903,

};

enum class HumanBodyBones : int {
    Hips = 0,
    LeftUpperLeg = 1,
    RightUpperLeg = 2,
    LeftLowerLeg = 3,
    RightLowerLeg = 4,
    LeftFoot = 5,
    RightFoot = 6,
    Spine = 7,
    Chest = 8,
    Neck = 9,
    Head = 10,
    LeftShoulder = 11,
    RightShoulder = 12,
    LeftUpperArm = 13,
    RightUpperArm = 14,
    LeftLowerArm = 15,
    RightLowerArm = 16,
    LeftHand = 17,
    RightHand = 18,
    LeftToes = 19,
    RightToes = 20,
    LeftEye = 21,
    RightEye = 22,
    Jaw = 23,
    LeftThumbProximal = 24,
    LeftThumbIntermediate = 25,
    LeftThumbDistal = 26,
    LeftIndexProximal = 27,
    LeftIndexIntermediate = 28,
    LeftIndexDistal = 29,
    LeftMiddleProximal = 30,
    LeftMiddleIntermediate = 31,
    LeftMiddleDistal = 32,
    LeftRingProximal = 33,
    LeftRingIntermediate = 34,
    LeftRingDistal = 35,
    LeftLittleProximal = 36,
    LeftLittleIntermediate = 37,
    LeftLittleDistal = 38,
    RightThumbProximal = 39,
    RightThumbIntermediate = 40,
    RightThumbDistal = 41,
    RightIndexProximal = 42,
    RightIndexIntermediate = 43,
    RightIndexDistal = 44,
    RightMiddleProximal = 45,
    RightMiddleIntermediate = 46,
    RightMiddleDistal = 47,
    RightRingProximal = 48,
    RightRingIntermediate = 49,
    RightRingDistal = 50,
    RightLittleProximal = 51,
    RightLittleIntermediate = 52,
    RightLittleDistal = 53,
    UpperChest = 54,
    LastBone = 55
};

enum PrimitiveType
{

    Sphere,
    Capsule,
    Cylinder,
    Cube,
    Plane,
    Quad,
};

enum class FightPropType : int {
    NONE = 0,
    BASE_HP = 1, HP = 2, HP_PERCENT = 3,
    BASE_ATTACK = 4, ATTACK = 5, ATTACK_PERCENT = 6,
    BASE_DEFENSE = 7, DEFENSE = 8, DEFENSE_PERCENT = 9,
    BASE_SPEED = 10, SPEED_PERCENT = 11,
    HP_MP_PERCENT = 12, ATTACK_MP_PERCENT = 13,
    CRITICAL = 20, ANTI_CRITICAL = 21, CRITICAL_HURT = 22,
    CHARGE_EFFICIENCY = 23, ADD_HURT = 24, SUB_HURT = 25,
    HEAL_ADD = 26, HEALED_ADD = 27, ELEMENT_MASTERY = 28,
    PHYSICAL_SUB_HURT = 29, PHYSICAL_ADD_HURT = 30,
    DEFENCE_IGNORE_RATIO = 31, DEFENCE_IGNORE_DELTA = 32,
    FIRE_ADD_HURT = 40, ELEC_ADD_HURT = 41, WATER_ADD_HURT = 42,
    GRASS_ADD_HURT = 43, WIND_ADD_HURT = 44, ROCK_ADD_HURT = 45,
    ICE_ADD_HURT = 46, HIT_HEAD_ADD_HURT = 47,
    FIRE_SUB_HURT = 50, ELEC_SUB_HURT = 51, WATER_SUB_HURT = 52,
    GRASS_SUB_HURT = 53, WIND_SUB_HURT = 54, ROCK_SUB_HURT = 55,
    ICE_SUB_HURT = 56,
    EFFECT_HIT = 60, EFFECT_RESIST = 61,
    FREEZE_RESIST = 62, DIZZY_RESIST = 64,
    FREEZE_SHORTEN = 65, DIZZY_SHORTEN = 67,
    MAX_FIRE_ENERGY = 70, MAX_ELEC_ENERGY = 71, MAX_WATER_ENERGY = 72,
    MAX_GRASS_ENERGY = 73, MAX_WIND_ENERGY = 74, MAX_ICE_ENERGY = 75,
    MAX_ROCK_ENERGY = 76,
    SKILL_CD_MINUS_RATIO = 80, SHIELD_COST_MINUS_RATIO = 81,
    CUR_FIRE_ENERGY = 1000, CUR_ELEC_ENERGY = 1001, CUR_WATER_ENERGY = 1002,
    CUR_GRASS_ENERGY = 1003, CUR_WIND_ENERGY = 1004, CUR_ICE_ENERGY = 1005,
    CUR_ROCK_ENERGY = 1006,
    CUR_HP = 1010,
    MAX_HP = 2000, CUR_ATTACK = 2001, CUR_DEFENSE = 2002, CUR_SPEED = 2003,
    NONEXTRA_ATTACK = 3000, NONEXTRA_DEFENSE = 3001,
    NONEXTRA_CRITICAL = 3002, NONEXTRA_ANTI_CRITICAL = 3003,
    NONEXTRA_CRITICAL_HURT = 3004, NONEXTRA_CHARGE_EFFICIENCY = 3005,
    NONEXTRA_ELEMENT_MASTERY = 3006, NONEXTRA_PHYSICAL_SUB_HURT = 3007,
    NONEXTRA_FIRE_ADD_HURT = 3008, NONEXTRA_ELEC_ADD_HURT = 3009,
    NONEXTRA_WATER_ADD_HURT = 3010, NONEXTRA_GRASS_ADD_HURT = 3011,
    NONEXTRA_WIND_ADD_HURT = 3012, NONEXTRA_ROCK_ADD_HURT = 3013,
    NONEXTRA_ICE_ADD_HURT = 3014,
    NONEXTRA_FIRE_SUB_HURT = 3015, NONEXTRA_ELEC_SUB_HURT = 3016,
    NONEXTRA_WATER_SUB_HURT = 3017, NONEXTRA_GRASS_SUB_HURT = 3018,
    NONEXTRA_WIND_SUB_HURT = 3019, NONEXTRA_ROCK_SUB_HURT = 3020,
    NONEXTRA_ICE_SUB_HURT = 3021,
    NONEXTRA_SKILL_CD_MINUS_RATIO = 3022, NONEXTRA_SHIELD_COST_MINUS_RATIO = 3023,
    NONEXTRA_PHYSICAL_ADD_HURT = 3024,
};

enum class MonoLoginSceneState : int32_t {
    Move = 0,
    TryToStop = 1,
    BeforeOpen = 2,
    AfterOpen = 3,
    Finish = 4,
};

enum class ElementType : int32_t
{
    None = 0,
    Fire = 1,
    Water = 2,
    Grass = 3,
    Electric = 4,
    Ice = 5,
    Frozen = 6,
    Wind = 7,
    Rock = 8,
    AntiFire = 9,
    VehicleMuteIce = 10,
    Mushroom = 11,
    Overdose = 12,
    Wood = 13,
    LiquidPhlogiston = 14,
    SolidPhlogiston = 15,
    SolidifyPhlogiston = 16,
    Count = 17
};

enum class AbilityState : uint64_t {
    None = 0ull,
    Invincible = 1ull << 0,
    DenyLockOn = 1ull << 1,
    IsGhostToAllied = 1ull << 2,
    IsGhostToEnemy = 1ull << 3,
    ElementBurning = 1ull << 4,
    ElementWet = 1ull << 5,
    ElementFrozen = 1ull << 7,
    LockHP = 1ull << 9,
    IgnoreTriggerBullet = 1ull << 10,
    ElementShock = 1ull << 11,
    Struggle = 1ull << 12,
    ElementRock = 1ull << 13,
    ElementIce = 1ull << 15,
    ElementFreeze = 1ull << 16,
    AttackUp = 1ull << 17,
    DefenseUp = 1ull << 18,
    SpeedUp = 1ull << 19,
    DefenseDown = 1ull << 20,
    SpeedDown = 1ull << 21,
    ElementWind = 1ull << 22,
    ElementElectric = 1ull << 23,
    ElementFire = 1ull << 24,
    NoHeal = 1ull << 25,
    Limbo = 1ull << 26,
    MuteTaunt = 1ull << 27,
    ElementPetrifaction = 1ull << 28,
    IgnoreAddEnergy = 1ull << 30,
    ElementGrass = 1ull << 31,
    ElementOverdose = 1ull << 32,
    Corruption = 1ull << 33,
    UnlockFrequencyLimit = 1ull << 34,
    ElementDeadTime = 1ull << 35,
    OvergrowVariation = 1ull << 36,
    NyxState = 1ull << 37,
    BeyondMuteTaunt = 1ull << 38
};

struct MonoLoginScene {
    void* klass;                                
    void* monitor;                              
    uint8_t _pad10[0x8];                        

    MonoLoginSceneState curState;               
    bool disable24Time;                         
    uint8_t _pad1D[0x3];                        
    float animatorSpeed;                        

    void* sceneNode;                            
    void* bridgeNode;                           
    void* doorNode;                             
    void* cloudNode;                            
    void* lightShaftNode;                       
    float moveSpeed1;                           
    float moveSpeed2;                           
    void* moveSpeed2Curve;                      
    float moveSpeed3;                           
    void* moveSpeed3Curve;                      
    float resetOffsetLen;                       
    uint8_t _pad74[0x4];                        
    void* sceneObj;                             
    int32_t sceneSize;                          
    float sceneLength;                          
    void* bridgeObj;                            
    int32_t bridgeSize;                         
    float bridgeLength;                         
    void* cloudObj;                             
    int32_t cloudSize;                          
    float cloudLength;                          
    void* cameraEffecs;                         
    void* doorPrefab;                           
    void* cameraPrefab;                         
    void* cameraTrans;                          
    void* enviroSky;                            
    float showDoorOnBridgeLength;               
    float stopBeforeDoor1;                      
    float stopBeforeDoor2;                      
    float pressAfterLiftDoorTime;               
    float connectAfterDoor2StageTime;           
    float whiteScreenExpro;                     
    float transWeatherTime;                     
    float hourOfDayLengthInMinutes;             
    float oneDayTimeIntervals;                  
    uint8_t _padF4[0x4];                        
    float oneDayTargetTimes;                    
    uint8_t _padFC[0x4];                        
    void* oneDayTargetLightShafts;              
    void* oneDayTargetCameraHalo;               
    float lightShaftLength;                     
    float internalHour;                         
    int32_t curTargetTimeIndex;                 
    int32_t lastTargetTimeIndex;                
    float showHour;                             
    float animationLen;                         
    float showDoorOnLoadProgress;               
    float showDoorOnPS4LoadProgress;            
    float focalDistance;                        
    float focalRange;                           
    float dofBlur;                              
    float nearDistance;                         
    float nearTrans;                            
    float curtainDelayTime;                     
    float curtainDurationTime;                  
    float blurLerpSpeed;                        
    float shadowDistance;                       
    Vector4 shadowDistanceVec;                    
    Vector3 shadowSplitVec;                       
    int32_t shadowQuality;                      
    float bridgetEmissionAfterTime;             
    float lightValue;                           
    float beginHeight;                          
    float lightSpeed1;                          
    float lightSpeed2;                          
    float ECPLPDONIJN;                          
    float DHFDPDANCAD;                          
    int32_t EIEHFPOKOEO;                        
    float IHEAMACPPAP;                          
    int32_t ABDMMBBLOLI;                        
    float NNHKJEPDIHF;                          
    int32_t IBPABNHOHEN;                        
    float IMFNFMBNEPI;                          
    int32_t NDCOLFMOEPG;                        
    float KBBHBLBNLOC;                          
    Vector3 GIIOMIDJKGP;                          
    Vector3 LFKBPGHLEJC;                          
    void* OLNEKNHGMJF;                          
    void* MJHKCFADDBP;                          
    void* ILGOAPPGNKE;                          
    void* DONKADELCAA;                          
    float CEMDLAIKKKG;                          
    float IAJMMPJJOMF;                          
    float ILPKNHMGEMJ;                          
    float KHHKJECGMGJ;                          
    float BDCIHGJJOIP;                          
    uint8_t _pad1FC[0x4];                       
    void* DLBCEKOILLL;                          
    void* NFNJNJMJFLH;                          
    void* EMGOCOFBGGD;                          
    void* PCKILPBGACK;                          
    void* ANBDNDLOCOG;                          
    void* BBGFANKLGAA;                          
    void* FHNBNNOFEGJ;                          
    float DCFEPFIFAHK;                          
    float EONEMGHOHBD;                          
    void* MDBLCKOKAFN;                          
    void* EADFCBNCHIP;                          
    bool BNIAPBNGLKB;                           
    uint8_t _pad251[0x7];                       
};

struct MonoLoadingCanvas {
    void* klass;            
    void* monitor;          

    void* m_CachedPtr;      

    void* _tipsText;        
    void* _progressBar;     
    void* _bgImg;           
    void* _animator;        
    void* _txtTipsTitle;    
    void* _txtTipsContent;  
    void* _btnNext;         
    void* _bgIcon;          
    void* _headphoneTip;    

    float dummyLoadingStartProgress;       
    float dummyLoadingStartDelayS;         
    float dummyLoadingMinProgress;         
    float dummyLoadingMaxProgress;         
    float dummyLoadingStepMinProgress;     
    float dummyLoadingStepMaxProgress;     
    float dummyLoadingStepMinIntervalS;    
    float dummyLoadingStepMaxIntervalS;    
};

struct ConfirmWithJoypad
{
    void* klass;    
    void* monitor;  

    uint8_t _pad10[0x48];

    void* strContent;         
    void* strAcceptTitle;     
    void* strCancelTitle;     
    void* OnConfirmResult;
    int32_t result;      
    uint8_t _pad7C[0x4];  

    void* dialogObject;           
    void* cancelButton;           
    void* acceptButton;           
    void* contentText;            
    void* acceptText;             
    void* cancelText;             
    void* cancelButtonJoypadIcon; 
    void* acceptButtonJoypadIcon; 
    void* m_joypadType;           

    uint8_t m_bJoypadExchange;    
    uint8_t m_bIsJoypadSignal;    
    uint8_t _padCA[0x6];       
};

struct NullableUInt32 {
    uint32_t value;     
    uint8_t  hasValue;  
    uint8_t  pad[3];    
};
static_assert(sizeof(NullableUInt32) == 0x8);

struct GeneralMarkData {
    uint8_t  objHeader[0x10];          

    uint32_t sceneID;                  
    uint32_t markType;                 
    uint32_t iconType;                 
    uint32_t markID;                   
    float    radius;                   
    float    sectorAngle;              
    float    sectorStartAngle;         
    Vector3  originPosition;           
    NullableUInt32 worldAreaID;        
    NullableUInt32 subAreaID;          
    Vector3  areaOffset;               
    Vector3  indicatorPositionOffset;  
    uintptr_t questIndicatorIconName;  
    bool     hideOnMapAndRadar;        
    uint8_t  pad69[0x3];               
    uint32_t groupId;                  

    uint8_t  pad70[0x10];              

    uintptr_t entity;                  
    bool      hideOnMove;              
    bool      hideTrace;               
    uint8_t   pad8A[0x2];              
    uint32_t  questSceneId;            
    uintptr_t mapMarkPoint;            
    bool      hideWhenAreaLocked;      
    bool      deleteStopTrack;         
    bool      mapLayerDirty;           
    uint8_t   pad9B[0x1];              
    uint32_t  mapLayerID;              
    uintptr_t monoMarkListenList;      
    bool      mapLayerLoading;         
    bool      positionDirty;           
    uint8_t   padAA[0x6];              
    uintptr_t onMapLayerGet;           
    uint64_t  hideState;               
    bool      isUseGuidePos;           
    uint8_t   padC1[0x3];              
    Vector3   guidePosition;           
    Vector3   guideAreaOffset;         
    uint32_t  guideMapLayerID;         
};
static_assert(offsetof(GeneralMarkData, originPosition) == 0x2C);

namespace UnityEngine {
    struct Transform;
    struct Camera;
}
struct CinemachineBlendDefinitionRaw {
    uint8_t data[0x10]; 
};
struct CinemachineBrain {
    uint8_t _base_0x00[0x18];              

    uint8_t m_SelfUpdate;                        
    uint8_t m_ShowDebugText;                     
    uint8_t m_ShowCameraFrustum;                 
    uint8_t m_IgnoreTimeScale;                   

    uint8_t _pad_0x1C[0x4];                      
    UnityEngine::Transform* m_WorldUpOverride;   

    int32_t m_UpdateMethod;                      
    uint8_t _pad_0x2C[0x4];                      

    CinemachineBlendDefinitionRaw m_DefaultBlend;
    void* m_CustomBlends;                        
    UnityEngine::Camera* m_OutputCamera;         
};

struct MonoCombatBillboard
{
    uint8_t _base_0x00[0x18]; 

    void* _titleGrp;      
    void* _nameText;      
    void* _onlineIdText;  

    uint8_t _pad_0x30[0x8];

    void* _combatBars;    

    uint8_t _pad_0x40[0x28];

    void* _hpBar;         
};

struct MonoEffect
{
    uint8_t _pad0[0x78];

    Il2CppString* effectName;   

    uint8_t _pad1[0x48];

    void* owner;                

    uint8_t _pad2[0x70];

    void* trailRenderers;       
};

struct MonoInLevelPlayerProfilePageV3
{
    uint8_t _base_0x00[0x48];

    void* _playerName;        
    void* _playerLv;          
    void* _playerExp;         
    void* _playerExpSlider;   
    void* _playerID;          
    void* _playerCost;        
    void* _playerInfo;        
    void* _playerNum;         
    void* playerIconImage;    
    void* _playerWorldLv;     
    void* _playerBirthday;    
    void* _playerSignature;   
    void* _playerNoSignature; 

    uint8_t _pad_0xB0[0x8];

    void* _nameCardPic;       
};

struct MonoTalkDialog
{
    void* klass;    
    void* monitor;  

    std::uint8_t _pad0[0x80];     

    float _protectTime;            
    std::uint32_t _unk94;          
    float _optionsDisplayDelay;    
    float _waitDialogSelectTime;   
    float _waitCoopSelectTime;     
};

struct MonoGrpConversation
{
    void* klass;    
    void* monitor;  

    std::uint8_t _pad0[0x18]; 

    void* _typewriter;     
};

struct LCBaseIntee
{
    void* klass;    
    void* monitor;  

    std::uint8_t _pad0[0x1CD];      
    std::uint8_t _isInterDisable;   
    std::uint8_t _isLuaInterDisable;

    std::uint8_t _pad1[0x1A];       
    std::uint8_t useInteractionTrigger; 

    std::uint8_t _pad2[0x41];       
    std::uint8_t _triggerEnter;         
    std::uint8_t _interactionTriggerEnter; 
    std::uint8_t _prePerformTriggerEnter;  
};

template<auto Offset, typename Ret, typename... Args>
inline Ret Call(Args... args) {
    using FnType = Ret(*)(Args...);
    static auto fn = reinterpret_cast<FnType>(g_GameBase + Offset);
    return fn ? fn(args...) : Ret{};
}

namespace UnityUtils
{
    inline void* get_SingletonManager() 
    {
        return Call<offsets::MoleMole::SingletonManager::get_Instance, void*>();
    }

    inline void* GetSingletonInstance(void* singletonManager, Il2CppString* typeName) 
    {
        return Call<offsets::MoleMole::SingletonManager::GetSingletonInstance, void*>(singletonManager, typeName);
    }

    inline Il2CppString* PtrToStringAnsi(void* ptr) 
    {
        return Call<offsets::System::Marshal::PtrToStringAnsi, Il2CppString*>(ptr);
    }

    inline void* GetLocalAvatarEntity(void* __this) 
    {
        return Call<offsets::MoleMole::EntityManager::GetLocalAvatarEntity, void*>(__this);
    }

	inline void* GetMainCamera()
	{
		return Call<offsets::UnityEngine::Camera::get_main, void*>();
	}

    inline Vector3 GetRelativePosition(Vector3 pos)
    {
		return Call<offsets::MoleMole::WorldShiftManager::GetRelativePosition, Vector3>(pos);
    }

	inline Vector3 GetEntityLocalPosHead(void* baseEntity)
	{
		return Call<offsets::MoleMole::Factory::GetEntityPos, Vector3>(baseEntity);
	}

	inline Vector3 WorldToScreenPoint(void* camera, Vector3 worldPos)
	{
		return Call<offsets::UnityEngine::Camera::WorldToScreenPoint, Vector3>(camera, worldPos);
	}

	inline int get_pixelWidth(void* camera)
	{
		return Call<offsets::UnityEngine::Camera::get_pixelWidth, int>(camera);
	}

	inline int get_pixelHeight(void* camera)
	{
		return Call<offsets::UnityEngine::Camera::get_pixelHeight, int>(camera);
	}

	inline int get_width()
	{
		return Call<offsets::UnityEngine::Screen::get_width, int>();
	}

	inline int get_height()
	{
		return Call<offsets::UnityEngine::Screen::get_height, int>();
	}

    inline Vector3 GetRelativePosition_Entity(void* baseEntity)
    {
		return Call<offsets::MoleMole::BaseEntity::GetRelativePosition, Vector3>(baseEntity);
    }
    
	inline void* GetEntityList(void* entityManager)
	{
		return Call<offsets::MoleMole::EntityManager::entityList, void*>(entityManager);
	}

	inline void* get_threadSharedCluster() {
		return Call<offsets::MoleMole::ObjectPoolUtility::get_threadSharedCluster, void*>();
	}

	inline void* AllocateAutoAllocRecycleType(void* inReusable)
	{
		return Call<offsets::MoleMole::RecycleExtension::AllocateAutoAllocRecycleType, void*>(inReusable);
	}

	inline void* InitEvtCrash(void* __this, uint32_t targetID)
	{
		return Call<offsets::MoleMole::EvtCrash::Init, void*>(__this, targetID);
	}

    inline Vector3 GetAbsolutePosition(Vector3 localPos)
    {
        return Call<offsets::MoleMole::WorldShiftManager::GetAbsolutePosition, Vector3>(localPos);
    }

    inline void* FireEvent(void* EventManager, void* BaseEvent, bool immediately)
	{
		return Call<offsets::MoleMole::EventManager::FireEvent, void*>(EventManager, BaseEvent, immediately);
	}

	inline GenericList* GetAllLogicComponents(void* baseEntity)
	{
		return Call<offsets::MoleMole::BaseEntity::GetAllLogicComponents, GenericList*>(baseEntity);
	}

    inline void* GetAIBetaComponent(void* baseEntity)
    {
        return Call<offsets::MoleMole::BaseEntity::GetAIBetaComponent, void*>(baseEntity);
    }

    inline GenericList* GetAllVisualComponents(void* baseEntity)
    {
        return Call<offsets::MoleMole::BaseEntity::GetAllVisualComponents, GenericList*>(baseEntity);
    }

	inline float SafeFloatGetValue(void* SafeFloat)
	{
		return Call<offsets::MoleMole::SafeFloat::GetValue, float>(SafeFloat);
	}

	inline void* FindWithTag(Il2CppString* tag)
	{
		return Call<offsets::UnityEngine::GameObject::FindWithTag, void*>(tag);
	}

    inline void* GameObject_Find(Il2CppString* name)
    {
        return Call<offsets::UnityEngine::GameObject::Find, void*>(name);
    }

    inline void* GameObject_CreatePrimitive(int type)
    {
        return Call<offsets::UnityEngine::GameObject::CreatePrimitive, void*>(type);
    }
    
	inline void* AddComponentInternal(void* gameObject, Il2CppString* className)
	{
		return Call<offsets::UnityEngine::GameObject::AddComponentInternal, void*>(gameObject, className);
	}

	inline void LineRenderer_set_useWorldSpace(void* lineRenderer, bool value)
	{
		return Call<offsets::UnityEngine::LineRenderer::set_useWorldSpace, void>(lineRenderer, value);
	}

	inline void LineRenderer_set_startWidth(void* lineRenderer, float width)
	{
		return Call<offsets::UnityEngine::LineRenderer::set_startWidth, void>(lineRenderer, width);
	}

    inline void LineRenderer_set_endWidth(void* lineRenderer, float width)
    {
        return Call<offsets::UnityEngine::LineRenderer::set_endWidth, void>(lineRenderer, width);
    }

	inline void LineRenderer_set_startColor(void* lineRenderer, Color color)
	{
		return Call<offsets::UnityEngine::LineRenderer::set_startColor, void>(lineRenderer, color);
	}

    inline void LineRenderer_set_endColor(void* lineRenderer, Color color)
    {
        return Call<offsets::UnityEngine::LineRenderer::set_endColor, void>(lineRenderer, color);
    }

	inline int Miscs_GetSceneGroundLayerMaskWithoutTemp()
	{
		return Call<offsets::MoleMole::Miscs::GetSceneGroundLayerMaskWithoutTemp, int>();
	}

	inline int Miscs_GetSceneGroundLayerMask()
	{
		return Call<offsets::MoleMole::Miscs::GetSceneGroundLayerMask, int>();
	}

	inline int Miscs_GetDynamicBarrierLayerMask()
	{
		return Call<offsets::MoleMole::Miscs::GetDynamicBarrierLayerMask, int>();
	}

	inline int Miscs_GetExtraSurfaceLayerMask()
	{
		return Call<offsets::MoleMole::Miscs::GetExtraSurfaceLayerMask, int>();
	}

    inline float Miscs_CalcCurrentGroundHeight(float x, float z, float rayStartHeight, float rayDetectLength, int layer)
	{
		return Call<offsets::MoleMole::Miscs::CalcCurrentGroundHeight, float>(x, z, rayStartHeight, rayDetectLength, layer);
	}

	inline void LineRenderer_set_positionCount(void* lineRenderer, int count)
	{
		return Call<offsets::UnityEngine::LineRenderer::set_positionCount, void>(lineRenderer, count);
	}

	inline void LineRenderer_SetPosition(void* lineRenderer, int index, Vector3 position)
	{
		return Call<offsets::UnityEngine::LineRenderer::SetPosition, void>(lineRenderer, index, position);
	}

    inline void TrailRenderer_set_colorGradient(void* trailRenderer, void* gradient)
    {
        return Call<offsets::UnityEngine::TrailRenderer::set_colorGradient, void>(trailRenderer, gradient);
    }

    inline void TrailRenderer_SetSimulationSpeed(void* trailRenderer, float speed)
    {
        return Call<offsets::UnityEngine::TrailRenderer::SetSimulationSpeed, void>(trailRenderer, speed);
    }

    inline void TrailRenderer_set_customUVMode(void* trailRenderer, int value)
    {
        return Call<offsets::UnityEngine::TrailRenderer::set_customUVMode, void>(trailRenderer, value);
    }

    inline void TrailRenderer_set_customAddSegmentNum(void* trailRenderer, int value)
    {
        return Call<offsets::UnityEngine::TrailRenderer::set_customAddSegmentNum, void>(trailRenderer, value);
    }

    inline void TrailRenderer_Clear(void* trailRenderer)
    {
        return Call<offsets::UnityEngine::TrailRenderer::Clear, void>(trailRenderer);
    }

	inline void Rigidbody_set_velocity(void* rigidbody, Vector3 velocity)
	{
		return Call<offsets::UnityEngine::Rigidbody::set_velocity, void>(rigidbody, velocity);
	}

	inline Vector3 Rigidbody_get_velocity(void* rigidbody)
	{
		return Call<offsets::UnityEngine::Rigidbody::get_velocity, Vector3>(rigidbody);
	}

    inline Quaternion Rigidbody_get_rotation(void* rigidbody)
    {
        return Call<offsets::UnityEngine::Rigidbody::get_rotation, Quaternion>(rigidbody);
    }

    inline void Rigidbody_set_rotation(void* rigidbody, Quaternion rot)
    {
        return Call<offsets::UnityEngine::Rigidbody::set_rotation, void>(rigidbody, rot);
    }

    inline void Rigidbody_MovePosition(void* rigidbody, Vector3 pos)
    {
        return Call<offsets::UnityEngine::Rigidbody::MovePosition, void>(rigidbody, pos);
    }

    inline void Rigidbody_MoveRotation(void* rigidbody, Quaternion rot)
    {
        return Call<offsets::UnityEngine::Rigidbody::MoveRotation, void>(rigidbody, rot);
    }

    inline bool Rigidbody_get_isKinematic(void* rigidbody)
    {
        return Call<offsets::UnityEngine::Rigidbody::get_isKinematic, bool>(rigidbody);
    }

    inline void Rigidbody_set_isKinematic(void* rigidbody, bool value)
    {
        return Call<offsets::UnityEngine::Rigidbody::set_isKinematic, void>(rigidbody, value);
    }

    inline void Rigidbody_set_useGravity(void* rigidbody, bool value)
    {
        return Call<offsets::UnityEngine::Rigidbody::set_useGravity, void>(rigidbody, value);
    }

    inline void Rigidbody_set_detectCollisions(void* rigidbody, bool value)
    {
        return Call<offsets::UnityEngine::Rigidbody::set_detectCollisions, void>(rigidbody, value);
    }

    inline void* Component_get_transform(void* component)
    {
        return Call<offsets::UnityEngine::Component::get_transform, void*>(component);
    }

    inline void* Component_get_gameObject(void* component)
    {
        return Call<offsets::UnityEngine::Component::get_gameObject, void*>(component);
    }

    inline Il2CppString* Object_get_name(void* obj)
    {
        if (!obj)
            return nullptr;
        if constexpr (offsets::UnityEngine::Object::get_name == 0)
            return nullptr;
        return Call<offsets::UnityEngine::Object::get_name, Il2CppString*>(obj);
    }

    inline void* Component_GetComponent(void* component, Il2CppString* typeName)
    {
        return Call<offsets::UnityEngine::Component::GetComponent, void*>(component, typeName);
    }

    inline void* Renderer_get_material(void* renderer)
    {
        return Call<offsets::UnityEngine::Renderer::get_material, void*>(renderer);
    }

    inline void Renderer_set_material(void* renderer, void* material)
    {
        Call<offsets::UnityEngine::Renderer::set_material, void>(renderer, material);
    }

    inline void* Renderer_get_sharedMaterial(void* renderer)
    {
        return Call<offsets::UnityEngine::Renderer::get_sharedMaterial, void*>(renderer);
    }

    inline void Renderer_set_sharedMaterial(void* renderer, void* material)
    {
        Call<offsets::UnityEngine::Renderer::set_sharedMaterial, void>(renderer, material);
    }

    inline Il2CppArray* Renderer_get_sharedMaterials(void* renderer)
    {
        return Call<offsets::UnityEngine::Renderer::get_sharedMaterials, Il2CppArray*>(renderer);
    }

    inline bool Renderer_get_skipGPUCulling(void* renderer)
    {
        return Call<offsets::UnityEngine::Renderer::get_skipGPUCulling, bool>(renderer);
    }

    inline void Renderer_set_skipGPUCulling(void* renderer, bool value)
    {
        Call<offsets::UnityEngine::Renderer::set_skipGPUCulling, void>(renderer, value);
    }

    inline float Renderer_get_viewDistanceRatio(void* renderer)
    {
        return Call<offsets::UnityEngine::Renderer::get_viewDistanceRatio, float>(renderer);
    }

    inline void Renderer_set_viewDistanceRatio(void* renderer, float value)
    {
        Call<offsets::UnityEngine::Renderer::set_viewDistanceRatio, void>(renderer, value);
    }

    inline void Renderer_set_enabled(void* renderer, bool value)
    {
        Call<offsets::UnityEngine::Renderer::set_enabled, void>(renderer, value);
    }

    inline Il2CppArray* MonoVisualEntityTool_get_renderers(void* visualTool)
    {
        return Call<offsets::MoleMole::MonoVisualEntityTool::get_renderers, Il2CppArray*>(visualTool);
    }

    
    inline void* VCCostume_GetVCCostume(void* entity)
    {
        return Call<offsets::MoleMole::VCCostume::GetVCCostume, void*>(entity);
    }

    inline void VCCostume_RefreshCostumeInfo(void* vcCostume, uint32_t costumeId)
    {
        Call<offsets::MoleMole::VCCostume::RefreshCostumeInfo, void>(vcCostume, costumeId);
    }

    inline void VCFlyAttachment_ChangeFlycloak(void* vcFly, uint32_t flycloakId)
    {
        Call<offsets::MoleMole::VCFlyAttachment::ChangeFlycloak, void>(vcFly, flycloakId);
    }

    




    inline void GameObject_SetActive(void* gameObject, bool value)
    {
        Call<offsets::UnityEngine::GameObject::SetActive, void>(gameObject, value);
    }

    inline bool GameObject_get_activeSelf(void* gameObject)
    {
        return Call<offsets::UnityEngine::GameObject::get_activeSelf, bool>(gameObject);
    }

    inline int GameObject_get_layer(void* gameObject)
    {
        return Call<offsets::UnityEngine::GameObject::get_layer, int>(gameObject);
    }

    inline void GameObject_set_layer(void* gameObject, int value)
    {
        Call<offsets::UnityEngine::GameObject::set_layer, void>(gameObject, value);
    }

    inline void Behaviour_set_enabled(void* behaviour, bool value)
    {
        Call<offsets::UnityEngine::Behaviour::set_enabled, void>(behaviour, value);
    }

    inline Color Graphic_get_color(void* graphic)
    {
        return Call<offsets::UnityEngine::UI_Graphic::get_color, Color>(graphic);
    }

    inline void Graphic_set_color(void* graphic, Color value)
    {
        Call<offsets::UnityEngine::UI_Graphic::set_color, void>(graphic, value);
    }

    inline void Canvas_set_overrideSorting(void* canvas, bool value)
    {
        Call<offsets::UnityEngine::Canvas::set_overrideSorting, void>(canvas, value);
    }

    inline void Canvas_set_sortingOrder(void* canvas, int value)
    {
        Call<offsets::UnityEngine::Canvas::set_sortingOrder, void>(canvas, value);
    }

    inline int Shader_PropertyToID(Il2CppString* name)
    {
        return Call<offsets::UnityEngine::Shader::PropertyToID, int>(name);
    }

    inline void* Shader_Find(Il2CppString* name)
    {
        return Call<offsets::UnityEngine::Shader::Find, void*>(name);
    }

    inline void Shader_SetGlobalInt(int nameID, int value)
    {
        Call<offsets::UnityEngine::Shader::SetGlobalInt, void>(nameID, value);
    }

    inline int Shader_GetGlobalInt(int nameID)
    {
        return Call<offsets::UnityEngine::Shader::GetGlobalInt, int>(nameID);
    }

    inline void* Graphic_get_material(void* graphic)
    {
        return Call<offsets::UnityEngine::UI_Graphic::get_material, void*>(graphic);
    }

    inline void* Graphic_get_materialForRendering(void* graphic)
    {
        return Call<offsets::UnityEngine::UI_Graphic::get_materialForRendering, void*>(graphic);
    }

    inline void Material_SetInt(void* material, int nameID, int value)
    {
        Call<offsets::UnityEngine::Material::SetIntImpl, void>(material, nameID, value);
    }

    inline void* Material_get_shader(void* material)
    {
        return Call<offsets::UnityEngine::Material::get_shader, void*>(material);
    }

    inline void Material_set_shader(void* material, void* shader)
    {
        Call<offsets::UnityEngine::Material::set_shader, void>(material, shader);
    }

    inline Color Material_get_color(void* material)
    {
        return Call<offsets::UnityEngine::Material::get_color, Color>(material);
    }

    inline void Material_set_color(void* material, Color value)
    {
        Call<offsets::UnityEngine::Material::set_color, void>(material, value);
    }

    inline int Material_GetInt(void* material, int nameID)
    {
        return Call<offsets::UnityEngine::Material::GetIntImpl, int>(material, nameID);
    }

    inline int Material_get_renderQueue(void* material)
    {
        return Call<offsets::UnityEngine::Material::get_renderQueue, int>(material);
    }

    inline void Material_set_renderQueue(void* material, int value)
    {
        Call<offsets::UnityEngine::Material::set_renderQueue, void>(material, value);
    }

    inline void* Material_get_mainTexture(void* material)
    {
        return Call<offsets::UnityEngine::Material::get_mainTexture, void*>(material);
    }

    inline Il2CppArray* Material_GetTexturePropertyNames(void* material)
    {
        return Call<offsets::UnityEngine::Material::GetTexturePropertyNames, Il2CppArray*>(material);
    }

    inline void* Material_GetTexture(void* material, Il2CppString* name)
    {
        return Call<offsets::UnityEngine::Material::GetTexture, void*>(material, name);
    }

    inline bool ImageConversion_LoadImageData(void* texture, Il2CppArray* data)
    {
        return Call<offsets::UnityEngine::ImageConversion::_LoadImage, bool>(texture, data);
    }

    inline Il2CppArray* ImageConversion_EncodeToPNG(void* texture)
    {
        return Call<offsets::UnityEngine::ImageConversion::EncodeToPNG, Il2CppArray*>(texture);
    }

    inline Il2CppArray* File_ReadAllBytes(Il2CppString* path)
    {
        return Call<offsets::System::IO_File::ReadAllBytes, Il2CppArray*>(path);
    }

    inline int Texture_get_width(void* texture)
    {
        return Call<offsets::UnityEngine::Texture::get_width, int>(texture);
    }

    inline int Texture_get_height(void* texture)
    {
        return Call<offsets::UnityEngine::Texture::get_height, int>(texture);
    }

    inline void Texture2D_Apply(void* texture)
    {
        Call<offsets::UnityEngine::Texture2D::Apply, void>(texture);
    }

    inline void Texture2D_ReadPixels(void* texture, void* rectPtr, int destX, int destY, bool recalcMipmaps)
    {
        Call<offsets::UnityEngine::Texture2D::ReadPixels, void>(texture, rectPtr, destX, destY, recalcMipmaps);
    }

    inline Il2CppArray* Texture2D_GetPixels32(void* texture, int miplevel)
    {
        return Call<offsets::UnityEngine::Texture2D::GetPixels32, Il2CppArray*>(texture, miplevel);
    }

    inline void Texture2D_ctor(void* texture, int width, int height, int format, bool mipmap)
    {
        Call<offsets::UnityEngine::Texture2D::ctor, void>(texture, width, height, format, mipmap);
    }

    inline void Object_Destroy(void* obj)
    {
        Call<offsets::UnityEngine::Object::Destroy, void>(obj);
    }

    inline void* RenderTexture_GetTemporary(int width, int height, int depthBuffer)
    {
        return Call<offsets::UnityEngine::RenderTexture::GetTemporary, void*>(width, height, depthBuffer);
    }

    inline void RenderTexture_ReleaseTemporary(void* rt)
    {
        Call<offsets::UnityEngine::RenderTexture::ReleaseTemporary, void>(rt);
    }

    inline void* RenderTexture_get_active()
    {
        return Call<offsets::UnityEngine::RenderTexture::get_active, void*>();
    }

    inline void RenderTexture_set_active(void* rt)
    {
        Call<offsets::UnityEngine::RenderTexture::set_active, void>(rt);
    }

    inline void Graphics_Blit(void* source, void* dest)
    {
        Call<offsets::UnityEngine::Graphics::Blit, void>(source, dest);
    }

    inline Vector3 Transform_get_position(void* transform)
    {
        return Call<offsets::UnityEngine::Transform::get_position, Vector3>(transform);
    }

    inline void Transform_set_position(void* transform, Vector3 value)
    {
        return Call<offsets::UnityEngine::Transform::set_position, void>(transform, value);
    }

    inline Vector3 Transform_get_localPosition(void* transform)
    {
        return Call<offsets::UnityEngine::Transform::get_localPosition, Vector3>(transform);
    }

    inline void Transform_set_localPosition(void* transform, Vector3 value)
    {
        return Call<offsets::UnityEngine::Transform::set_localPosition, void>(transform, value);
    }

    inline Vector3 Transform_get_eulerAngles(void* transform)
    {
        return Call<offsets::UnityEngine::Transform::get_eulerAngles, Vector3>(transform);
    }

    inline void Transform_set_eulerAngles(void* transform, Vector3 value)
    {
        return Call<offsets::UnityEngine::Transform::set_eulerAngles, void>(transform, value);
    }

    inline Vector3 Transform_get_localEulerAngles(void* transform)
    {
        return Call<offsets::UnityEngine::Transform::get_localEulerAngles, Vector3>(transform);
    }

    inline void Transform_set_localEulerAngles(void* transform, Vector3 value)
    {
        return Call<offsets::UnityEngine::Transform::set_localEulerAngles, void>(transform, value);
    }

    inline Vector3 Transform_get_localScale(void* transform)
    {
        return Call<offsets::UnityEngine::Transform::get_localScale, Vector3>(transform);
    }

    inline void Transform_set_localScale(void* transform, Vector3 value)
    {
        return Call<offsets::UnityEngine::Transform::set_localScale, void>(transform, value);
    }

    inline int Transform_get_childCount(void* transform)
    {
        return Call<offsets::UnityEngine::Transform::get_childCount, int>(transform);
    }

    inline void* Transform_GetChild(void* transform, int index)
    {
        return Call<offsets::UnityEngine::Transform::GetChild, void*>(transform, index);
    }


    inline Vector3 Transform_get_forward(void* transform)
    {
        return Call<offsets::UnityEngine::Transform::get_forward, Vector3>(transform);
    }

	inline Vector3 Transform_get_right(void* transform)
	{
		return Call<offsets::UnityEngine::Transform::get_right, Vector3>(transform);
	}

    inline Il2CppString* Text_get_text(void* text)
    {
        return Call<offsets::UnityEngine::UI_Text::get_text, Il2CppString*>(text);
    }

    inline void Text_set_text(void* text, Il2CppString* value)
    {
        Call<offsets::UnityEngine::UI_Text::set_text, void>(text, value);
    }

    inline int Text_get_fontSize(void* text)
    {
        return Call<offsets::UnityEngine::UI_Text::get_fontSize, int>(text);
    }

    inline void Text_set_fontSize(void* text, int value)
    {
        Call<offsets::UnityEngine::UI_Text::set_fontSize, void>(text, value);
    }

	inline void* BaseEntity_rootObject(void* baseEntity)
	{
		return Call<offsets::MoleMole::BaseEntity::rootObject, void*>(baseEntity);
	}

    inline void* GetEntityOwner(void* BaseEntity){
		return Call<offsets::MoleMole::BaseEntity::GetEntityOwner, void*>(BaseEntity);
    }

	inline void SetRelativePosition(void* BaseEntity, Vector3 position, bool forceSyncToRigidbody)
	{
		Call<offsets::MoleMole::BaseEntity::SetRelativePosition, void>(BaseEntity, position, forceSyncToRigidbody);
	}

	inline void SetRotation(void* BaseEntity, Quaternion rotation, bool immediately)
	{
		Call<offsets::MoleMole::BaseEntity::SetRotation, void>(BaseEntity, rotation, immediately);
	}

	inline float Time_get_deltaTime()
	{
		return Call<offsets::UnityEngine::Time::get_deltaTime, float>();
	}

	inline bool Collider_get_enabled(void* collider)
	{
		return Call<offsets::UnityEngine::Collider::get_enabled, bool>(collider);
	}

	inline void Collider_set_enabled(void* collider, bool value)
	{
		Call<offsets::UnityEngine::Collider::set_enabled, void>(collider, value);
	}

	inline void* GetCameraEntity(void* entityManager)
	{
		return Call<offsets::MoleMole::EntityManager::GetCameraEntity, void*>(entityManager);
	}

	inline void* MonoInLevelMapPage_get_mapBackground(void* MonoInLevelMapPage)
	{
		return Call<offsets::MoleMole::MonoInLevelMapPage::get_mapBackground, void*>(MonoInLevelMapPage);
	}

	inline bool ScreenPointToLocalPointInRectangle(void* rect, Vector2 screenPoint, void* cam, Vector2* localPoint)
	{
		return Call<offsets::UnityEngine::RectTransformUtility::ScreenPointToLocalPointInRectangle, bool>(rect, screenPoint, cam, localPoint);
	}

    inline Rect MonoInLevelMapPage_get_mapRect(void* MonoInLevelMapPage) 
    {
        return Call<offsets::MoleMole::MonoInLevelMapPage::get_mapRect, Rect>(MonoInLevelMapPage);
    }

    inline void* MonoBillboard_get_combatBillboard(void* MonoBillboard)
    {
        return Call<offsets::MoleMole::MonoBillboard::get_combatBillboard, void*>(MonoBillboard);
    }

    inline void MonoBillboard_SetHPRatio(void* MonoBillboard, float ratio, bool isLive)
    {
        Call<offsets::MoleMole::MonoBillboard::SetHPRatio, void>(MonoBillboard, ratio, isLive);
    }

    inline void MonoBillboard_CreateCombatBillboard(void* MonoBillboard, int size, void* callback, Il2CppString* prefabName)
    {
        Call<offsets::MoleMole::MonoBillboard::CreateCombatBillboard, void>(MonoBillboard, size, callback, prefabName);
    }

    inline void* VCBillboard_get_billboard(void* vcBillboard)
    {
        return Call<offsets::MoleMole::VCBillboard::get_billboard, void*>(vcBillboard);
    }

    inline void MonoTransitionalReduceBar_ShowFill(void* bar, bool show)
    {
        Call<offsets::MoleMole::MonoTransitionalReduceBar::ShowFill, void>(bar, show);
    }

    inline void MonoTransitionalReduceBar_UpdateFill(void* bar, float value)
    {
        Call<offsets::MoleMole::MonoTransitionalReduceBar::UpdateFill, void>(bar, value);
    }

    inline void MonoCombatBillboard_set_showLevel(void* billboard, bool value)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::set_showLevel, void>(billboard, value);
    }

    inline void MonoCombatBillboard_set_showHpGrp(void* billboard, bool value)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::set_showHpGrp, void>(billboard, value);
    }

    inline void MonoCombatBillboard_set_showCombatBars(void* billboard, bool value)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::set_showCombatBars, void>(billboard, value);
    }

    inline void MonoCombatBillboard_set_showTitleGrp(void* billboard, bool value)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::set_showTitleGrp, void>(billboard, value);
    }

    inline void MonoCombatBillboard_set_showName(void* billboard, bool value)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::set_showName, void>(billboard, value);
    }

    inline void MonoCombatBillboard_set_showOnlineID(void* billboard, bool value)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::set_showOnlineID, void>(billboard, value);
    }

    inline void MonoCombatBillboard_set_titleLevel(void* billboard, Il2CppString* value)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::set_titleLevel, void>(billboard, value);
    }

    inline void MonoCombatBillboard_SetScale(void* billboard, float scale)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::SetScale, void>(billboard, scale);
    }

    inline void MonoCombatBillboard_SetLevelDisplay(void* billboard, float alpha)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::SetLevelDisplay, void>(billboard, alpha);
    }

    inline void MonoCombatBillboard_UpdateView(void* billboard)
    {
        Call<offsets::MoleMole::MonoCombatBillboard::UpdateView, void>(billboard);
    }

	inline Vector3 GenWorldPos(Vector2 levelMapPos)
	{
		return Call<offsets::MoleMole::Miscs::GenWorldPos, Vector3>(levelMapPos);
	}

	inline float CalcCurrentGroundHeight(float x, float z)
	{
		return Call<offsets::MoleMole::Miscs::CalcCurrentGroundHeight_xz, float>(x, z);
	}

    inline void SetAvatarPos(Vector3 pos) 
    {
        return Call<offsets::MoleMole::ActorUtils::SetAvatarPos, void>(pos);
    }

    inline void ActorUtils_SyncEntityPos(void* entity, int state, uint32_t mainQuestId)
    {
        return Call<offsets::MoleMole::ActorUtils::SyncEntityPos, void>(entity, state, mainQuestId);
    }

	inline void Camera_set_fieldOfView(void* camera, float fov)
	{
		Call<offsets::UnityEngine::Camera::set_fieldOfView, void>(camera, fov);
	}

	inline void Camera_set_farClipPlane(void* camera, float value)
	{
		Call<offsets::UnityEngine::Camera::set_farClipPlane, void>(camera, value);
	}

	inline void* GameObject_GetComponentByName(void* gameObject, Il2CppString* typeName)
	{
		return Call<offsets::UnityEngine::GameObject::GetComponentByName, void*>(gameObject, typeName);
	}

	inline void* Animator_GetBoneTransform(void* animator, int humanBoneId)
	{
		return Call<offsets::UnityEngine::Animator::GetBoneTransform, void*>(animator, humanBoneId);
	}

    inline void QualitySettings_set_shadows(int ShadowQuality) {
        return Call<offsets::UnityEngine::QualitySettings::set_shadows, void>(ShadowQuality);
    }
    inline int QualitySettings_get_shadows() {
        return Call<offsets::UnityEngine::QualitySettings::get_shadows, int>();
    }

    inline void MiHoYoVegetationManager_SetRuntimeQualityLevel(int value) {
        return Call<offsets::UnityEngine::MiHoYoVegetationManager::SetRuntimeQualityLevel, void>(value);
    }
    inline int MiHoYoVegetationManager_GetRuntimeQualityLevel() {
        return Call<offsets::UnityEngine::MiHoYoVegetationManager::GetRuntimeQualityLevel, int>();
    }

    inline void RenderSettings_set_fog(bool value) {
        return Call<offsets::UnityEngine::RenderSettings::set_fog, void>(value);
    }
    inline bool RenderSettings_get_fog() {
        return Call<offsets::UnityEngine::RenderSettings::get_fog, bool>();
    }

    inline Color RenderSettings_get_ambientGroundColor() {
        return Call<offsets::UnityEngine::RenderSettings::get_ambientGroundColor, Color>();
    }

    inline void RenderSettings_set_ambientGroundColor(Color color) {
        return Call<offsets::UnityEngine::RenderSettings::set_ambientGroundColor, void>(color);
    }

    inline void RenderSettings_set_ambientEquatorColor(Color color) {
        return Call<offsets::UnityEngine::RenderSettings::set_ambientEquatorColor, void>(color);
    }

    inline EnviroSky* EnviroSky_get_Instance() {
        return Call<offsets::MoleMole::EnviroSky::get_Instance, EnviroSky*>();
    }

    inline Bounds Renderer_get_bounds(void* renderer) {
        return Call<offsets::UnityEngine::Renderer::get_bounds, Bounds>(renderer);
    }

    inline void* Image_get_activeSprite(void* image)
    {
        return Call<offsets::UnityEngine::UI_Image::get_activeSprite, void*>(image);
    }

    inline void* Sprite_get_texture(void* sprite)
    {
        return Call<offsets::UnityEngine::Sprite::get_texture, void*>(sprite);
    }

    inline float Slider_get_minValue(void* slider)
    {
        return Call<offsets::UnityEngine::UI_Slider::get_minValue, float>(slider);
    }

    inline float Slider_get_maxValue(void* slider)
    {
        return Call<offsets::UnityEngine::UI_Slider::get_maxValue, float>(slider);
    }

    inline void Slider_set_value(void* slider, float value)
    {
        Call<offsets::UnityEngine::UI_Slider::set_value, void>(slider, value);
    }

    inline void Slider_UpdateVisuals(void* slider)
    {
        Call<offsets::UnityEngine::UI_Slider::UpdateVisuals, void>(slider);
    }
    inline void* MonoTalkDialog_get_conversationGrp(void* dialog)
    {
        return Call<offsets::MoleMole::MonoTalkDialog::get_conversationGrp, void*>(dialog);
    }

    inline void* MonoTalkDialog_get_selectGrp(void* dialog)
    {
        return Call<offsets::MoleMole::MonoTalkDialog::get_selectGrp, void*>(dialog);
    }

    inline void MonoTalkDialog_OnDialogClick(void* dialog, bool skip)
    {
        Call<offsets::MoleMole::MonoTalkDialog::OnDialogClick, void>(dialog, skip);
    }

    inline void MonoTalkDialog_set_showClickTip(void* dialog, bool value)
    {
        Call<offsets::MoleMole::MonoTalkDialog::set_showClickTip, void>(dialog, value);
    }

    inline bool MonoGrpConversation_ForceShowAllText(void* conversation)
    {
        return Call<offsets::MoleMole::MonoGrpConversation::ForceShowAllText, bool>(conversation);
    }

    inline void MonoGrpSelect_set_CurrSelection(void* grp, int value)
    {
        Call<offsets::MoleMole::MonoGrpSelect::set_CurrSelection, void>(grp, value);
    }

    inline void MonoGrpSelect_ConfirmSelection(void* grp)
    {
        Call<offsets::MoleMole::MonoGrpSelect::ConfirmSelection, void>(grp);
    }

    inline int MonoGrpSelect_get_itemCount(void* grp)
    {
        return Call<offsets::MoleMole::MonoGrpSelect::get_itemCount, int>(grp);
    }

    inline bool MonoTypewriter_ShowAllText(void* typewriter, bool instant)
    {
        return Call<offsets::MoleMole::MonoTypewriter::ShowAllText, bool>(typewriter, instant);
    }

    inline void MonoTypewriter_SetSecondPerChar(void* typewriter, float value)
    {
       return Call<offsets::MoleMole::MonoTypewriter::SetSecondPerChar, void>(typewriter, value);
    }

    inline void InLevelCutScenePageContext_Skip(void* ctx)
    {
        return Call<offsets::MoleMole::InLevelCutScenePageContext::Skip, void>(ctx);
    }

    inline void CriwareMediaPlayer_Skip(void* mediaPlayer)
    {
        return Call<offsets::MoleMole::CriwareMediaPlayer::Skip, void>(mediaPlayer);
    }

    inline void ItemModule_PickItem(void* itemModule, uint32_t entityID) {
        return Call<offsets::MoleMole::ItemModule::PickItem, void>(itemModule, entityID);
    }

    inline void MapModule_RequestUnlockTransPoint(void* mapModule, void* lcScenePoint) {
        if (!mapModule || !lcScenePoint)
            return;
        return Call<offsets::MoleMole::MapModule::RequestUnlockTransPoint, void>(mapModule, lcScenePoint);
    }

    inline void MapModule_ApplyUnlockScenePoint(void* mapModule, uint32_t sceneId, uint32_t pointId) {
        if (!mapModule || !sceneId || !pointId)
            return;
        return Call<offsets::MoleMole::MapModule::ApplyUnlockScenePoint, void>(mapModule, sceneId, pointId);
    }

    inline uint32_t LCScenePoint_GetSceneId(void* lcScenePoint)
    {
        return lcScenePoint
            ? *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(lcScenePoint) + offsets::MoleMole::LCScenePoint::SceneId)
            : 0;
    }

    inline uint32_t LCScenePoint_GetPointId(void* lcScenePoint)
    {
        return lcScenePoint
            ? *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(lcScenePoint) + offsets::MoleMole::LCScenePoint::PointId)
            : 0;
    }

    inline bool MapModule_UnlockScenePointImmediate(void* mapModule, void* lcScenePoint)
    {
        if (!mapModule || !lcScenePoint)
            return false;

        const uint32_t sceneId = LCScenePoint_GetSceneId(lcScenePoint);
        const uint32_t pointId = LCScenePoint_GetPointId(lcScenePoint);
        if (!sceneId || !pointId)
            return false;

        MapModule_RequestUnlockTransPoint(mapModule, lcScenePoint);
        MapModule_ApplyUnlockScenePoint(mapModule, sceneId, pointId);
        return true;
    }

    inline void InteractionManager_RequestNPCInteraction(
        void* interactionManager,
        uint32_t entityRuntimeID,
        bool force,
        bool asyncRequest,
        bool isRequestByInput,
        bool isRequestByNoBlack,
        int extraInfo,
        Il2CppString* interData,
        bool isHighPriority,
        bool clearBlackScreenBGAlpha)
    {
        return Call<offsets::MoleMole::InteractionManager::RequestNPCInteraction, void>(
            interactionManager,
            entityRuntimeID,
            force,
            asyncRequest,
            isRequestByInput,
            isRequestByNoBlack,
            extraInfo,
            interData,
            isHighPriority,
            clearBlackScreenBGAlpha);
    }

    inline bool LCChestPlugin_get_isLock(void* chestPlugin) {
        return Call<offsets::MoleMole::LCChestPlugin::get_isLock, bool>(chestPlugin);
    }

    inline bool LCSelectPickup_IsInPosition(void* LCSelectPickup, void* e) {
        return Call<offsets::MoleMole::LCSelectPickup::IsInPosition, bool>(LCSelectPickup, e);
    }

    inline bool LCSelectPickup_IsOutPosition(void* LCSelectPickup, void* e) {
        return Call<offsets::MoleMole::LCSelectPickup::IsOutPosition, bool>(LCSelectPickup, e);
    }

    inline bool LCSelectPickup_NeedShowPickUpBtn(void* LCSelectPickup, void* e) {
        return Call<offsets::MoleMole::LCSelectPickup::NeedShowPickUpBtn, bool>(LCSelectPickup, e);
    }

    inline void Application_set_targetFrameRate(int value) {
        return Call<offsets::UnityEngine::Application::set_targetFrameRate, void>(value);
    }

    struct EntityFactoryMonsterCreationData {
        uint32_t entityID = 0;
        int currentPoseID = 0;
        int aiPoseOrState = 0;
        void* aiData = nullptr;
        float scale = 1.0f;
        bool isElite = false;
        void* affixList = nullptr;
    };

    inline void* EntityFactory_CreateLocalGadget(int configId, Vector3 initPos, Vector3 initEuler, bool createComponents) {
        return Call <offsets::MoleMole::EntityFactory::CreateLocalGadget, void*>(configId, initPos, initEuler, createComponents);
    }

    inline void* EntityFactory_CreateLocalMonster(
        uint32_t entityId,
        uint32_t configId,
        Vector3 initPos,
        Vector3 initEuler,
        int currentPoseID,
        int aiPoseOrState,
        void* aiData,
        Nullable<uint32_t> campId,
        float scale,
        bool asyncLoad,
        void* loadFinishCallback,
        bool isElite,
        void* affixList,
        uint32_t ownerOrAuthority,
        bool isSummoned,
        bool lightWeight)
    {
        return Call<offsets::MoleMole::EntityFactory::CreateLocalMonster, void*>(
            entityId,
            configId,
            initPos,
            initEuler,
            currentPoseID,
            aiPoseOrState,
            aiData,
            campId,
            scale,
            asyncLoad,
            loadFinishCallback,
            isElite,
            affixList,
            ownerOrAuthority,
            isSummoned,
            lightWeight
        );
    }

    inline void EntityFactory_CreateMonsterComponents(
        void* monsterEntity,
        int32_t targetType,
        bool isElite,
        void* affixList,
        void* aiData,
        int32_t currentPoseID)
    {
        return Call<offsets::MoleMole::EntityFactory::CreateMonsterComponents, void>(
            monsterEntity,
            targetType,
            isElite,
            affixList,
            aiData,
            currentPoseID
        );
    }

    inline void ActorManager_CreateLocalMonster(
        void* actorManager,
        Il2CppString* alias,
        uint32_t configId,
        uint32_t level,
        Vector3 bornPos,
        float yaw,
        uint32_t dropId,
        float scale,
        bool isElite,
        void* affixList)
    {
        return Call<offsets::MoleMole::ActorManager::CreateLocalMonster, void>(
            actorManager,
            alias,
            configId,
            level,
            bornPos,
            yaw,
            dropId,
            scale,
            isElite,
            affixList
        );
    }

    inline bool EntityFactory_TryGetMonsterCreationData(uint32_t entityId, EntityFactoryMonsterCreationData* outData)
    {
        if (!outData) return false;
        return Call<offsets::MoleMole::EntityFactory::TryGetMonsterCreationData, bool>(entityId, outData);
    }

    inline void* EntityFactory_CreateLocalAvatar(
        uint64_t guid,
        uint32_t entityId,
        uint32_t configId,
        uint32_t campId,
        Vector3 initPos,
        bool asyncLoad,
        void* loadFinishCallback,
        uint32_t costumeId)
    {
        return Call<offsets::MoleMole::EntityFactory::CreateLocalAvatar, void*>(
            guid,
            entityId,
            configId,
            campId,
            initPos,
            asyncLoad,
            loadFinishCallback,
            costumeId
        );
    }

    inline void* EntityFactory_CreateNpcEntity(
        uint32_t entityId,
        uint32_t configId,
        Vector3 initPos,
        Vector3 initEuler,
        uint32_t roomID,
        uint32_t questID,
        bool asyncLoad)
    {
        return Call<offsets::MoleMole::EntityFactory::CreateNpcEntity, void*>(
            entityId,
            configId,
            initPos,
            initEuler,
            roomID,
            questID,
            asyncLoad
        );
    }

    inline void IPlayNetworkVideo_Stop(void* CriwareVideoPluginImplement) {
        return Call<offsets::MoleMole::CriwareVideoPluginImplement::IPlayNetworkVideo_Stop, void>(CriwareVideoPluginImplement);
    }

    inline void InLevelCutScenePageContext_RealShowSkipBtn(void* InLevelCutScenePageContext, bool showBtn) {
        return Call <offsets::MoleMole::InLevelCutScenePageContext::RealShowSkipBtn, void>(InLevelCutScenePageContext, showBtn);
    }

	inline void Transform_SetParent(void* transform, void* parent, bool worldPositionStays)
	{
		return Call<offsets::UnityEngine::Transform::SetParent, void>(transform, parent, worldPositionStays);
	}

	inline void Transform_set_localRotation(void* transform, Quaternion value)
	{
		return Call<offsets::UnityEngine::Transform::set_localRotation, void>(transform, value);
	}

	inline void* Transform_Find(void* transform, Il2CppString* name)
	{
		return Call<offsets::UnityEngine::Transform::Find, void*>(transform, name);
	}

	inline void* Texture2D_get_blackTexture()
	{
		return Call<offsets::UnityEngine::Texture2D::get_blackTexture, void*>();
	}

	inline bool ImageConversion_LoadImage(void* texture, Il2CppArray* data)
	{
		return Call<offsets::UnityEngine::ImageConversion::_LoadImage, bool>(texture, data);
	}

	inline void Material_SetTexture(void* mat, Il2CppString* name, void* texture)
	{
		Call<offsets::UnityEngine::Material::SetTexture, void>(mat, name, texture);
	}

    inline void UpdateCombatProp(void* LCBaseCombat, int propType, float val, int DataPropOp)
    {
        return Call<offsets::MoleMole::LCBaseCombat::UpdateCombatProp, void>(LCBaseCombat, propType, val, DataPropOp);
    }

    inline void* MonoLoadingCanvas_get_bgImg(void* MonoLoadingCanvas)
    {
        return Call<offsets::MoleMole::MonoLoadingCanvas::get_bgImg, void*>(MonoLoadingCanvas);
    }

	inline void ConfirmWithJoypad_Show(Il2CppString* strContent, Il2CppString* strAcceptBtnText, Il2CppString* strCancelBtnText, void* callback, int priority)
	{
		return Call<offsets::MoleMole::MiHoYo_SDK_ConfirmWithJoypad::Show, void>(strContent, strAcceptBtnText, strCancelBtnText, callback, priority);
	}

	inline void ConfirmWithJoypad_Hide()
	{
		return Call<offsets::MoleMole::MiHoYo_SDK_ConfirmWithJoypad::Hide, void>();
	}

	inline void Cursor_set_visible(bool value)
	{
		Call<offsets::UnityEngine::Cursor::set_visible, void>(value);
	}

    inline void Cursor_set_lockState(int value)
    {
        Call<offsets::UnityEngine::Cursor::set_lockState, void>(value);
    }

    inline void Texture2D_LoadRawTextureData(void* texture, void* data, int size)
    {
        return Call<offsets::UnityEngine::Texture2D::LoadRawTextureData, void>(texture, data, size);
    }

    inline void Texture2D_Apply_2(void* texture, bool updateMipmaps, bool makeNoLongerReadable)
    {
        return Call<offsets::UnityEngine::Texture2D::Apply, void>(texture, updateMipmaps, makeNoLongerReadable);
    }
    
    inline void AttackResult_set_elementType(void* attackResult, int elementType, MethodInfo* method) {
        return Call<offsets::MoleMole::AttackResult::set_elementType, void>(attackResult, elementType, method);
    }
    
    inline Il2CppString* BaseEntity_get_alias(void* BaseEntity) {
        return Call<offsets::MoleMole::BaseEntity::get_alias, Il2CppString*>(BaseEntity);
    }

    inline void* AttackResult_CreateAttackResult(ElementType sourceElementType, float attackElementDurability) {
        return Call<offsets::MoleMole::AttackResult::CreateAttackResult, void*>(sourceElementType, attackElementDurability);
    }

    inline void EvtBeingHit_Init(void* evtBeingHit, uint32_t targetID, uint32_t attackerID, void* attackResult, uint32_t frameID, void* method) {
        return Call<offsets::MoleMole::EvtBeingHit::Init, void>(evtBeingHit, targetID, attackerID, attackResult, frameID, method);
    }
};


namespace il2cppUtils {
	inline Il2CppClass* GetTypeInfoFromTypeDefinitionIndex(int typeDefIndex) {
		return Call<offsets::IL2CPP::GetTypeInfoFromTypeDefinitionIndex, Il2CppClass*>(typeDefIndex);
	}
	inline char* il2cpp_class_get_name(Il2CppClass* klass) {
		return Call<offsets::IL2CPP::il2cpp_class_get_name, char*>(klass);
	}
	inline FieldInfo* il2cpp_class_get_fields(Il2CppClass* klass, void** iter) {
		return Call<offsets::IL2CPP::il2cpp_class_get_fields, FieldInfo*>(klass, iter);
	}
	inline char* il2cpp_field_get_name(FieldInfo* field) {
		return Call<offsets::IL2CPP::il2cpp_field_get_name, char*>(field);
	}
	inline int32_t il2cpp_field_get_offset(FieldInfo* field) {
		return Call<offsets::IL2CPP::il2cpp_field_get_offset, int32_t>(field);
	}
	inline Il2CppType* il2cpp_field_get_type(FieldInfo* field) {
		return Call<offsets::IL2CPP::il2cpp_field_get_type, Il2CppType*>(field);
	}
	inline char* il2cpp_type_get_name(const Il2CppType* type) {
		return Call<offsets::IL2CPP::il2cpp_type_get_name, char*>(type);
	}

	inline Il2CppArray* il2cpp_array_new(Il2CppClass* elementTypeInfo, il2cpp_array_size_t length) {
		return Call<offsets::IL2CPP::il2cpp_array_new, Il2CppArray*>(elementTypeInfo, length);
	}

	inline Il2CppArray* il2cpp_array_new_бля(Il2CppClass* arrayTypeInfo, il2cpp_array_size_t length) {
		return Call<offsets::IL2CPP::il2cpp_array_new_бля, Il2CppArray*>(arrayTypeInfo, length);
	}

	inline void* ArrayDataPtr(Il2CppArray* arr) {
		return arr ? reinterpret_cast<void*>(&arr->vector[0]) : nullptr;
	}

	template <typename T>
	inline T* ArrayData(Il2CppArray* arr) {
		return arr ? reinterpret_cast<T*>(&arr->vector[0]) : nullptr;
	}

	inline Il2CppArray* ArrayFromBytes(Il2CppClass* byteClass, const void* src, size_t size) {
		if (!byteClass || !src || size == 0)
			return nullptr;

		Il2CppArray* arr = il2cpp_array_new(byteClass, static_cast<il2cpp_array_size_t>(size));
		if (!arr)
			return nullptr;

		void* dst = ArrayDataPtr(arr);
		if (!dst)
			return nullptr;

		std::memcpy(dst, src, size);
		return arr;
	}

	inline Il2CppArray* ArrayFromBytesL(Il2CppArray* lArray, const void* src, size_t size) {
		if (!lArray || !src || size == 0)
			return nullptr;

		Il2CppClass* arrayTypeInfo = lArray->obj.klass;
		if (!arrayTypeInfo)
			return nullptr;

		Il2CppArray* arr = il2cpp_array_new_бля(arrayTypeInfo, static_cast<il2cpp_array_size_t>(size));
		if (!arr)
			return nullptr;

		void* dst = ArrayDataPtr(arr);
		if (!dst)
			return nullptr;

		std::memcpy(dst, src, size);
		return arr;
	}

	inline bool LoadResourceBytes(HMODULE module, int resourceId, const uint8_t*& outData, size_t& outSize) {
		outData = nullptr;
		outSize = 0;

		if (!module)
			return false;

		const LPCWSTR resName = reinterpret_cast<LPCWSTR>(static_cast<ULONG_PTR>(static_cast<WORD>(resourceId)));
		const LPCWSTR resType = reinterpret_cast<LPCWSTR>(static_cast<ULONG_PTR>(10)); 
		HRSRC hResInfo = FindResourceW(module, resName, resType);
		if (!hResInfo)
			return false;

		const DWORD size = SizeofResource(module, hResInfo);
		if (size == 0)
			return false;

		HGLOBAL hRes = LoadResource(module, hResInfo);
		if (!hRes)
			return false;

		const void* data = LockResource(hRes);
		if (!data)
			return false;

		outData = reinterpret_cast<const uint8_t*>(data);
		outSize = static_cast<size_t>(size);
		return true;
	}
}

namespace MoleMoleUtils
{
    inline void* LCAIBeta_GetLevelCreationData(void* lcAIBeta)
    {
        return Call<offsets::MoleMole::LCAIBeta::GetLevelCreationData, void*>(lcAIBeta);
    }

    inline void LCAIBeta_resetAIData(void* lcAIBeta, void* aiData, bool authorityChange)
    {
        return Call<offsets::MoleMole::LCAIBeta::resetAIData, void>(lcAIBeta, aiData, authorityChange);
    }
}

// gotovie kolbaski
namespace OtherUtils 
{
    inline bool IsEntityRemoved(void* entity)
    {
        return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::_isToBeRemoved);
    }

    inline std::string Utf16ToUtf8(const char16_t* data, int32_t len)
    {
        if (!data || len <= 0)
            return {};
        int needed = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)data, len, NULL, 0, NULL, NULL);
        if (needed <= 0)
            return {};
        std::string out;
        out.resize(needed);
        WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)data, len, out.data(), needed, NULL, NULL);
        return out;
    }

    inline void* EntityManager()
    {
        const auto singletonManager = UnityUtils::get_SingletonManager();
        if (!singletonManager) return nullptr;

        auto* entityManagerName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::EntityManager()); 
        return UnityUtils::GetSingletonInstance(singletonManager, entityManagerName);
    }
    inline void* AvatarManager()
    {
        auto entityManager = OtherUtils::EntityManager();
        if (!entityManager) return nullptr;
        return UnityUtils::GetLocalAvatarEntity(entityManager);
    }

    inline void* ActorManager()
    {
        const auto singletonManager = UnityUtils::get_SingletonManager();
        if (!singletonManager)
            return nullptr;

        auto* actorManagerName = []() -> Il2CppString*
            {
                auto* klass = il2cppUtils::GetTypeInfoFromTypeDefinitionIndex(MoleMole_ActorManager);
                if (!klass)
                    return nullptr;

                auto* name = il2cppUtils::il2cpp_class_get_name(klass);
                if (!name)
                    return nullptr;

                return UnityUtils::PtrToStringAnsi(name);
            }();

        if (!actorManagerName)
            return nullptr;

        return UnityUtils::GetSingletonInstance(singletonManager, actorManagerName);
    }

    template <typename Fn>
    inline bool ForEachEntity(Fn&& fn)
    {
        auto* entityManager = EntityManager();
        if (!entityManager)
            return false;

        auto* listObj = UnityUtils::GetEntityList(entityManager);
        if (!listObj)
            return false;

        const auto* list = reinterpret_cast<GenericList*>(listObj);
        if (!list || list->size <= 0 || list->size > 10000)
            return false;

        const auto* items = reinterpret_cast<Il2CppArray*>(list->items);
        if (!items || list->size > static_cast<int>(items->max_length))
            return false;

        using Result = std::invoke_result_t<Fn&, void*>;

        for (int i = 0; i < list->size; ++i)
        {
            auto* entity = items->vector[i];
            if (!entity)
                continue;

            if constexpr (std::is_convertible_v<Result, bool>)
            {
                if (!static_cast<bool>(fn(entity)))
                    break;
            }
            else
            {
                fn(entity);
            }
        }

        return true;
    }

    template <typename Pred>
    inline void* FindEntity(Pred&& pred)
    {
        void* found = nullptr;
        ForEachEntity([&](void* entity) -> bool
            {
                if (!pred(entity))
                    return true;

                found = entity;
                return false;
            });
        return found;
    }

    inline void* EffectManager()
    {
        void* singletonManager = UnityUtils::get_SingletonManager();
        if (!singletonManager) return nullptr;
        Il2CppString* effectManagerName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::EffectManager());
        if (auto* mgr = UnityUtils::GetSingletonInstance(singletonManager, effectManagerName)) return mgr;
        return nullptr;
    }
    
    inline Vector3 WorldToScreenPointCorrected(void* camera, Vector3 worldPos)
    {
        auto screenPos = UnityUtils::WorldToScreenPoint(camera, worldPos);
        if (screenPos.z <= 0.f)
            return screenPos;

        static int realScreenW = UnityUtils::get_width();
        static int realScreenH = UnityUtils::get_height();
        static int cameraPixelW = camera ? UnityUtils::get_pixelWidth(camera) : realScreenW;
        static int cameraPixelH = camera ? UnityUtils::get_pixelHeight(camera) : realScreenH;
        static int refreshCounter = 0;

        if (++refreshCounter > 60)
        {
            realScreenW = UnityUtils::get_width();
            realScreenH = UnityUtils::get_height();
            if (camera)
            {
                cameraPixelW = UnityUtils::get_pixelWidth(camera);
                cameraPixelH = UnityUtils::get_pixelHeight(camera);
            }
            refreshCounter = 0;
        }

        if (cameraPixelW <= 0 || cameraPixelH <= 0)
            return screenPos;

        const float scaleX = static_cast<float>(realScreenW) / static_cast<float>(cameraPixelW);
        const float scaleY = static_cast<float>(realScreenH) / static_cast<float>(cameraPixelH);
        screenPos.x *= scaleX;
        screenPos.y *= scaleY;

        return screenPos;
    }

    inline void* AllocateFromPool(void* cluster, void* typeHandle) {
        if (!cluster || !typeHandle) return nullptr;
        void* klass = *(void**)cluster;
        auto alloc = (void* (__fastcall*)(void*, void*, int))(*(void**)((uintptr_t)klass + 0xF0));
        return alloc ? alloc(cluster, typeHandle, 0) : nullptr;
    }

    inline void* AllocateEvtCrashFromPool() {
        auto cluster = UnityUtils::get_threadSharedCluster();
        if (!cluster) return nullptr;

		auto evtCrashClass = il2cppUtils::GetTypeInfoFromTypeDefinitionIndex(MoleMole_EvtCrash);
        if (!evtCrashClass) return nullptr;

        void* evt = AllocateFromPool(cluster, evtCrashClass);
        if (!evt) return nullptr;

        UnityUtils::AllocateAutoAllocRecycleType(evt);
        return evt;
    }

    static inline EntityType GetEntityType(const void* ent) {
        return *reinterpret_cast<const EntityType*>(
            reinterpret_cast<std::uintptr_t>(ent) + static_cast<std::uintptr_t>(offsets::MoleMole::BaseEntity::entityType)
            );
    }

    inline void* FindProp(void* entity, GenericList* compList, const char* compName)
    {
        const auto lcList = compList;
        if (!lcList || lcList->size <= 0) return nullptr;

        const auto arr = reinterpret_cast<Il2CppArray*>(lcList->items);
        if (!arr) return nullptr;

        for (int i = 0; i < lcList->size; ++i)
        {
            auto* comp = arr->vector[i];
            if (!comp) continue;

            const auto klass = *reinterpret_cast<Il2CppClass**>(comp);
            if (!klass) continue;

            if (std::strcmp(il2cppUtils::il2cpp_class_get_name(klass), compName) == 0) return comp;
        }

        return nullptr;
    }
};

namespace EffectUtils
{
    inline void SetEffectActiveCmd(void* monoEffectProxy, bool active, bool checkOwner)
    {
        Call<offsets::MoleMole::MonoEffectProxy::SetEffectActiveCmd, void>(
            monoEffectProxy,
            active,
            checkOwner
        );
    }

    inline void SetEffectStopCmd(void* monoEffectProxy)
    {
        Call<offsets::MoleMole::MonoEffectProxy::SetEffectStopCmd, void>(
            monoEffectProxy
        );
    }

    inline void SetEffectDeallocateCmd(void* monoEffectProxy)
    {
        Call<offsets::MoleMole::MonoEffectProxy::SetEffectDeallocateCmd, void>(
            monoEffectProxy
        );
    }

    inline void DeallocateEffectProxy(void* effectManager, ABKFGEMPBNN proxyHandle)
    {
        Call<offsets::MoleMole::EffectManager::DeallocateEffectProxy, void>(
            effectManager,
            proxyHandle
        );
    }

    inline void RemoveEntityEffects(void* effectManager, ABKFGEMPBNN proxyHandle)
    {
        Call<offsets::MoleMole::EffectManager::RemoveEntityEffects, void>(
            effectManager,
            proxyHandle
        );
    }

    inline ABKFGEMPBNN CreateUnindexedEntityEffect(
        void* effectManager,
        void* owner,
        Il2CppString* effectName,
        Nullable<Vector3> initPos,
        Nullable<Quaternion> initRot,
        Nullable<Vector3> initScale,
        Nullable<Vector3> targetPos,
        void* targetEntity,
        void* targetTransform,
        Nullable<Vector3> targetDir,
        bool indexed,
        bool async,
        Il2CppString* effectTag,
        bool extraFlag,
        Nullable<OLOHHCPGJLF> extraData)
    {
        return Call<offsets::MoleMole::EffectManager::CreateUnindexedEntityEffect, ABKFGEMPBNN>(
            effectManager,
            owner,
            effectName,
            initPos,
            initRot,
            initScale,
            targetPos,
            targetEntity,
            targetTransform,
            targetDir,
            indexed,
            async,
            effectTag,
            extraFlag,
            extraData
        );
    }

    inline bool SpawnEffectAt(void* owner, const char* effectName, const Vector3& pos, const Vector3& scale)
    {
        if (!owner || !effectName || !*effectName)
            return false;

        void* mgr = OtherUtils::EffectManager();
        if (!mgr)
            return false;

        Il2CppString* effectStr = UnityUtils::PtrToStringAnsi((void*)effectName);
        if (!effectStr)
            return false;

        Nullable<Vector3> initPos = MakeNullable(pos);
        Quaternion rot{};
        rot.w = 1.0f;
        Nullable<Quaternion> initRot = MakeNullable(rot);
        Nullable<Vector3> initScale = MakeNullable(scale);
        Nullable<Vector3> targetPos = NullNullable<Vector3>();
        Nullable<Vector3> targetDir = NullNullable<Vector3>();
        Nullable<OLOHHCPGJLF> extraData = NullNullable<OLOHHCPGJLF>();

        CreateUnindexedEntityEffect(
            mgr,
            owner,
            effectStr,
            initPos,
            initRot,
            initScale,
            targetPos,
            nullptr,
            nullptr,
            targetDir,
            false,
            false,
            effectStr,
            false,
            extraData
        );

        return true;
    }
}

namespace dxUtils {

    
    inline bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
    {
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
        if (image_data == NULL)
            return false;
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = image_width;
        desc.Height = image_height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = image_data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        HRESULT hr = g_device->CreateTexture2D(&desc, &subResource, &pTexture);

        if (FAILED(hr)) {
            stbi_image_free(image_data);
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        hr = g_device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
        pTexture->Release();

        if (FAILED(hr)) {
            stbi_image_free(image_data);
            return false;
        }

        *out_width = image_width;
        *out_height = image_height;
        stbi_image_free(image_data);

        return true;
    }

    inline bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
    {
        FILE* f = nullptr;
        if (fopen_s(&f, file_name, "rb") != 0 || f == nullptr)
            return false;

        if (f == NULL)
            return false;
        fseek(f, 0, SEEK_END);
        size_t file_size = (size_t)ftell(f);
        if (file_size == -1)
            return false;
        fseek(f, 0, SEEK_SET);
        void* file_data = IM_ALLOC(file_size);
        fread(file_data, 1, file_size, f);
        fclose(f);
        bool ret = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height);
        IM_FREE(file_data);
        return ret;
    }

   
}

namespace BoneUtils
{
    struct BoneData
    {
        void* transform = nullptr;
        std::string name;
        Vector3 scale{};
        Vector3 rotation{};
    };

    template <typename Fn>
    inline int ForEachBone(void* rootTransform, const char* nameFilter, bool exactName, Fn&& fn)
    {
        if (!rootTransform) return 0;

        std::vector<void*> stack;
        stack.push_back(rootTransform);

        int visited = 0;
        while (!stack.empty())
        {
            void* t = stack.back();
            stack.pop_back();
            if (!t) continue;

            ++visited;

            void* go = UnityUtils::Component_get_gameObject(t);
            std::string boneName;
            if (go)
            {
                Il2CppString* nameStr = UnityUtils::Object_get_name(go);
                if (nameStr && nameStr->length > 0)
                    boneName = OtherUtils::Utf16ToUtf8(nameStr->chars, nameStr->length);
            }

            bool pass = true;
            if (nameFilter && nameFilter[0] != '\0')
            {
                if (exactName) pass = (boneName == nameFilter);
                else pass = (boneName.find(nameFilter) != std::string::npos);
            }

            if (pass)
            {
                BoneData b;
                b.transform = t;
                b.name = boneName;
                b.scale = UnityUtils::Transform_get_localScale(t);
                b.rotation = UnityUtils::Transform_get_eulerAngles(t);
                fn(b);
            }

            int childCount = UnityUtils::Transform_get_childCount(t);
            if (childCount <= 0 || childCount > 4096) continue;

            for (int i = 0; i < childCount; ++i)
            {
                void* child = UnityUtils::Transform_GetChild(t, i);
                if (child) stack.push_back(child);
            }
        }

        return visited;
    }

    inline void* GetLocalAvatarRootTransform()
    {
        void* localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar) return nullptr;

        void* rootObj = UnityUtils::BaseEntity_rootObject(localAvatar);
        if (!rootObj) return nullptr;

        return UnityUtils::GameObject_GetComponentByName(
            rootObj, UnityUtils::PtrToStringAnsi((void*)XS("Transform")));
    }
}
