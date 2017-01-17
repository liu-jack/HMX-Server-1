#include "ForSClientMsgHandler.h"
#include "../DyNetSocket/NetSocket.h"
#include "../DyNetSocket/NetClient.h"
#include "CommonW2S.h"
#include "CommonS2W.h"
#include "CommonInternal.h"

#include "LogManager.h"
#include "ServerClientCommon.h"

#include "ProcServerHandler.h"
#include "CommonServerOpt.h"
#include "ServerConnectMgr.h"
#include "ServerSession.h"
#include "ClientSession.h"


ForSClientMsgHandler::ForSClientMsgHandler():m_bFirstMsg(true)
{

#define REGISTER_INTERNAL_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ForSClientMsgHandler::handler, this, _1, _2, _3)); \
}

	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_LOGINED,SSRepLogined,RepLogined);	// ��¼��� 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_NOFTTY_SERVERID,SSSessionNofitySInfo,NofityClientSessionInfo); // ͬ����������� ID 
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REQ_CLIENT_EXIT,SSNofityClientExit,NofityClientExit);
	REGISTER_INTERNAL_MESSAGE(PRO_SS_REP_PING_S,SSRepPingS,RepPingToS); // ping 

#undef REGISTER_INTERNAL_MESSAGE

#define REGISTER_S2LOG_MESSAGE(msg_idx,cls,handler)\
	{\
	RegisterMessage(msg_idx, sizeof(cls), boost::bind(&ProcServerHandler::handler, ProcServerHandler::Instance(), _1, _2, _3)); \
	}

	REGISTER_S2LOG_MESSAGE(PRO_S2LOG_MSG,S2LogMsg,ReqSaveToDb);	

#undef REGISTER_S2LOG_MESSAGE

}


ForSClientMsgHandler::~ForSClientMsgHandler()
{
}

// �пͻ������ӻص� 
void ForSClientMsgHandler::OnNetMsgEnter(NetSocket& rSocket)
{
	if(m_bFirstMsg)
	{
		ClientCommonHandler::ForWsOnMsgEnter(rSocket,ESERVER_TYPE_LOG);
		m_bFirstMsg = false;
	}else
	{
		ClientCommonHandler::ForClientToServerOnMsgEnter(rSocket,ESERVER_TYPE_LOG);
	}
}

// �пͻ�����Ϣ�յ� 
void ForSClientMsgHandler::OnNetMsg(NetSocket& rSocket, NetMsgSS* pHead,int32 nSize)
{
	ClientCommonHandler::ForWsOnMsg(*this, rSocket, pHead, nSize);
}

// �пͻ��˶Ͽ��ص�(��ʱsocket�Ѿ�����) 
void ForSClientMsgHandler::OnNetMsgExit(NetSocket& rSocket)
{
	ClientCommonHandler::ForWsOnExit(rSocket);
}


void ForSClientMsgHandler::RepLogined(BaseSession* pBaseSession, const NetMsgSS* pMsg,int32 nSize)
{
	ClientCommonHandler::ForServerLogined(pBaseSession, pMsg);
}

void ForSClientMsgHandler::NofityClientSessionInfo(BaseSession* pSession, const NetMsgSS* pMsg,int32 nSize)
{
	//---------------------------------���������begin-------------------------------
	//ServerCommonHandler::ForServerUpdataClientUpConnectInfo(pSession,pMsg,ESERVER_TYPE_LOG);
	//---------------------------------���������end-------------------------------
}

void ForSClientMsgHandler::NofityClientExit(BaseSession* pSession, const NetMsgSS* pMsg, int32 nSize)
{
	const SSNofityClientExit* pPacket = static_cast<const SSNofityClientExit*>(pMsg);
	int32 nClientSessionID = pPacket->nSessionID;

	ClientSession* pClientSession = ClientSessionMgr::Instance()->GetSession(nClientSessionID);
	ASSERT(pClientSession);

	// �������ݵ����ݿ� 

	// ɾ�� 
	ClientSessionMgr::Instance()->RemoveSession(nClientSessionID);

}

void ForSClientMsgHandler::RepPingToS(BaseSession* pSession, const NetMsgSS* pMsg,int32 nSize)
{
	ServerCommonHandler::ForServerTimeOutCancel(pSession);	// ȡ����ʱ 
}

