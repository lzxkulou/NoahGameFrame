// -------------------------------------------------------------------------
//    @FileName			:    NFCAIModule.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2017-02-08
//    @Module           :    NFCAIModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "NFCAIModule.h"
#include "NFCStateMachine.h"
#include "NFCHateModule.h"
#include "NFAIPlugin.h"
#include "NFComm/NFMessageDefine/NFProtocolDefine.hpp"


bool NFCAIModule::Init()
{
    m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>("NFCKernelModule");

    assert(NULL != m_pKernelModule);

    //////////////////////////////////////////////////////////////////////////

    m_pHateModule = new NFCHateModule(pPluginManager);

    //��ȡ���е�״̬������
    mtStateMap.insert(TMAPSTATE::value_type(DeadState, new NFCDeadState(5.0f, pPluginManager)));
    mtStateMap.insert(TMAPSTATE::value_type(FightState, new NFCFightState(1.0f, pPluginManager)));
    mtStateMap.insert(TMAPSTATE::value_type(IdleState, new NFCIdleState(3.0f, pPluginManager)));
    mtStateMap.insert(TMAPSTATE::value_type(PatrolState, new NFCPatrolState(5.0f, pPluginManager)));


    return true;
}

bool NFCAIModule::Shut()
{
    TOBJECTSTATEMACHINE::iterator itObject = mtObjectStateMachine.begin();
    for (itObject; itObject != mtObjectStateMachine.end(); itObject++)
    {
        delete itObject->second;
    }

    TMAPSTATE::iterator itState = mtStateMap.begin();
    for (itState; itState != mtStateMap.end(); itState++)
    {
        delete itState->second;
    }

    delete m_pHateModule;
    m_pHateModule = NULL;

    return true;
}

NFIState* NFCAIModule::GetState(const int eState)
{
    TMAPSTATE::iterator it = mtStateMap.find((NFAI_STATE)eState);
    if (it != mtStateMap.end())
    {
        return it->second;
    }

    return NULL;
}

bool NFCAIModule::CreateAIObject(const NFGUID& self)
{
    //����ֻ��Ϊ���Ժ󷽱�ά��״̬��ʱ�䣬��ԼCPU
    TOBJECTSTATEMACHINE::iterator it = mtObjectStateMachine.find(self);
    if (it == mtObjectStateMachine.end())
    {
        mtObjectStateMachine.insert(TOBJECTSTATEMACHINE::value_type(self, new NFCStateMachine(self, this)));

        return true;
    }

    return false;
}

bool NFCAIModule::DelAIObject(const NFGUID& self)
{
    TOBJECTSTATEMACHINE::iterator it = mtObjectStateMachine.find(self);
    if (it != mtObjectStateMachine.end())
    {
        delete it->second;
        mtObjectStateMachine.erase(it);
        return true;
    }

    return false;
}

bool NFCAIModule::Execute()
{
    TOBJECTSTATEMACHINE::iterator it = mtObjectStateMachine.begin();
    for (it; it != mtObjectStateMachine.end(); it++)
    {
        it->second->Execute();
    }

    return true;
}

NFIStateMachine* NFCAIModule::GetStateMachine(const NFGUID& self)
{
    TOBJECTSTATEMACHINE::iterator it = mtObjectStateMachine.find(self);
    if (it != mtObjectStateMachine.end())
    {
        return it->second;
    }

    return NULL;
}

NFIHateModule* NFCAIModule::GetHateModule()
{
    return m_pHateModule;
}

NFIKernelModule* NFCAIModule::GetKernelModule()
{
    return m_pKernelModule;
}

void NFCAIModule::OnBeKilled(const NFGUID& self, const NFGUID& other)
{
    /*
    ������Ҫչʾһ��״̬ת������Ӧ��
    ��������״̬�⣬��һ��״̬���зǳ���Ĺ���[����˵���е�״̬���ǹ���]
    ����ĳ������[״̬]���ܵ�ĳ���¼���ʱ�򣬽�������һ������[״̬]
    ���������ǿ����õģ���NFIAIRuleConfigModule.h��Ӧ�ÿ��Բ�ѯ�������ת�����
    ��ô�����Ļ������е�NPC�ɿع�����ǲ߻����õĹ���

    Ӧ����˵��״̬�ܹ���Ϊ����״̬
    ��ֹ״̬
    [
    ��������
    ���й���
    ]
    ��Ծ״̬
    [
    Ѳ�߹���
    ս������
    ׷������
    ���Ź���[DoorNpcʹ��]
    ]
    ����״̬
    [
    �Ա�����
    �Ա�����
    ��������
    ]

    ״̬�ڲ����������л�������״̬�����������л�����
    ����������ͬһ��״̬���ܳƣ�����ֻ��һ��ִ����Ϻ��л���������������л���״̬

    �������д�ɵ��ű���ÿ�������Ǽ򵥵��߼�
    Ȼ��NPC�����п���������ӵ�м��ֹ���

    */

    TOBJECTSTATEMACHINE::iterator it = mtObjectStateMachine.find(self);
    if (it != mtObjectStateMachine.end())
    {
        it->second->ChangeState(DeadState);
    }
}

void NFCAIModule::OnBeAttack(const NFGUID& self, const NFGUID& other, const int nDamageValue)
{
    GetHateModule()->AddHate(self, other, nDamageValue);
    GetHateModule()->CompSortList(self);

    NFIStateMachine* pStateMachine = GetStateMachine(self);
    if (pStateMachine)
    {
        if (FightState != pStateMachine->CurrentState())
        {
            pStateMachine->ChangeState(FightState);
        }
    }
}

void NFCAIModule::OnSpring(const NFGUID& self, const NFGUID& other)
{
    //����ְҵ,�ȼ�,Ѫ��,����
    //ս��״ֻ̬�����б��ڵ��ˣ�Ѳ��,����״̬�������Ҷ����
    NF_AI_SUB_TYPE subType = (NF_AI_SUB_TYPE)m_pKernelModule->GetPropertyInt(self, "SubType");
    if (NF_AI_SUB_TYPE::NFAST_INITATIVE == subType)
    {
        //��һ���PET����
        const std::string& strClassName = m_pKernelModule->GetPropertyString(other, "ClassName");
        if ("Player" == strClassName
            || "Pet" == strClassName)
        {
            GetHateModule()->AddHate(self, other, 100);
            GetHateModule()->CompSortList(self);
            NFIStateMachine* pStateMachine = GetStateMachine(self);
            if (pStateMachine)
            {
                if (NFAI_STATE::FightState != pStateMachine->CurrentState())
                {
                    NFIState* pState = GetState(pStateMachine->CurrentState());
                    pState->Execute(self);
                }
            }
        }
    }
}

void NFCAIModule::OnEndSpring(const NFGUID& self, const NFGUID& other)
{
    GetHateModule()->SetHateValue(self, other, 0);
}

void NFCAIModule::OnMotion(const NFGUID& self, int nResults)
{
    //�˶��ɹ�����ʧ�ܣ������������һЩ���������紫�͵�
}

void NFCAIModule::OnSelect(const NFGUID& self, const NFGUID& other)
{
    //ѡ�������ǽ����񰡣������ǶԻ��������Ǹ���
}

void NFCAIModule::OnReload(const char* pstrModuleName, NFIModule* pModule)
{

}

int NFCAIModule::CanUseAnySkill(const NFGUID& self, const NFGUID& other)
{
    return 0;
}

float NFCAIModule::UseAnySkill(const NFGUID& self, const NFGUID& other)
{
    return 0.5f;
}

int NFCAIModule::OnAIObjectEvent(const NFGUID& self, const std::string& strClassNames, const CLASS_OBJECT_EVENT eClassEvent, const NFDataList& var)
{
    if (CLASS_OBJECT_EVENT::COE_DESTROY == eClassEvent)
    {
        DelAIObject(self);
    }
    else if (CLASS_OBJECT_EVENT::COE_CREATE_HASDATA == eClassEvent)
    {
        //m_pEventProcessModule->AddEventCallBack(self, NFED_ON_SERVER_MOVEING, OnMoveRquireEvent);
        CreateAIObject(self);
    }

    return 0;
}

bool NFCAIModule::AfterInit()
{
	m_pKernelModule->AddClassCallBack(NFrame::NPC::ThisName(), this, &NFCAIModule::OnAIObjectEvent);
	m_pKernelModule->AddClassCallBack(NFrame::NPC::ThisName(), this, &NFCAIModule::OnAIObjectEvent);
    return true;
}

bool NFCAIModule::BeforeShut()
{
    return true;
}