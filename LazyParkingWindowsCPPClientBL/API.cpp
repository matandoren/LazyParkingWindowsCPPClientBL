#include "pch.h"
#include <unordered_map>
#include <random>
#include "API.h"
#include "Model.h"
#include "Exceptions.h"

using namespace LazyParkingWindowsCPPClientBL;
//#define _MOCKUP

#if !defined _DEBUG ||  !defined _MOCKUP
static std::unordered_map<MODEL_DESCRIPTOR, Model> descriptorToModelMap;
static std::unordered_map<CALLBACK_DESCRIPTOR, void*> descriptorToCallbackMap;
static std::random_device rd;
static std::mt19937 gen(rd());

#define MODELCALL_BOILERPLATE(CALL) \
	auto iterator = descriptorToModelMap.find(i_Descriptor); \
 \
	if (iterator == descriptorToModelMap.end()) \
	{ \
		return eStatusCode_InvalidDescriptor; \
	} \
 \
	if (iterator->second.CALL) \
	{ \
		return eStatusCode_Success; \
	} \
 \
	return eStatusCode_BadArgumentsOrModelInImproperState;

#define ADDCALLBACK_BOILERPLATE(CALL) \
	auto iterator = descriptorToModelMap.find(i_ModelDescriptor); \
 \
	if (iterator == descriptorToModelMap.end()) \
	{ \
		return eStatusCode_InvalidDescriptor; \
	} \
 \
	o_CallbackDescriptor = generateCallbackDescriptor(); \
	descriptorToCallbackMap[o_CallbackDescriptor] = iterator->second.CALL.AddEventHandler(i_Callback, i_OptionalData); \
 \
	return eStatusCode_Success;

#define REMOVECALLBACK_BOILERPLATE(CALL) \
	auto modelIterator = descriptorToModelMap.find(i_ModelDescriptor); \
	auto callbackIterator = descriptorToCallbackMap.find(i_CallbackDescriptor); \
 \
	if (modelIterator == descriptorToModelMap.end() || callbackIterator == descriptorToCallbackMap.end() || \
		!modelIterator->second.CALL.RemoveEventHandler(callbackIterator->second)) \
	{ \
		return eStatusCode_InvalidDescriptor; \
	} \
 \
	descriptorToCallbackMap.erase(callbackIterator); \
 \
	return eStatusCode_Success; 
#endif

static MODEL_DESCRIPTOR generateModelDescriptor()
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODEL_DESCRIPTOR descriptor;

	do
	{
		descriptor = gen();
	} while (descriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR || descriptorToModelMap.find(descriptor) != descriptorToModelMap.end());

	return descriptor;
#else
	return 0;
#endif
}

static CALLBACK_DESCRIPTOR generateCallbackDescriptor()
{
#if !defined _DEBUG ||  !defined _MOCKUP
	CALLBACK_DESCRIPTOR descriptor;

	do
	{
		descriptor = gen();
	} while (descriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR || descriptorToCallbackMap.find(descriptor) != descriptorToCallbackMap.end());

	return descriptor;
#else
	return 0;
#endif
}

eStatusCode CreateModel(MODEL_DESCRIPTOR& o_ModelDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	o_ModelDescriptor = generateModelDescriptor();

	try
	{
		descriptorToModelMap[o_ModelDescriptor];
	}
	catch (WSAStartupException)
	{
		return eStatusCode_WSAStartupException;
	}

	return eStatusCode_Success;
#else
	return eStatusCode_Success;
#endif
}

eStatusCode CleanupModel(MODEL_DESCRIPTOR i_Descriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	if (descriptorToModelMap.erase(i_Descriptor) == 0)
	{
		return eStatusCode_InvalidDescriptor;
	}
	
	return eStatusCode_Success;
#else
	return eStatusCode_Success;
#endif
}

eStatusCode SetNumberOfParkingSpots(MODEL_DESCRIPTOR i_Descriptor, int i_numberOfParkingSpots)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(SetNumberOfParkingSpots(i_numberOfParkingSpots))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode SetIP(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_IP)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(SetIP(i_IP))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode SetPort(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Port)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(SetPort(i_Port))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode Login(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username, const std::string& i_Password)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	try
	{
		MODELCALL_BOILERPLATE(Login(i_Username, i_Password))
	}
	catch (SocketConnectionException)
	{
		return eStatusCode_SocketConnectionException;
	}
#else
	return eStatusCode_Success;
#endif
}

eStatusCode Logout(MODEL_DESCRIPTOR i_Descriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(Logout())
#else
	return eStatusCode_Success;
#endif
}

eStatusCode OpenGate(MODEL_DESCRIPTOR i_Descriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(OpenGate())
#else
	return eStatusCode_Success;
#endif
}

eStatusCode CancelReservation(MODEL_DESCRIPTOR i_Descriptor, int i_ParkingSpotID)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(CancelReservation(i_ParkingSpotID))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode ChangePassword(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username, const std::string& i_Password)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(ChangePassword(i_Username, i_Password))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode DeleteDriver(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(DeleteDriver(i_Username))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddDriver(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Name, const std::string& i_Password, time_t i_ExpirationDate)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(AddDriver(i_Name, i_Password, i_ExpirationDate))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode UpdateDriverExpiration(MODEL_DESCRIPTOR i_Descriptor, const std::string& i_Username, time_t i_ExpirationDate)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(UpdateDriverExpiration(i_Username, i_ExpirationDate))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode ReserveParkingSpot(MODEL_DESCRIPTOR i_Descriptor, int i_ParkingSpotID, const std::string& i_ReservedFor, time_t i_ExpirationDate)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(ReserveParkingSpot(i_ParkingSpotID, i_ReservedFor, i_ExpirationDate))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RequestExpiredUsers(MODEL_DESCRIPTOR i_Descriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(RequestExpiredUsers())
#else
	return eStatusCode_Success;
#endif
}

eStatusCode GetNextExpiredUser(MODEL_DESCRIPTOR i_Descriptor, std::string& o_Username, time_t& o_ExpirationDate)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(GetNextExpiredUser(o_Username, o_ExpirationDate))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode GetParkingSpotStatus(MODEL_DESCRIPTOR i_Descriptor, int i_ID, bool& o_IsOccupied, bool& o_IsReserved, std::string& o_ReservedFor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	MODELCALL_BOILERPLATE(GetParkingSpotStatus(i_ID, o_IsOccupied, o_IsReserved, o_ReservedFor))
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddLoginReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, int, bool, bool), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(LoginReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddLogoutReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(LogoutReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddOpenGateReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, bool), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(OpenGateReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddCancelReservationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, bool, bool), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(CancelReservationReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddChangePasswordReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, bool, bool), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(ChangePasswordReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddDeleteDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, bool, bool), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(DeleteDriverReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddAddDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, bool, bool, const std::string&, int), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(AddDriverReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddUpdateDriverExpirationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, bool, bool, bool), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(UpdateDriverExpirationReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddReserveParkingSpotReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, bool, bool, bool), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(ReserveParkingSpotReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddRequestExpiredUsersReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, int), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(RequestExpiredUsersReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode AddParkingStatusUpdateEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, void(*i_Callback)(void*, int, bool, bool, const std::string&), void* i_OptionalData, CALLBACK_DESCRIPTOR& o_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	ADDCALLBACK_BOILERPLATE(ParkingStatusUpdate)
#else
	return eStatusCode_Success;
#endif
}


eStatusCode RemoveLoginReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(LoginReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveLogoutReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(LogoutReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveOpenGateReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(OpenGateReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveCancelReservationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(CancelReservationReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveChangePasswordReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(ChangePasswordReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveDeleteDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(DeleteDriverReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveAddDriverReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(AddDriverReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveUpdateDriverExpirationReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(UpdateDriverExpirationReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveReserveParkingSpotReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(ReserveParkingSpotReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveRequestExpiredUsersReplyEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(RequestExpiredUsersReply)
#else
	return eStatusCode_Success;
#endif
}

eStatusCode RemoveParkingStatusUpdateEventHandler(MODEL_DESCRIPTOR i_ModelDescriptor, CALLBACK_DESCRIPTOR i_CallbackDescriptor)
{
#if !defined _DEBUG ||  !defined _MOCKUP
	REMOVECALLBACK_BOILERPLATE(ParkingStatusUpdate)
#else
	return eStatusCode_Success;
#endif
}
