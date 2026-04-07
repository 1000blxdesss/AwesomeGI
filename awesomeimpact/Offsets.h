/* AUTOGENRATED BY AwesomeAI - сын авесома*/
#pragma once
#include "Utils/test02.h"

namespace стринги_типо {
	inline const char* LCAvatarCombat() { return XS("APBEALMGOGC"); } // CKILHHPPACL
    inline const char* EventManager() { return XS("KCLAJFCFGCH"); }  // AFBPGJPILFA
    inline const char* LCMonsterCombat() { return XS("IGMMANECCMN"); } // FCFPBKKGBDC
    inline const char* MapModule() { return XS("PJEKOIAKMJM"); } // BBFIIGJPGMP
    inline const char* LCScenePoint() { return XS("JGBMKDOCAIP"); } // BCOLHLGBKNF
    inline const char* ItemModule() { return XS("AKELJJJLBCH"); } // BFJBDCPBINB
    inline const char* LCGadgetMisc() { return XS("GHDFIMDGJMB"); } // OFAHOMJEAGK
	inline const char* LCGadgetIntee() { return XS("LCGadgetIntee"); } // LCGadgetIntee
    inline const char* UIManager() { return XS("DGGLGNIIFJE"); } // HPDJOMNOHPH
    inline const char* EffectManager() { return XS("JFAKFBHCEHD"); } // FPDACGHNKDA
    inline const char* VCFlyAttachment() { return XS("GDOKGLGMFKP"); } // EJJDLJFEFOP
    inline const char* EntityManager() { return XS("IOOEEBGFAIN"); } //GHGEBKKDDKE
    inline const char* LCAbilityElement() { return XS("NOGDEOKCHDG"); }
}

namespace offsets {
    
    // 6.4
    // временная героиново-оффсетовая зависимость, хочется resolve_icall :(
    namespace UnityEngine {
        namespace Animator {
            inline constexpr uint32_t GetBoneTransform = 0x15F39CF0;
        }

        namespace Application {
            inline constexpr uint32_t set_targetFrameRate = 0x15F49EA0;
        }

        namespace Behaviour {
            inline constexpr uint32_t set_enabled = 0x15F3DEA0;
        }

        namespace Camera {
            inline constexpr uint32_t get_main = 0x15F5D0C0;
            inline constexpr uint32_t get_pixelHeight = 0x15F5C600;
            inline constexpr uint32_t get_pixelWidth = 0x15F5C5F0;
            inline constexpr uint32_t set_farClipPlane = 0x15F5BD90;
            inline constexpr uint32_t set_fieldOfView = 0x15F5BD50;
            inline constexpr uint32_t WorldToScreenPoint = 0x15F5CCE0;
        }

        namespace Canvas {
            inline constexpr uint32_t set_overrideSorting = 0x1602BBE0;
            inline constexpr uint32_t set_sortingOrder = 0x1602BC00;
        }

        namespace Collider {
            inline constexpr uint32_t get_enabled = 0x15FB8560;
            inline constexpr uint32_t set_enabled = 0x15FB8570;
        }

        namespace Component {
            inline constexpr uint32_t get_gameObject = 0x15F6D9E0;
            inline constexpr uint32_t get_transform = 0x15F6D9C0;
            inline constexpr uint32_t GetComponent = 0x15F6DA40;
        }

        namespace Cursor {
            inline constexpr uint32_t set_lockState = 0x15F42340;
            inline constexpr uint32_t set_visible = 0x15F42320;
        }

        namespace GameObject {
            inline constexpr uint32_t AddComponentInternal = 0x15F4C770;
            inline constexpr uint32_t CreatePrimitive = 0x15F4C390;
            inline constexpr uint32_t Find = 0x15F4CAA0;
            inline constexpr uint32_t FindWithTag = 0x15F4C8E0;
            inline constexpr uint32_t get_activeSelf = 0x15F4C800;
            inline constexpr uint32_t get_layer = 0x15F4C7B0;
            inline constexpr uint32_t GetComponentByName = 0x15F4C440;
            inline constexpr uint32_t set_layer = 0x15F4C7C0;
            inline constexpr uint32_t SetActive = 0x15F4C7F0;
        }

        namespace Graphics {
            inline constexpr uint32_t Blit = 0x15F588A0;
        }

        namespace ImageConversion {
            inline constexpr uint32_t EncodeToPNG = 0x15F846A0;
            inline constexpr uint32_t _LoadImage = 0x15F846E0;
        }

        namespace LineRenderer {
            inline constexpr uint32_t set_endColor = 0x15F59380;
            inline constexpr uint32_t set_endWidth = 0x15F59280;
            inline constexpr uint32_t set_positionCount = 0x15F593B0;
            inline constexpr uint32_t set_startColor = 0x15F59310;
            inline constexpr uint32_t set_startWidth = 0x15F59270;
            inline constexpr uint32_t set_useWorldSpace = 0x15F592B0;
            inline constexpr uint32_t SetPosition = 0x15F593C0;
        }

        namespace Material {
            inline constexpr uint32_t get_color = 0x15F6E640;
            inline constexpr uint32_t get_mainTexture = 0x15F6E6B0;
            inline constexpr uint32_t get_renderQueue = 0x15F6ECD0;
            inline constexpr uint32_t get_shader = 0x15F6E600;
            inline constexpr uint32_t GetIntImpl = 0x15F6E860;
            inline constexpr uint32_t GetTexture = 0x15F70130;
            inline constexpr uint32_t GetTexturePropertyNames = 0x15F6EA10;
            inline constexpr uint32_t set_color = 0x15F6E680;
            inline constexpr uint32_t set_renderQueue = 0x15F6ECF0;
            inline constexpr uint32_t set_shader = 0x15F6E620;
            inline constexpr uint32_t SetIntImpl = 0x15F6E780;
            inline constexpr uint32_t SetTexture = 0x15F6F1D0;
        }

        namespace MiHoYoVegetationManager {
            inline constexpr uint32_t GetRuntimeQualityLevel = 0x15FA7740;
            inline constexpr uint32_t SetRuntimeQualityLevel = 0x15FA7730;
        }

        namespace Object {
            inline constexpr uint32_t Destroy = 0x15F639C0;
            inline constexpr uint32_t get_name = 0x15F63A80;
        }

        namespace QualitySettings {
            inline constexpr uint32_t get_shadows = 0x15F82DB0;
            inline constexpr uint32_t set_shadows = 0x15F82DC0;
        }

        namespace RectTransformUtility {
            inline constexpr uint32_t ScreenPointToLocalPointInRectangle = 0x1602C9E0;
        }

        namespace RenderSettings {
            inline constexpr uint32_t get_ambientGroundColor = 0x15F59CE0;
            inline constexpr uint32_t get_fog = 0x15F59820;
            inline constexpr uint32_t set_ambientEquatorColor = 0x15F59CC0;
            inline constexpr uint32_t set_ambientGroundColor = 0x15F59D20;
            inline constexpr uint32_t set_fog = 0x15F59830;
        }

        namespace RenderTexture {
            inline constexpr uint32_t get_active = 0x15F44E90;
            inline constexpr uint32_t GetTemporary = 0x15F44810;
            inline constexpr uint32_t ReleaseTemporary = 0x15F44C10;
            inline constexpr uint32_t set_active = 0x15F44EA0;
        }

        namespace Renderer {
            inline constexpr uint32_t get_bounds = 0x15F83500;
            inline constexpr uint32_t get_material = 0x15F838B0;
            inline constexpr uint32_t get_sharedMaterial = 0x15F838D0;
            inline constexpr uint32_t get_sharedMaterials = 0x15F839C0;
            inline constexpr uint32_t get_skipGPUCulling = 0x15F82EB0;
            inline constexpr uint32_t get_viewDistanceRatio = 0x15F83410;
            inline constexpr uint32_t set_enabled = 0x15F83650;
            inline constexpr uint32_t set_material = 0x15F838C0;
            inline constexpr uint32_t set_sharedMaterial = 0x15F838E0;
            inline constexpr uint32_t set_skipGPUCulling = 0x15F82EC0;
            inline constexpr uint32_t set_viewDistanceRatio = 0x15F83420;
        }

        namespace Rigidbody {
            inline constexpr uint32_t get_isKinematic = 0x15FC4CB0;
            inline constexpr uint32_t get_rotation = 0x15FC5030;
            inline constexpr uint32_t get_velocity = 0x15FC4B20;
            inline constexpr uint32_t MovePosition = 0x15FC50C0;
            inline constexpr uint32_t MoveRotation = 0x15FC50E0;
            inline constexpr uint32_t set_detectCollisions = 0x15FC4FA0;
            inline constexpr uint32_t set_isKinematic = 0x15FC4CC0;
            inline constexpr uint32_t set_rotation = 0x15FC5080;
            inline constexpr uint32_t set_useGravity = 0x15FC4C80;
            inline constexpr uint32_t set_velocity = 0x15FC4B80;
        }

        namespace Screen {
            inline constexpr uint32_t get_currentResolution = 0x15F74110;
            inline constexpr uint32_t get_height = 0x15F74340;
            inline constexpr uint32_t get_width = 0x15F74330;
        }

        namespace Shader {
            inline constexpr uint32_t Find = 0x15F3E7A0;
            inline constexpr uint32_t GetGlobalInt = 0x15F3EFE0;
            inline constexpr uint32_t PropertyToID = 0x15F3EA10;
            inline constexpr uint32_t SetGlobalInt = 0x15F3EC30;
        }

        namespace Sprite {
            inline constexpr uint32_t get_texture = 0x15F7A250;
        }

        namespace Texture {
            inline constexpr uint32_t get_height = 0x15F5F1F0;
            inline constexpr uint32_t get_width = 0x15F5F180;
        }

        namespace Texture2D {
            inline constexpr uint32_t ctor = 0x15F47140;
            inline constexpr uint32_t Apply = 0x15F47530;
            inline constexpr uint32_t Apply_2 = 0x15F47510;
            inline constexpr uint32_t get_blackTexture = 0x15F471C0;
            inline constexpr uint32_t GetPixels32 = 0x15F474F0;
            inline constexpr uint32_t LoadRawTextureData = 0x15F47400;
            inline constexpr uint32_t ReadPixels = 0x15F475A0;
        }

        namespace Time {
            inline constexpr uint32_t get_deltaTime = 0x15F4EDD0;
        }

        namespace TrailRenderer {
            inline constexpr uint32_t Clear = 0x15F5A440;
            inline constexpr uint32_t set_colorGradient = 0x15F5A3F0;
            inline constexpr uint32_t set_customAddSegmentNum = 0x15F5A420;
            inline constexpr uint32_t set_customUVMode = 0x15F5A410;
            inline constexpr uint32_t SetSimulationSpeed = 0x15F5A400;
        }

        namespace Transform {
            inline constexpr uint32_t Find = 0x15F69740;
            inline constexpr uint32_t get_childCount = 0x15F696E0;
            inline constexpr uint32_t get_eulerAngles = 0x15F67410;
            inline constexpr uint32_t get_forward = 0x15F67B10;
            inline constexpr uint32_t get_localEulerAngles = 0x15F675C0;
            inline constexpr uint32_t get_localPosition = 0x15F643D0;
            inline constexpr uint32_t get_localScale = 0x15F67D20;
            inline constexpr uint32_t get_position = 0x15F67010;
            inline constexpr uint32_t get_right = 0x15F67770;
            inline constexpr uint32_t GetChild = 0x15F69860;
            inline constexpr uint32_t set_eulerAngles = 0x15F67550;
            inline constexpr uint32_t set_localEulerAngles = 0x15F67700;
            inline constexpr uint32_t set_localPosition = 0x15F644B0;
            inline constexpr uint32_t set_localRotation = 0x15F67760;
            inline constexpr uint32_t set_localScale = 0x15F67D80;
            inline constexpr uint32_t set_position = 0x15F67070;
            inline constexpr uint32_t SetParent = 0x15F67F50;
        }

        namespace UI_Graphic {
            inline constexpr uint32_t get_color = 0x1604B340;
            inline constexpr uint32_t get_material = 0x1604BB60;
            inline constexpr uint32_t get_materialForRendering = 0x1604BBE0;
            inline constexpr uint32_t set_color = 0x1604B350;
            inline constexpr uint32_t set_material = 0x1604BB80;
        }

        namespace UI_Image {
            inline constexpr uint32_t m_PreserveAspect = 0xF4;

            inline constexpr uint32_t get_activeSprite = 0x16065530;
            inline constexpr uint32_t set_sprite = 0x16064FB0;
        }

        namespace UI_Slider {
            inline constexpr uint32_t m_FillImage = 0x108;

            inline constexpr uint32_t get_maxValue = 0x1607E370;
            inline constexpr uint32_t get_minValue = 0x1607E2F0;
            inline constexpr uint32_t set_value = 0x1607E4F0;
            inline constexpr uint32_t UpdateVisuals = 0x1607DCF0;
        }

        namespace UI_Text {
            inline constexpr uint32_t get_fontSize = 0x16049250;
            inline constexpr uint32_t get_text = 0x160489A0;
            inline constexpr uint32_t set_fontSize = 0x16049270;
            inline constexpr uint32_t set_text = 0x160489B0;
        }
    }

    // 6.4
    namespace System {
        namespace IO_File {
            inline constexpr uint32_t ReadAllBytes = 0x159C7710;
        }

        namespace Marshal {
            inline constexpr uint32_t PtrToStringAnsi = 0x15950640;
        }
    }

    namespace MoleMole {

        // 6.4
        namespace LuaLimitRegionTask {
            inline constexpr uint32_t TickInternal = 0x7CAA170;
        }

        // 6.4
        namespace ActorUtils {
            inline constexpr uint32_t SetAvatarPos = 0x10952E40;
            inline constexpr uint32_t SyncEntityPos = 0x10937C70;
            inline constexpr uint32_t OnEnterSceneDone = 0x109562C0;
        }

        // 6.4
        namespace MiHoYo_SDK_ConfirmWithJoypad {
            inline constexpr uint32_t Show = 0x156D0E20;
            inline constexpr uint32_t Awake = 0x156D0480;
            inline constexpr uint32_t Hide = 0x156D11C0;
            inline constexpr uint32_t Update = 0x156D09B0;
            inline constexpr uint32_t OnTapAccept = 0x156D0D40;
            inline constexpr uint32_t OnTapCancel = 0x156D0DB0;
        }

        // 6.4
        namespace MonoLoginScene {
            inline constexpr uintptr_t Update = 0xF688EC0;//0xf2a1350;
            inline constexpr uintptr_t OpenDoor = 0xF6886C0;//0xF29F120;
        }

        // 6.4
        namespace MonoLoadingCanvas {
            inline constexpr uint32_t Start = 0x71BC3E0;
            inline constexpr uint32_t get_bgImg = 0x71BC4D0;
        }

        // 6.4
        namespace GameManager {
            inline constexpr uint32_t Update = 0xCC2D750;
        }

        // 6.4
        namespace SingletonManager {
            inline constexpr uint32_t GetSingletonInstance = 0x5FF2090;
            inline constexpr uint32_t get_Instance = 0x5FF21C0;
        }

        // 6.4
        namespace Miscs {
            inline constexpr uint32_t CheckTargetAttackable = 0x100F44E0;
            inline constexpr uint32_t GetSceneGroundLayerMaskWithoutTemp = 0x100F37C0;
            inline constexpr uint32_t GetDynamicBarrierLayerMask = 0x100D7AB0;
            inline constexpr uint32_t GetSceneGroundLayerMask = 0x100EA160;
            inline constexpr uint32_t GetExtraSurfaceLayerMask = 0x100AD300;
            inline constexpr uint32_t CalcCurrentGroundHeight = 0x100D9260;
            inline constexpr uint32_t GenWorldPos = 0x100CEA20;
            inline constexpr uint32_t CalcCurrentGroundHeight_xz = 0x100D6610;
        }

        // 6.4
        namespace ObjectPoolUtility {
            inline constexpr uint32_t get_threadSharedCluster = 0x5FB0B40;
        }

        // 6.4
        namespace RecycleExtension {
            inline constexpr uint32_t AllocateAutoAllocRecycleType = 0x5EF5540;
        }

        // 6.4
        namespace WorldShiftManager {
            inline constexpr uint32_t GetAbsolutePosition = 0x61CB9D0;
            inline constexpr uint32_t GetRelativePosition = 0x61CFF80;
        }

        // 6.4
        namespace MonoJumpButton {
            inline constexpr uint32_t OnPointerUp = 0x6812220;
            inline constexpr uint32_t OnRealPointerDown = 0x68124A0;
        }

        // 6.4
        namespace MonoInLevelMapPage {
            inline constexpr uint32_t get_mapRect = 0xCC95530;
            inline constexpr uint32_t get_mapBackground = 0xCC95510;
        }

        // 6.4
        namespace EnviroSky {
            inline constexpr uint32_t get_Instance = 0x10048410;
        }

        // 6.4
        namespace CinemachineBrain {
            inline constexpr uint32_t FlushToOutpuCamera = 0x5F140E0;
        }

        // 6.4
        namespace MonoVisualEntityTool {
            inline constexpr uint32_t get_renderers = 0x86B5030;
        }

        // 6.4
        namespace MonoBillboard {
            inline constexpr uint32_t get_combatBillboard = 0xE1E4790;
            inline constexpr uint32_t CreateCombatBillboard = 0xE1E2360;
            inline constexpr uint32_t SetHPRatio = 0xE1E2990;
            inline constexpr uint32_t set_entityName = 0xE1E34D0;
        }

        // 6.4
        namespace MonoCombatBillboard {
            inline constexpr uint32_t set_showLevel = 0x99A9D10;
            inline constexpr uint32_t set_showHpGrp = 0x99AB150;
            inline constexpr uint32_t set_showCombatBars = 0x99AB4A0;
            inline constexpr uint32_t set_showTitleGrp = 0x99A9230;
            inline constexpr uint32_t set_titleLevel = 0x99A9F40;
            inline constexpr uint32_t set_showName = 0x99A9430;
            inline constexpr uint32_t set_showOnlineID = 0x99AA820;
            inline constexpr uint32_t SetScale = 0x99ABAB0;
            inline constexpr uint32_t SetLevelDisplay = 0x99A9DF0;
            inline constexpr uint32_t UpdateView = 0x99A9000;
        }

        // 6.4
        namespace MonoTransitionalReduceBar {
            inline constexpr uint32_t ShowFill = 0xC329DA0;
            inline constexpr uint32_t UpdateFill = 0xC329D00;
        }

        // 6.4
        namespace MonoEffect {
            inline constexpr uint32_t ShowRenders = 0x99B7010;
            inline constexpr uint32_t AttachToProxy = 0x99BAE30;
        }

        // 6.4
        namespace BaseActor {
            inline constexpr uint32_t TryPlayVideo = 0x1097A250;
        }

        // 6.4
        namespace ActorManager {
            inline constexpr uint32_t CreateLocalMonster = 0xD717230;
        }

        // 6.4
        namespace CriwareMediaPlayer {
            inline constexpr uint32_t Skip = 0x1002CA20;
            inline constexpr uint32_t OnStartPlay = 0x1002B3B0;
        }

        // 6.4
        namespace MonoInLevelPlayerProfilePageV3 {
            inline constexpr uint32_t get_logoutButton = 0x109F2F30;
            inline constexpr uint32_t get_logoutButtonMask = 0x109F31B0;
        }

        // 6.4
        namespace MonoTalkDialog {
            inline constexpr uint32_t OnEnable = 0xCCCBE90;
            inline constexpr uint32_t HideDialog = 0xCCCB7E0;
            inline constexpr uint32_t set_showClickTip = 0xCCCB310;
            inline constexpr uint32_t OnDialogClick = 0xCCCD150;
            inline constexpr uint32_t get_selectGrp = 0xCCCB970;
            inline constexpr uint32_t get_conversationGrp = 0xCCCD740;
        }

        // 6.4
        namespace MonoGrpSelect {
            inline constexpr uint32_t set_CurrSelection = 0x99DA970;
            inline constexpr uint32_t ConfirmSelection = 0x99DB110;
            inline constexpr uint32_t get_itemCount = 0x99DB420;
        }

        // 6.4
        namespace MonoGrpConversation {
            inline constexpr uint32_t ForceShowAllText = 0x8F3DBE0;
        }

        // 6.4
        namespace MonoTypewriter {
            inline constexpr uint32_t SetSecondPerChar = 0xAF0B570;
            inline constexpr uint32_t ShowAllText = 0xAF0BF70;
        }

        // дальше хуйня

        // 6.4
        namespace AttackResult {
            inline constexpr uint32_t CreateAttackResult = 0x80E9470;
            inline constexpr uint32_t set_elementType = 0x80E5C60;
        }

        namespace ___ {
			inline constexpr uint32_t ____ = 0xD2D3FF0;
        }

        // 6.4
        namespace LCAbilityElement {
            inline constexpr uint32_t _lcAbilityState = 0x138;
        }

        // 6.4
        namespace LCAbilityState {
            inline constexpr uint32_t abilityState = 0x1C8;
        }

        // 6.4
        namespace EvtBeingHit {
            inline constexpr uint32_t Init = 0xB381F10;
        }

        // 6.4
        namespace EntityFactory
        {
            inline constexpr uintptr_t CreateMonsterComponents = 0x3A6E160;
            inline constexpr uintptr_t CreateLocalGadget = 0x70837E0;//0x7F5C3D0;
            inline constexpr uintptr_t CreateLocalMonster = 0x707A530;//0x7F50F70;
            inline constexpr uintptr_t CreateLocalAvatar = 0x70829D0;//0x7F5E000;
            inline constexpr uintptr_t CreateNpcEntity = 0x707DB70;//0x7F5F050;
            inline constexpr uintptr_t TryGetMonsterCreationData = 0x70861C0;//0x7F539F0;
        }

        // 6.4
        namespace LCScenePoint {
            inline constexpr uintptr_t _unlocked = 0x154; // public class MoleMole.MonoFetterVoiceInfo : MonoBehaviour 
        }

        /*namespace InLevelPlayerProfilePageContext {
            inline constexpr uint32_t SetupView = 0x0bd5ae00;
            inline constexpr uint32_t BackCameraLegacy = 0x0bd59270;
            inline constexpr uint32_t ClosePage = 0x0bd58c60;
            inline constexpr uint32_t OnDisable = 0x0bd5a030;
        }*/

        /*namespace ProfilePageContextV3 {
            inline constexpr uint32_t BackCamera = 0x0df5d160;
        }*/

        // 6.4
        namespace InLevelCutScenePageContext {
            inline constexpr uint32_t UpdateView = 0xFB50B00;//0x96af0a0;
            inline constexpr uint32_t RealShowSkipBtn = 0xFB51300;//0x96AC5A0;
            inline constexpr uint32_t Skip = 0xFB4E470;//0x96AD230;
            inline constexpr uint32_t ClearView = 0xFB4FB80;//0x96AEA70;

            //inline constexpr uint32_t _talkDialog = 0x250; // DBJCDAPMEPK
            //inline constexpr uint32_t _timerA = 0x270;
            //inline constexpr uint32_t _timerB = 0x274;
            //inline constexpr uint32_t _flagA = 0x27c;
            //inline constexpr uint32_t _flagB = 0x27d;
            //inline constexpr uint32_t _flagC = 0x27e;
            //inline constexpr uint32_t _timerC = 0x280;
            //inline constexpr uint32_t _timerD = 0x2a4;
        }

		// 6.4
        namespace CriwareVideoPluginImplement {
            inline constexpr uint32_t IPlayNetworkVideo_Stop = 0xF334950;//0x74D9650;
            inline constexpr uint32_t AsyncPlayLike = 0xF334EE0;//0x74D9870;
        }

        /*namespace VideoDispatch {
            inline constexpr uint32_t CallbackBind = 0x0FAE4D50;
        }*/

        // 6.4
        namespace LCAbilityElement {
            inline constexpr uint32_t ReduceModifierDurability = 0xED692F0;//0xED692F0;
        }

        // 6.4
        namespace ItemModule {
            inline constexpr uint32_t PickItem = 0xFBDE4D0;//0x839f330;
        }

        // 6.4
        namespace MapModule {
            inline constexpr uint32_t RequestUnlockTransPoint = 0x10BFCA00;//0xF7E2A80;
            inline constexpr uint32_t ApplyUnlockScenePoint = 0x10BE4BE0;//0xF7E5940;
        }

        namespace LCScenePoint {
            inline constexpr uint32_t PointId = 0x148;
            inline constexpr uint32_t SceneId = 0x150;
        }

        namespace InteractionManager {
            inline constexpr uint32_t RequestNPCInteraction = 0xD2F0070;
        }

        // 6.4
        namespace LCSelectPickup {
            inline constexpr uint32_t AddInteeBtnByID = 0xAD57A30;//0xc1428f0;
            inline constexpr uint32_t IsInPosition = 0xAD58DD0;//0xC142410;
            inline constexpr uint32_t IsOutPosition = 0xAD56CF0;//0xC1410E0;
            inline constexpr uint32_t NeedShowPickUpBtn = 0xC1431D0; // MCHFNAJCICJ(BaseEntity)
        }

        // 6.4
        namespace LCGadgetMisc {
            inline constexpr uint32_t curGadgetState = 0x158; // 0x158;
            inline constexpr uint32_t _chestPlugin = 0x148; // 0x140;
        }

        // 6.4
        namespace LCChestPlugin {
            inline constexpr uint32_t get_isLock = 0xE7AC0B0;//0xD06B460;
        }

        /*namespace GadgetModule {
            inline constexpr uint32_t RequestGadgetInteract = 0x6D072C0; 
            inline constexpr uint32_t DoGadgetTouch = 0x6D0F5E0; 
            inline constexpr uint32_t OnGadgetTouch = 0x6D143C0;
        }*/

        // 6.4
        namespace VCBaseSetDitherValue {
            inline constexpr uint32_t get_DitherAlphaValue = 0x8D87800;//0xD7FC400;
        }

        // 6.4
        namespace HumanoidMoveFSM { // _humanoidMoveFSM
            inline constexpr uint32_t _rigidbody = 0x58;//0x100;
            inline constexpr uint32_t _moveData = 0x100;//0x40;
            inline constexpr uint32_t _curState = 0x108;//0x48;

            inline constexpr uint32_t LateTick = 0x919AAB0;//0x9f3bab0;
            inline constexpr uint32_t UpdateSprintCooldown = 0x91908A0;//0x9F44670;
        }
		


		// 6.4
        namespace BaseEntity {
            inline constexpr uint32_t _colliders = 0x30;//0x28;
            inline constexpr uint32_t configID = 0x450;//0x368;
            inline constexpr uint32_t runtimeID = 0x3AC;//0x474;
            inline constexpr uint32_t entityType = 0x410;//0x470;
            inline constexpr uint32_t _isToBeRemoved = 0x44E;//0x3a4; // MoleMole.MonoEffectPluginDestroyImediately : BaseMonoEffectPlugin
            inline constexpr uint32_t monoVisualEntityTool = 0xB8;//0xd8;

            inline constexpr uint32_t GetRelativePosition = 0x6DD9410;//0x87a9080;
            inline constexpr uint32_t SetRelativePosition = 0x6DD03B0;//0x87c3240;
            inline constexpr uint32_t get_alias = 0x6DE2910;//0x87CA220;
            inline constexpr uint32_t GetAIBetaComponent = 0x64DB4E4;
            inline constexpr uint32_t GetAllLogicComponents = 0x6DCD1F0;//0x87C5E50;
            inline constexpr uint32_t GetAllVisualComponents = 0x6DC6440;//0x87b2a30;
            inline constexpr uint32_t rootObject = 0x6DF3770;//0x87d61e0; // class MoleMole.MonoNestedReusableList
            inline constexpr uint32_t GetEntityOwner = 0x6DF59F0;//0x87BCEE0;
            inline constexpr uint32_t SetRotation = 0x6DC96E0;//0x87b2800;
        }

        namespace LCAIBeta {
            inline constexpr uint32_t resetAIData = 0x5EB31E4;
            inline constexpr uint32_t GetLevelCreationData = 0x4F2C74C;
        }

        // 6.4
        namespace ActorAbilityPlugin {
            //inline constexpr uint32_t nextValidAbilityID = 0x1F8;//0x1fc;
            inline constexpr uint32_t SetDynamicFloatWithRange = 0xB2F2DE0;//0x72b7480;
        }

		// 6.4
        namespace LCBaseCombat {
            inline constexpr uint32_t CombatProperty_k__BackingField = 0x1C8;//0x148;

            inline constexpr uint32_t FireBeingHitEvent = 0xE092D10;//0x8A18C20;
            inline constexpr uint32_t UpdateCombatProp = 0xE0948C0;//0x8a1fe50;
        }

        // 6.4
        namespace VCHumanoidMove {
            inline constexpr uint32_t NotifyLandVelocity = 0x901B630;//0x8e11150;
        }
		// 6.4
        namespace EntityManager {
            inline constexpr uint32_t GetLocalAvatarEntity = 0xDEAC630;//0xE4217E0;
            inline constexpr uint32_t GetCameraEntity = 0xDF02870;//0xE42CF80;
            inline constexpr uint32_t entityList = 0xDEF3F00;//0xe42fd90;
            inline constexpr uint32_t AddEntity = 0xDF00D20;//0xE430E10;
        }

		// 6.4
        namespace EffectManager {
            inline constexpr uint32_t CreateUnindexedEntityEffect = 0x8CE1CB0;//0x10ba8b90;
            inline constexpr uint32_t DeallocateEffectProxy = 0x8CE05C0;
            inline constexpr uint32_t RemoveEntityEffects = 0x8CE1060;
            //inline constexpr uint32_t SetEntityEffectStop = 0x8CE4439;//0x10BA7EFC;
            inline constexpr uint32_t AttachProxyWithEffect = 0x8CE4AD0;//0x10ba9080;
        }

        // 6.4 LFKEJGNKIMO
        namespace MonoEffectProxy {
            inline constexpr uint32_t SetEffectStopCmd = 0xCA661F0;
            inline constexpr uint32_t SetEffectDeallocateCmd = 0xCA66660;
            inline constexpr uint32_t SetEffectActiveCmd = 0xCA66D70;
        }



        // 6.4
        namespace LCAvatarCombat {
            inline constexpr uint32_t TickSkillCd = 0xA908220;//0xAE8EE30;
            inline constexpr uint32_t IsEnergyMax = 0xA90F8B0;//0xAE92800;
        }

        // 6.4
        namespace BaseMoveSyncPlugin {
            inline constexpr uint32_t _syncTask = 0x48;//0x40; // protected MoleMole.MoveSyncTask
            inline constexpr uint32_t _prevMotionState = 0x37C;//0x378;

            inline constexpr uint32_t ConvertSyncTaskToMotionInfo = 0xA8C8B90;//0x92F7EF0;
        }

        // 6.4
        namespace BaseComponentPlugin {
            inline constexpr uint32_t _owner = 0x28;//0x20;
        }

        // 6.4
        namespace BaseComponent {
            inline constexpr uint32_t _entity = 0x28;//0x28;
            inline constexpr uint32_t entityRuntimeID = 0x14;//0x14;
        }
        namespace Factory {
            inline constexpr uint32_t GetEntityPos = 0xef4b870;
        }
		// 6.4
        namespace EvtCrash {
            inline constexpr uint32_t hitPos = 0x30; // 0x30;
            inline constexpr uint32_t velChange = 0x40;//0x3c;
            inline constexpr uint32_t maxHp = 0x3c;//0x40;

            inline constexpr uint32_t Init = 0xC9F5D50;//0xdb9e150;
        }
		// 6.4
        namespace EventManager {
            inline constexpr uint32_t FireEvent = 0xC99E140;//0xede1660;
        }

		// 6.4
        namespace SafeFloat {
            inline constexpr uint32_t maxHP = 0x4D8;//0xe8;

            inline constexpr uint32_t GetValue = 0x82B70;//0xf2de0;
        }

        // 6.4
        namespace VCCostume {
            inline constexpr uint32_t GetVCCostume = 0xDA77790;//0x88defe0;
            inline constexpr uint32_t RefreshCostumeInfo = 0xDA77870;//0x88df940;
        }

        // 6.4
        namespace VCFlyAttachment {
            inline constexpr uint32_t ChangeFlycloak = 0xE8873E0;//0x8765970;
        }


        /*namespace BaseEntity_VC {
            inline constexpr uint32_t GetVisualComponent_uint = 0x64ea34c;
        }*/

		// 6.4
        namespace DataItem {
            inline constexpr uint32_t HandleNormalProp = 0x90BA400;//0x7C6FB20;
        }
        // 6.3
        /*namespace VCHumanoidMoveData {
            inline constexpr uint32_t tryDoJump = 0x370;
            inline constexpr uint32_t jumpXZVelocityScale = 0x3a8;
            inline constexpr uint32_t tryFlyUp = 0x37e;
            inline constexpr uint32_t tryFlyDown = 0x380;
            inline constexpr uint32_t forceAirStateFly = 0x3a4;
            inline constexpr uint32_t vcHumanoidMove = 0x4f8;
        }*/
        // 6.3
        //namespace VCHumanoidMove {
        //    inline constexpr uint32_t _moveConfig = 0x5f8;
        //    /*       inline constexpr uint32_t yawSpeedRatio = 0x2b0;
        //           inline constexpr uint32_t arcHorizonMoveSpeed = 0x340;
        //           inline constexpr uint32_t arcHorizonMoveAcc = 0x344;
        //           inline constexpr uint32_t arcHorizonMoveAirSpeed = 0x348;
        //           inline constexpr uint32_t arcHorizonMoveAirAcc = 0x34c;
        //           inline constexpr uint32_t overrideFlyMinHeight = 0x5dc;
        //           inline constexpr uint32_t applyFlySpeedRatioInCombatAir = 0x358;*/
        //}

		// 6.4
        namespace HumanoidMoveFSMBaseMoveState {
            inline constexpr uint32_t k__BackingField = 0xE0;//0x14c;
        }

        // 6.4
        namespace VCBillboard {
            inline constexpr uint32_t _forceAddCombat = 0x1C0;//0x1ac; 80 BE ? ? ? ? ? 0F 84 ? ? ? ? 48 8B 8E ? ? ? ? 48 85 C9 0F 84 ? ? ? ? E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 8B 4C 24

            inline constexpr uint32_t NeedHideBillboard = 0x9116DD0;//0x743df70;
            inline constexpr uint32_t UpdateTransform = 0x911CD80;//0x7442d70;
            inline constexpr uint32_t get_billboard = 0x9119050;//0x7443660;
        }

		// 6.4
        namespace VCMoveData {
            inline constexpr uint32_t onWater = 0xA0;//0xA0; public System.Void SetIKLayer(System.Boolean
        }
		// 6.4
        namespace InLevelMapPageContext {
            inline  constexpr uint32_t _mapViewRect = 0x4E0;//0x50c;
            inline constexpr uint32_t _pageMono = 0x2A0;//0x288;

            inline constexpr uint32_t OnMapClicked = 0x9EC4D60;//0xA90C680;
            inline constexpr uint32_t OnMarkClicked = 0x9EC7710;//0xA902480;
        }
		// 6.4
        namespace MonoMapMark {
            inline constexpr uint32_t k__BackingField = 0x20;//0x20;
        }


		// 6.4
        namespace UIManager {
            inline constexpr uint32_t _uiCamera = 0x1E0;//0x50;
        }
    }

    // 6.4
    namespace IL2CPP {
        inline constexpr uint32_t GetTypeInfoFromTypeDefinitionIndex = 0x481000;//0x46F9F0;
        inline constexpr uint32_t il2cpp_class_get_name = 0xACF0;//0x17050;
        inline constexpr uint32_t il2cpp_type_get_name = 0x417240;
        inline constexpr uint32_t il2cpp_class_get_fields = 0x416330;
        inline constexpr uint32_t il2cpp_field_get_name = 0x416760;
        inline constexpr uint32_t il2cpp_field_get_offset = 0x4167A0;
        inline constexpr uint32_t il2cpp_field_get_type = 0x4167B0;
        inline constexpr uint32_t il2cpp_array_new = 0x416240;//0x404D90;
        inline constexpr uint32_t il2cpp_array_new_бля = 0x416260;//0x404DB0;
        inline constexpr uint32_t il2cpp_array_new_full = 0x416270;//0x404DC0;
    }
}
